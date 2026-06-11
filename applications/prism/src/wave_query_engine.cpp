#include "wave_query_engine.h"

#include <algorithm>

WaveQueryEngine::WaveQueryEngine(const WaveformDocument* document)
   : m_document(document)
{
   rebuildSignalMaps();
}

void WaveQueryEngine::setDocument(const WaveformDocument* document)
{
   m_document = document;
   rebuildSignalMaps();
   m_boundSignals.clear();
}

WaveQueryEngine::WaveQuerySearchResult WaveQueryEngine::search(const WaveQueryNode* root)
{
   WaveQuerySearchResult result;

   if (!m_document)
   {
      result.error = "No waveform document";
      return result;
   }

   auto bindResult = bind(root);
   if (!bindResult.ok)
   {
      result.error = bindResult.error;
      return result;
   }

   QVector<qreal> times;
   collectBoundTransitionTimes(times);

   if (times.size() < 2)
   {
      result.ok = true;
      return result;
   }

   for (int i = 0; i + 1 < times.size(); ++i)
   {
      const qreal t0 = times[i];
      const qreal t1 = times[i + 1];

      if (t1 <= t0)
         continue;

      // VCD-style semantics: value at t0 is valid for [t0, t1).
      // Since WaveSignal::valueAtTime(t0) should return the segment beginning at t0,
      // sampling at t0 is normally correct.
      const qreal sampleTime = t0;

      QString error;
      const bool matched = evaluateBool(root, sampleTime, &error);

      if (!error.isEmpty())
      {
         result.error = error;
         return result;
      }

      if (matched)
         appendMatch(result.matches, t0, t1);
   }

   result.ok = true;
   return result;
}

bool WaveQueryEngine::evaluateBool(const WaveQueryNode* root,
                                   qreal time,
                                   QString* error) const
{
   const WaveQueryValue value = evaluateValue(root, time, error);
   return value.toBool();
}

WaveQueryEngine::BindResult WaveQueryEngine::bind(const WaveQueryNode* root)
{
   BindResult result;

   m_boundSignals.clear();

   if (!root)
   {
      result.error = "No query expression";
      return result;
   }

   QSet<QString> ids;
   collectIdentifiers(root, ids);

   for (const QString& id : ids)
   {
      QString error;
      WaveSignal* sig = resolveIdentifier(id, &error);

      if (!sig)
      {
         result.error = error;
         return result;
      }

      m_boundSignals.insert(id, sig);
   }

   result.ok = true;
   return result;
}

void WaveQueryEngine::rebuildSignalMaps()
{
   m_signalByFullPath.clear();
   m_signalByShortName.clear();

   if (!m_document)
      return;

   for (const auto& sig : m_document->signalList)
   {
      if (!sig)
         continue;

      m_signalByFullPath.insert(sig->fullPath, sig.get());
      m_signalByShortName[sig->name].push_back(sig.get());
   }
}

void WaveQueryEngine::collectBoundTransitionTimes(QVector<qreal>& times) const
{
   if (!m_document)
      return;

   times.push_back(0.0);
   times.push_back(m_document->endTime());

   QSet<WaveSignal*> seen;

   for (auto it = m_boundSignals.begin(); it != m_boundSignals.end(); ++it)
   {
      WaveSignal* sig = it.value();
      if (!sig || seen.contains(sig))
         continue;

      seen.insert(sig);

      for (const WaveSegment& seg : sig->segments)
      {
         times.push_back(seg.startTime);
         times.push_back(seg.endTime);
      }
   }

   std::sort(times.begin(), times.end());

   QVector<qreal> uniqueTimes;
   uniqueTimes.reserve(times.size());

   constexpr qreal Eps = 1e-12;

   for (qreal t : times)
   {
      if (uniqueTimes.isEmpty() || std::abs(uniqueTimes.back() - t) > Eps)
         uniqueTimes.push_back(t);
   }

   times = std::move(uniqueTimes);
}

void WaveQueryEngine::addUniqueSortedTime(QVector<qreal>& times, qreal t) const
{
   times.push_back(t);
}

void WaveQueryEngine::appendMatch(QVector<WaveQueryMatch>& matches,
                                  qreal startTime,
                                  qreal endTime) const
{
   if (endTime <= startTime)
      return;

   constexpr qreal Eps = 1e-12;

   if (!matches.isEmpty() &&
       std::abs(matches.back().endTime - startTime) <= Eps)
   {
      matches.back().endTime = endTime;
      return;
   }

   matches.push_back({startTime, endTime});
}

void WaveQueryEngine::collectIdentifiers(const WaveQueryNode* node, QSet<QString>& ids) const
{
   if (!node)
      return;

   if (node->kind == WaveQueryNodeKind::Identifier)
      ids.insert(node->text);

   collectIdentifiers(node->left.get(), ids);
   collectIdentifiers(node->right.get(), ids);
}

WaveSignal* WaveQueryEngine::resolveIdentifier(const QString& id, QString* error) const
{
   if (m_signalByFullPath.contains(id))
      return m_signalByFullPath.value(id);

   const QVector<WaveSignal*> matches = m_signalByShortName.value(id);

   if (matches.size() == 1)
      return matches.first();

   if (matches.size() > 1)
   {
      if (error)
         *error = QString("Signal name '%1' is ambiguous; use full path").arg(id);
      return nullptr;
   }

   if (error)
      *error = QString("Unknown signal '%1'").arg(id);

   return nullptr;
}

WaveQueryValue WaveQueryEngine::evaluateValue(const WaveQueryNode* node,
                                              qreal time,
                                              QString* error) const
{
   if (!node)
   {
      if (error)
         *error = "Invalid query node";
      return WaveQueryValue::invalid();
   }

   switch (node->kind)
   {
   case WaveQueryNodeKind::Identifier:
   {
      WaveSignal* sig = m_boundSignals.value(node->text, nullptr);
      if (!sig)
      {
         if (error)
            *error = QString("Unbound signal '%1'").arg(node->text);
         return WaveQueryValue::invalid();
      }

      qDebug() << "getting value for id: " << node->text;
      return valueFromSignal(sig, time);
   }

   case WaveQueryNodeKind::Number:
      return WaveQueryValue::intValue(node->number, node->text);

   case WaveQueryNodeKind::Literal:
      return valueFromString(node->text);

   case WaveQueryNodeKind::UnaryNot:
   {
      const WaveQueryValue v = evaluateValue(node->left.get(), time, error);
      if (!v.valid || v.unknown)
         return WaveQueryValue::unknownValue(v.raw);

      return WaveQueryValue::intValue(v.integer == 0 ? 1 : 0);
   }

   case WaveQueryNodeKind::LogicalAnd:
   {
      const WaveQueryValue lhs = evaluateValue(node->left.get(), time, error);
      if (!lhs.valid)
         return lhs;

      if (!lhs.unknown && lhs.integer == 0)
         return WaveQueryValue::intValue(0);

      const WaveQueryValue rhs = evaluateValue(node->right.get(), time, error);
      if (!rhs.valid)
         return rhs;

      if (lhs.unknown || rhs.unknown)
         return WaveQueryValue::unknownValue();

      return WaveQueryValue::intValue(rhs.integer != 0 ? 1 : 0);
   }

   case WaveQueryNodeKind::LogicalOr:
   {
      const WaveQueryValue lhs = evaluateValue(node->left.get(), time, error);
      if (!lhs.valid)
         return lhs;

      if (!lhs.unknown && lhs.integer != 0)
         return WaveQueryValue::intValue(1);

      const WaveQueryValue rhs = evaluateValue(node->right.get(), time, error);
      if (!rhs.valid)
         return rhs;

      if (lhs.unknown || rhs.unknown)
         return WaveQueryValue::unknownValue();

      return WaveQueryValue::intValue(rhs.integer != 0 ? 1 : 0);
   }

   case WaveQueryNodeKind::Equal:
   case WaveQueryNodeKind::NotEqual:
   case WaveQueryNodeKind::Less:
   case WaveQueryNodeKind::LessEqual:
   case WaveQueryNodeKind::Greater:
   case WaveQueryNodeKind::GreaterEqual:
   {
      const WaveQueryValue lhs = evaluateValue(node->left.get(), time, error);
      const WaveQueryValue rhs = evaluateValue(node->right.get(), time, error);

      if (!lhs.valid)
         return lhs;
      if (!rhs.valid)
         return rhs;

      if (lhs.unknown || rhs.unknown)
         return WaveQueryValue::unknownValue();

      bool result = false;

      switch (node->kind)
      {
      case WaveQueryNodeKind::Equal:
         qDebug() << "equal: " << lhs.integer << " == " << rhs.integer;
         result = lhs.integer == rhs.integer;
         break;

      case WaveQueryNodeKind::NotEqual:
         result = lhs.integer != rhs.integer;
         break;

      case WaveQueryNodeKind::Less:
         result = lhs.integer < rhs.integer;
         break;

      case WaveQueryNodeKind::LessEqual:
         result = lhs.integer <= rhs.integer;
         break;

      case WaveQueryNodeKind::Greater:
         result = lhs.integer > rhs.integer;
         break;

      case WaveQueryNodeKind::GreaterEqual:
         result = lhs.integer >= rhs.integer;
         break;

      default:
         break;
      }

      return WaveQueryValue::intValue(result ? 1 : 0);
   }
   }

   if (error)
      *error = "Unsupported query node";

   return WaveQueryValue::invalid();
}

WaveQueryValue WaveQueryEngine::valueFromSignal(WaveSignal* signal, qreal time) const
{
   if (!signal)
      return WaveQueryValue::invalid();

   qDebug() << "value at time: " << time << " is: " << signal->valueAtTime(time);

   return valueFromString(signal->valueAtTime(time));
}

WaveQueryValue WaveQueryEngine::valueFromString(const QString& value) const
{
   QString s = value.trimmed();

   if (s.isEmpty())
      return WaveQueryValue::unknownValue();

   s = s.toUpper();

   if (s.contains('X') || s.contains('Z'))
      return WaveQueryValue::unknownValue(s);

   if (s.startsWith("0X"))
   {
      bool ok = false;
      const qint64 v = s.mid(2).toLongLong(&ok, 16);
      return ok ? WaveQueryValue::intValue(v, value) : WaveQueryValue::unknownValue(value);
   }

   if (s.startsWith("0B"))
   {
      bool ok = false;
      const qint64 v = s.mid(2).toLongLong(&ok, 2);
      return ok ? WaveQueryValue::intValue(v, value) : WaveQueryValue::unknownValue(value);
   }

   bool isBinary = true;
   for (const QChar c : s)
   {
      if (c != '0' && c != '1')
      {
         isBinary = false;
         break;
      }
   }

   if (isBinary)
   {
      bool ok = false;
      const qint64 v = s.toLongLong(&ok, 2);
      return ok ? WaveQueryValue::intValue(v, value) : WaveQueryValue::unknownValue(value);
   }

   bool ok = false;
   const qint64 decimal = s.toLongLong(&ok, 10);
   if (ok)
      return WaveQueryValue::intValue(decimal, value);

   return WaveQueryValue::unknownValue(value);
}