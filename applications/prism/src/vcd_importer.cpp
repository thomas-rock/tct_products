#include "vcd_importer.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

std::unique_ptr<WaveformDocument> VcdImporter::importFile(const QString& path,
                                                          QString* errorMessage)
{
    reset();

    if (!parseFile(path, errorMessage))
        return nullptr;

    finalizeDocument();
    return std::move(m_document);
}

void VcdImporter::reset()
{
    m_document = std::make_unique<WaveformDocument>();
    m_scopeStack.clear();
    m_currentTime = 0.0;
    m_lastTime = 0.0;
    m_varsById.clear();
    m_changesById.clear();
    m_designNodeByPath.clear();
    m_timescaleFactor = 1.0; // default: assume ns if not specified
}

bool VcdImporter::parseFile(const QString& path, QString* errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Could not open VCD file: %1").arg(path);
        return false;
    }

    QTextStream in(&file);

    bool inDefinitions = true;

    while (!in.atEnd())
    {
        const QString rawLine = in.readLine();
        const QString line = rawLine.trimmed();

        if (line.isEmpty())
            continue;

        if (inDefinitions)
        {
            processDirectiveLine(line);

            if (line.startsWith("$enddefinitions"))
                inDefinitions = false;

            continue;
        }

        if (line.startsWith('$'))
        {
            processDirectiveLine(line);
        }
        else if (line.startsWith('#'))
        {
           bool ok = false;
           const qreal t = line.mid(1).toDouble(&ok);
           if (ok)
           {
             const qreal scaled = t * m_timescaleFactor;

              m_currentTime = scaled;
              m_lastTime = std::max(m_lastTime, scaled);
           }
        }
        else
        {
            processValueChangeLine(line);
        }
    }

    return true;
}

void VcdImporter::processDirectiveLine(const QString& line)
{
    if (line.startsWith("$scope"))
    {
        // Example: $scope module top $end
        const QStringList parts = line.split(QRegularExpression("\\s+"),
                                             Qt::SkipEmptyParts);
        if (parts.size() >= 3)
            m_scopeStack.push_back(parts[2]);
    }
    else if (line.startsWith("$upscope"))
    {
        if (!m_scopeStack.isEmpty())
            m_scopeStack.removeLast();
    }
    else if (line.startsWith("$var"))
    {
        // Example: $var wire 1 ! clk $end
        const QStringList parts = line.split(QRegularExpression("\\s+"),
                                             Qt::SkipEmptyParts);
        if (parts.size() >= 5)
        {
            ParsedVar var;
            var.width = parts[2].toInt();
            var.idCode = parts[3];
            var.name = parts[4];
            var.scopePath = currentScopePath();

            m_varsById.insert(var.idCode, var);
        }
    }

    else if (line.startsWith("$timescale"))
    {
       // Example formats:
       // $timescale 1ns $end
       // $timescale 10 ps $end
       QString tmp = line;
       tmp.remove("$timescale");
       tmp.remove("$end");
       tmp = tmp.trimmed();

       // Normalize spacing
       QStringList parts = tmp.split(QRegularExpression("\\s+"),
                                     Qt::SkipEmptyParts);

       if (parts.isEmpty())
          return;

       QString numberPart;
       QString unitPart;

       if (parts.size() == 1)
       {
          // e.g. "1ns"
          const QString s = parts[0];
          int i = 0;
          while (i < s.size() && s[i].isDigit())
             ++i;

          numberPart = s.left(i);
          unitPart = s.mid(i);
       }
       else
       {
          // e.g. "10 ps"
          numberPart = parts[0];
          unitPart = parts[1];
       }

       bool ok = false;
       const qreal multiplier = numberPart.toDouble(&ok);
       if (!ok)
          return;

       // Convert unit to seconds
       qreal unitSeconds = 1e-9; // default ns

       if (unitPart == "s")
          unitSeconds = 1.0;
       else if (unitPart == "ms")
          unitSeconds = 1e-3;
       else if (unitPart == "us")
          unitSeconds = 1e-6;
       else if (unitPart == "ns")
          unitSeconds = 1e-9;
       else if (unitPart == "ps")
          unitSeconds = 1e-12;
       else if (unitPart == "fs")
          unitSeconds = 1e-15;

       // Convert to nanoseconds
       const qreal unitNs = unitSeconds / 1e-9;

       m_timescaleFactor = multiplier * unitNs;
    }
}

void VcdImporter::processValueChangeLine(const QString& line)
{
    if (line.isEmpty())
        return;

    // Scalar change: 0!, 1!, x!, z!
    if (line[0] != 'b' && line[0] != 'B')
    {
        if (line.size() >= 2)
        {
            const QString idCode = line.mid(1);
            const QString value = normalizeScalarValue(line[0]);

            m_changesById[idCode].push_back({m_currentTime, value});
        }
        return;
    }

    // Vector change: b1010 !
    const QString payload = line.mid(1).trimmed();
    const int spaceIndex = payload.indexOf(' ');
    if (spaceIndex < 0)
        return;

    const QString valuePart = payload.left(spaceIndex).trimmed();
    const QString idCode = payload.mid(spaceIndex + 1).trimmed();

    const QString value = normalizeVectorValue(valuePart);
    m_changesById[idCode].push_back({m_currentTime, value});
}

QString VcdImporter::currentScopePath() const
{
    return m_scopeStack.join('.');
}

WaveSignalKind VcdImporter::signalKindForWidth(int width) const
{
    return (width <= 1) ? WaveSignalKind::Bit : WaveSignalKind::Bus;
}

QString VcdImporter::normalizeScalarValue(QChar c) const
{
    switch (c.toLatin1())
    {
    case '0': return "0";
    case '1': return "1";
    case 'x':
    case 'X': return "X";
    case 'z':
    case 'Z': return "Z";
    default:  return QString(c).toUpper();
    }
}

QString VcdImporter::normalizeVectorValue(const QString& v) const
{
    return v.toUpper();
}

QString VcdImporter::defaultUnknownValue(const ParsedVar& var) const
{
   if (var.width <= 1)
      return "X";

   return QString(var.width, QChar('X'));
}

void VcdImporter::finalizeDocument()
{
   auto ensureDesignNode = [this](const QString& scopePath) -> DesignNode*
   {
      if (scopePath.isEmpty())
         return nullptr;

      if (m_designNodeByPath.contains(scopePath))
         return m_designNodeByPath.value(scopePath);

      const QStringList parts = scopePath.split('.', Qt::SkipEmptyParts);

      QString accum;
      DesignNode* parent = nullptr;
      DesignNode* current = nullptr;

      for (const QString& part : parts)
      {
         accum = accum.isEmpty() ? part : accum + "." + part;

         if (m_designNodeByPath.contains(accum))
         {
            current = m_designNodeByPath.value(accum);
            parent = current;
            continue;
         }

         current = m_document->createDesignNode(part, accum);
         m_designNodeByPath.insert(accum, current);

         if (parent)
            parent->children.push_back(current);
         else
            m_document->addHierarchyRoot(current);

         parent = current;
      }

      return current;
   };

   for (auto it = m_varsById.begin(); it != m_varsById.end(); ++it)
   {
      const ParsedVar& var = it.value();

      WaveSignal* sig =
         m_document->createSignal(var.name,
                                  signalKindForWidth(var.width),
                                  var.width,
                                  var.scopePath);

      DesignNode* node = ensureDesignNode(var.scopePath);
      if (node)
         node->signalList.push_back(sig);

      const QVector<ParsedChange> changes = m_changesById.value(var.idCode);

      // No changes at all: show unknown for the full sim range.
      if (changes.isEmpty())
      {
         if (m_lastTime > 0.0)
            sig->segments.push_back({0.0, m_lastTime, defaultUnknownValue(var)});
         continue;
      }

      // First change occurs after time 0: fill leading gap with unknown.
      if (changes.first().time > 0.0)
      {
         sig->segments.push_back({
            0.0,
            changes.first().time,
            defaultUnknownValue(var)
         });
      }

      // Build segments from changes.
      for (int i = 0; i < changes.size(); ++i)
      {
         const qreal start = changes[i].time;
         const qreal end = (i + 1 < changes.size()) ? changes[i + 1].time : m_lastTime;

         if (end > start)
            sig->segments.push_back({start, end, changes[i].value});
      }
   }
}