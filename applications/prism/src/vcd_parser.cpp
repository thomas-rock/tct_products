#include "vcd_parser.h"

#include <QFile>

// -----------------------------------------------------------------------------
// VCD Parser
// -----------------------------------------------------------------------------

VcdParser::VcdParser(QTextStream& in) : QObject(),  m_in(in)
{
   m_filename = qobject_cast<QFile*>(m_in.device())->fileName();
   m_errors   = 0;
   m_warnings = 0;
}

std::unique_ptr<WaveformDocument> VcdParser::parse()
{
   m_document = std::make_unique<WaveformDocument>();

   if (!eof()) parseDeclarationCommands();
   if (!eof()) parseSimulationCommands();

   MESSAGE(SysDebug, "parse complete");

   return std::move(m_document);
}

void VcdParser::parseDeclarationCommands ()
{
   m_in_definitions = true;

   while (!eof() && m_in_definitions) {
      QString token = parseToken();
      MESSAGE(SysDebug, QString("next declaration: %1").arg(token));
      if      (token == QStringLiteral("$comment"))          parseComment();
      else if (token == QStringLiteral("$date"))             parseDate();
      else if (token == QStringLiteral("$scope"))            parseScope();
      else if (token == QStringLiteral("$timescale"))        parseTimescale();
      else if (token == QStringLiteral("$upscope"))          parseUpscope();
      else if (token == QStringLiteral("$var"))              parseVar();
      else if (token == QStringLiteral("$version"))          parseVersion();
      else if (token == QStringLiteral("$enddefinitions"))   parseEndDefinition();
      else MESSAGE(SysError, QString("Unknown VCD declaration command: '%1'").arg(token), m_filename, m_line, m_column);
   }
}

void VcdParser::parseSimulationCommands ()
{
   while (!eof()) {
      skipWhitespace();
      if (peek() == '#') {
         get();
         parseSimulationTime();
      }
      else if (peek() != '$') {
         parseValueChange();
      }
      else {
         QString token = parseToken();
         MESSAGE(SysDebug, QString("next simulation command: %1").arg(token));
         if      (token == QStringLiteral("$dumpall"))          parseDumpall();
         else if (token == QStringLiteral("$dumpoff"))          parseDumpoff();
         else if (token == QStringLiteral("$dumpon"))           parseDumpon();
         else if (token == QStringLiteral("$dumpvars"))         parseDumpvars();
         else if (token == QStringLiteral("$comment"))          parseComment();
         else MESSAGE(SysError, QString("Unknown VCD simulation command: '%1'").arg(token), m_filename, m_line, m_column);
      }
   }
}

//------ declaration command parse functions ----------------------------
void VcdParser::parseComment ()
{
   // '$comment' comment_text '$end'

   // comments are ignored
   parseTextUntilEnd();
}

void VcdParser::parseDate ()
{
   // '$date' date_text '$end'

   // dates are ignored
   parseTextUntilEnd();
}

void VcdParser::parseScope ()
{
   // '$scope' scope_type scope_identifier '$end'

   QString scope_type = parseToken();        // Not used  but verified
   if (scope_type != QStringLiteral("begin") &&
       scope_type != QStringLiteral("fork")  &&
       scope_type != QStringLiteral("function")  &&
       scope_type != QStringLiteral("module")  &&
       scope_type != QStringLiteral("task"))
      MESSAGE(SysError, QString("Unknown scope type: '%1'").arg(scope_type), m_filename, m_line, m_column);

   QString scope_id = parseToken();
   QString fullPath = (m_scopeStack.isEmpty()) ? scope_id : m_scopeStack.last()->fullPath + "." + scope_id;
   auto* node = m_document->createDesignNode(scope_id, fullPath);
   if (m_scopeStack.isEmpty())
      m_document->addHierarchyRoot(node);
   else
      m_scopeStack.last()->children.push_back(node);

   m_scopeStack.push_back(node);

   expectKeyword(QStringLiteral("$end"));
}

void VcdParser::parseTimescale ()
{
   // '$timescale' time_number time_unit '$end'

   int time_number = parseDecimalNumber();
   if (time_number != 1 && time_number != 10 && time_number != 100) {
      MESSAGE(SysError, QString("Unexpected timescale time number: '%1'. Expected 1, 10, or 100").arg(time_number), m_filename, m_line, m_column);
      time_number = 1;
   }

   qreal unit_seconds = 1e-9; // default ns
   QString time_unit = parseToken();
   if      (time_unit == QStringLiteral("s"))  unit_seconds = 1.0;
   else if (time_unit == QStringLiteral("ms")) unit_seconds = 1.0e-3;
   else if (time_unit == QStringLiteral("us")) unit_seconds = 1.0e-6;
   else if (time_unit == QStringLiteral("ns")) unit_seconds = 1.0e-9;
   else if (time_unit == QStringLiteral("ps")) unit_seconds = 1.0e-12;
   else if (time_unit == QStringLiteral("fs")) unit_seconds = 1.0e-15;
   else
      MESSAGE(SysError, QString("Unknown time unit '%1'. Expected 's', 'ms', 'us', 'ns', 'ps' or 'fs'").arg(time_unit), m_filename, m_line, m_column);

   const qreal unit_ns = unit_seconds / 1.0e-9;

   m_timescaleFactor = time_number * unit_ns;

   expectKeyword(QStringLiteral("$end"));
}

void VcdParser::parseUpscope ()
{
   // '$upscope' '$end'

   if (!m_scopeStack.isEmpty())
      m_scopeStack.removeLast();

   expectKeyword(QStringLiteral("$end"));
}

void VcdParser::parseVar ()
{
   // '$var' var_type size identifier_code reference '$end'

   QString var_type = parseToken();
   int size         = parseDecimalNumber();
   QString id_code  = parseToken();

   // reference
   //    : identifier
   //    | identifier '[' bit_select_index ']'
   //    | identifier '[' msb_index ':' lsb_index ']'
   QString ref_id      = parseToken();
   int msb_select      = 0;
   int lsb             = 0;
   skipWhitespace();
   if (peek() == '[') {
      get();
      msb_select = parseDecimalNumber();
      lsb = msb_select;
      skipWhitespace();
      if (peek() == ':') {
         get();
         lsb = parseDecimalNumber();
         skipWhitespace();
      }
      QChar c = get();
      if (c != ']')
         MESSAGE(SysError, QString("Unexpected token '%1'. Expected ']'").arg(c), m_filename, m_line, m_column);
   }
   expectKeyword(QStringLiteral("$end"));

   const int index = m_signals.size();
   m_idToIndex.insert(id_code, index);
   SignalBuildState state;
   state.signal = m_document->createSignal(ref_id,
                                           (size <= 1) ?  WaveSignalKind::Bit : WaveSignalKind::Bus,
                                           size,
                                           m_scopeStack.last()->fullPath);
   m_scopeStack.last()->signalList.push_back(state.signal);
   m_signals.push_back(state);
}

void VcdParser::parseVersion ()
{
   // '$version' text_string '$end'

   // versions are ignored
   parseTextUntilEnd();
}

void VcdParser::parseEndDefinition ()
{
   // '$enddefinitions' '$end'

   expectKeyword(QStringLiteral("$end"));
   m_in_definitions = false;
}

//------ simulation command parse functions ----------------------------
void VcdParser::parseDumpall()
{
   // '$dumpall' value_change+ '$end'

   parseValueChangeUntilEnd();
}

void VcdParser::parseDumpoff()
{
   // '$dumpoff' value_change+ '$end'

   parseValueChangeUntilEnd();
}

void VcdParser::parseDumpon()
{
   // '$dumpon' value_change+ '$end'

   parseValueChangeUntilEnd();
}

void VcdParser::parseDumpvars()
{
   // '$dumpvars' value_change+ '$end'

   parseValueChangeUntilEnd();
}

void VcdParser::parseSimulationTime()
{
   // '#' decimal_number
   m_currentTime = parseTimeValue();
   debugMessage(QString("new simulation time: %1").arg(m_currentTime));
}

void VcdParser::parseValueChange()
{
   debugMessage("parsing value change");
   skipWhitespace();

   QString value;
   QString idCode;
   QString radix;

   const QChar c = peek().toLower();
   if (c == QLatin1Char('0') ||
       c == QLatin1Char('1') ||
       c == QLatin1Char('x') ||
       c == QLatin1Char('z')) {

      value  = get();
      idCode = parseToken();

      if (idCode.isEmpty())
         errorMessage(QString("Expected identifier code after scalar value"));
   }

   else if (c == QLatin1Char('b') ||
            c == QLatin1Char('r')) {

      radix  = get();
      value  = parseToken();
      idCode = parseToken();
      if (idCode.isEmpty())
         errorMessage(QString("Expected identifier code after signal value"));
   }

   // insert value change into document
   const auto it = m_idToIndex.constFind(idCode);
   if (it == m_idToIndex.constEnd())
      return;

   SignalBuildState& state = m_signals[*it];
   if (c == 'b' && value.size() < state.signal->width)
      value.prepend(QString(state.signal->width-value.size(), '0'));

   // First value ever seen for this signal.
   if (!state.hasValue)
   {
      // insert initial segment
      if (m_currentTime != 0.0) {
         state.signal->segments.push_back({
            0.0,
            m_currentTime,
            QString(state.signal->width, 'x')
         });
      }
      state.currentValue = value;
      state.currentStartTime = m_currentTime;
      state.hasValue = true;
      return;
   }

   // Same value: no new segment needed.
   if (state.currentValue == value)
      return;

   // Close previous segment.
   state.signal->segments.push_back({
      state.currentStartTime,
      m_currentTime,
      state.currentValue
   });

   // Start new segment.
   state.currentValue = value;
   state.currentStartTime = m_currentTime;
}

void VcdParser::parseValueChangeUntilEnd()
{
   while (!eof()) {
      parseValueChange();
      skipWhitespace();
      if (peek() == '$') {
         expectKeyword(QStringLiteral("$end"));
         break;
      }
   }
}

//------ low-level parse functions ----------------------------
// Reads text until a standalone $end token. The $end token is consumed.
QString VcdParser::parseTextUntilEnd()
{
   skipWhitespace();

   QString text;

   while (!eof()) {
      if (peek() == QLatin1Char('$')) {
         const QString token = parseToken();
         if (token == QStringLiteral("$end")) {
            return text.trimmed();
         }

         text.append(token);
         continue;
      }

      text.append(get());
   }

   errorMessage(QString("Expected '$end'"));
   return text.trimmed();
}

void VcdParser::expectKeyword(const QString& keyword)
{
   const QString token = parseToken();
   if (token != keyword) {
      errorMessage(QString("Unexpected keyword '%1'. Expected '%2'").arg(token, keyword));
   }
}

int VcdParser::parseDecimalNumber()
{
   skipWhitespace();

   QString digits;
   while (!eof() && peek().isDigit())
      digits.append(get());

   if (digits.isEmpty())
      errorMessage(QString("Expected decimal number"));

   bool ok = false;
   const int value = digits.toInt(&ok, 10);
   if (!ok) {
      errorMessage(QString("Invalid decimal number '%1'").arg(digits));
   }

   return value;
}

quint64 VcdParser::parseTimeValue()
{
   skipWhitespace();

   QString digits;
   while (!eof() && peek().isDigit())
      digits.append(get());

   if (digits.isEmpty())
      errorMessage(QString("Expected decimal number"));

   bool ok = false;
   const quint64 value = digits.toULongLong(&ok, 10);
   if (!ok)
      errorMessage(QString("Invalid simulation time '%1'").arg(digits));

   return value;
}



//------ tokenizer functions ----------------------------
void VcdParser::skipWhitespace()
{
   while (!eof() && peek().isSpace())
      get();
}

QString VcdParser::parseToken()
{
   skipWhitespace();

   if (eof()) {
      errorMessage(QString("Unexpected end-of-file"));
      return QString();
   }

   QString token;
   while (!eof() && !peek().isSpace())
      token.append(get());

   return token;
}

bool VcdParser::eof()
{
   if (m_hasLookahead) {
      return false;
   }
   return m_in.atEnd();
}

QChar VcdParser::peek()
{
   if (!m_hasLookahead) {
      if (m_in.atEnd()) {
         return QChar();
      }

      m_in >> m_lookahead;
      m_hasLookahead = true;
   }
   return m_lookahead;
}

QChar VcdParser::get()
{
   QChar c;

   if (m_hasLookahead) {
      c = m_lookahead;
      m_hasLookahead = false;
      m_lookahead = QChar();
   }
   else {
      if (m_in.atEnd()) {
         errorMessage(QString("Unexpected end-of-file"));
      }
      m_in >> c;
   }

   ++m_offset;

   if (c == QLatin1Char('\n')) {
      ++m_line;
      m_column = 1;
   }
   else {
      ++m_column;
   }

   return c;
}

//------ utility functions ----------------------------
void VcdParser::debugMessage (const QString& msg, bool noloc)
{
   if (noloc) MESSAGE(SysDebug, msg);
   else       MESSAGE(SysDebug, msg, m_filename, m_line, m_column);
}

void VcdParser::errorMessage (const QString& msg, bool noloc)
{
   if (noloc) MESSAGE(SysError, msg);
   else       MESSAGE(SysError, msg, m_filename, m_line, m_column);
   m_errors++;
}

void VcdParser::warningMessage (const QString& msg, bool noloc)
{
   if (noloc) MESSAGE(SysWarning, msg);
   else       MESSAGE(SysWarning, msg, m_filename, m_line, m_column);
   m_warnings++;
}

void VcdParser::infoMessage (const QString& msg, bool noloc)
{
   if (noloc) MESSAGE(SysInfo, msg);
   else       MESSAGE(SysInfo, msg, m_filename, m_line, m_column);
}