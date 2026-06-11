#include "vcd_parser.h"

using namespace vcd;

ParseError::ParseError(SourceLocation where, const QString& msg)
   : std::runtime_error(format(where, msg).toStdString()), loc(where)
{
}

QString ParseError::format(SourceLocation where, const QString& msg)
{
   return QStringLiteral("VCD parse error at line %1, column %2: %3")
      .arg(where.line)
      .arg(where.column)
      .arg(msg);
}

// -----------------------------------------------------------------------------
// VCD Parser
// -----------------------------------------------------------------------------

VcdParser::VcdParser(QTextStream& in) : m_in(in) {}

std::unique_ptr<WaveformDocument> VcdParser::parse()
{
   std::unique_ptr<WaveformDocument> document = std::make_unique<WaveformDocument>();

   skipWhitespace();

   // value_change_dump_definitions
   //    : declaration_command+ simulation_command+
   //    ;
   if (!isDeclarationCommandStart()) {
      fail(QStringLiteral("expected declaration command"));
   }

   do {
      Declaration decl = parseDeclarationCommand();
      if (decl.kind == Declaration::Kind::Scope)
         m_scopeStack.push_back(decl.scope.identifier);
      else if (decl.kind == Declaration::Kind::Upscope)
      {
         if (!m_scopeStack.isEmpty())
            m_scopeStack.removeLast();
      }
      else if (decl.kind == Declaration::Kind::Timescale)
      {
         const int multiplier = decl.timescale.number;

         // Convert unit to seconds
         qreal unitSeconds = 1e-9; // default ns

         if (decl.timescale.unit == TimeUnit::S)
            unitSeconds = 1.0;
         else if (decl.timescale.unit == TimeUnit::Ms)
            unitSeconds = 1e-3;
         else if (decl.timescale.unit == TimeUnit::Us)
            unitSeconds = 1e-6;
         else if (decl.timescale.unit == TimeUnit::Ns)
            unitSeconds = 1e-9;
         else if (decl.timescale.unit == TimeUnit::Ps)
            unitSeconds = 1e-12;
         else if (decl.timescale.unit == TimeUnit::Fs)
            unitSeconds = 1e-15;

         // Convert to nanoseconds
         const qreal unitNs = unitSeconds / 1e-9;

         m_timescaleFactor = multiplier * unitNs;
      }
      else if (decl.kind == Declaration::Kind::Var)
      {
         VarDecl var = decl.var;
         m_varsById.insert(var.idCode, var);
      }

//      file.declarations.push_back(parseDeclarationCommand());
      skipWhitespace();
   } while (!eof() && isDeclarationCommandStart());

   if (!isSimulationCommandStart()) {
      fail(QStringLiteral("expected simulation command"));
   }

   do {
      SimulationCommand cmd = parseSimulationCommand();
      if (cmd.kind == SimulationCommand::Kind::Time)
      {
         m_currentTime = cmd.time;
      }
      else if (cmd.kind == SimulationCommand::Kind::ValueChange)
      {
         QString idCode;
         QString value;

         idCode = cmd.valueChange.idCode;
         value = cmd.valueChange.value;
         m_changesById[idCode].push_back({m_currentTime, value});
      }


//      file.simulationCommands.push_back(parseSimulationCommand());
      skipWhitespace();
   } while (!eof());

   // re-organize value changes
   for (auto it = m_varsById.begin(); it != m_varsById.end(); ++it)
   {
      const VarDecl& var = it.value();

      WaveSignal* sig =
         m_document->createSignal(var.reference.name,
                                  (var.size <= 1) ?  WaveSignalKind::Bit : WaveSignalKind::Bus,
                                  var.size,
                                  m_scopeStack.last());

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

   return std::move(document);
}

SourceLocation VcdParser::loc() const
{
   return SourceLocation{m_offset, m_line, m_column};
}

[[noreturn]] void VcdParser::fail(const QString& msg) const
{
   throw ParseError(loc(), msg);
}

bool VcdParser::isWhitespace(QChar c)
{
   return c == QLatin1Char(' ')  ||
          c == QLatin1Char('\t') ||
          c == QLatin1Char('\r') ||
          c == QLatin1Char('\n');
}

bool VcdParser::isDigit(QChar c)
{
   return c >= QLatin1Char('0') && c <= QLatin1Char('9');
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
         fail(QStringLiteral("unexpected end of file"));
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

void VcdParser::skipWhitespace()
{
   while (!eof() && isWhitespace(peek())) {
      get();
   }
}

QString VcdParser::parseToken()
{
   skipWhitespace();

   if (eof()) {
      fail(QStringLiteral("expected token"));
   }

   QString token;
   while (!eof() && !isWhitespace(peek())) {
      token.append(get());
   }

   return token;
}

QString VcdParser::readRawTokenFromCurrentPosition()
{
   QString token;
   while (!eof() && !isWhitespace(peek())) {
      token.append(get());
   }
   return token;
}

bool VcdParser::nextTokenIs(const QString& keyword)
{
   skipWhitespace();

   const QString token = parseToken();
   const bool    match = (token == keyword);

   // We cannot un-read a whole token with the one-char buffer, so this helper
   // intentionally consumes the token. Use only when consumption is desired.
   return match;
}

void VcdParser::expectKeyword(const QString& keyword)
{
   const QString token = parseToken();
   if (token != keyword) {
      fail(QStringLiteral("expected '%1', got '%2'").arg(keyword, token));
   }
}

void VcdParser::expectChar(QChar expected)
{
   skipWhitespace();

   if (peek() != expected) {
      fail(QStringLiteral("expected '%1'").arg(expected));
   }

   get();
}

int VcdParser::parseDecimalNumber()
{
   const QString token = parseToken();

   bool ok = false;
   const int value = token.toInt(&ok, 10);
   if (!ok) {
      fail(QStringLiteral("expected decimal number, got '%1'").arg(token));
   }

   return value;
}

quint64 VcdParser::parseUnsigned64AfterHash()
{
   skipWhitespace();
   expectChar(QLatin1Char('#'));

   QString digits;
   while (!eof() && isDigit(peek())) {
      digits.append(get());
   }

   if (digits.isEmpty()) {
      fail(QStringLiteral("expected decimal number after '#'"));
   }

   bool ok = false;
   const quint64 value = digits.toULongLong(&ok, 10);
   if (!ok) {
      fail(QStringLiteral("invalid simulation time '%1'").arg(digits));
   }

   return value;
}

// Reads text until a standalone $end token. The $end token is consumed.
QString VcdParser::parseTextUntilEnd()
{
   skipWhitespace();

   QString text;

   while (!eof()) {
      const QChar c = peek();

      if (c == QLatin1Char('$')) {
         const QString token = readRawTokenFromCurrentPosition();
         if (token == QStringLiteral("$end")) {
            trimRight(text);
            return text;
         }

         text.append(token);
         continue;
      }

      text.append(get());
   }

   fail(QStringLiteral("expected '$end'"));
}

void VcdParser::trimRight(QString& s)
{
   while (!s.isEmpty() && isWhitespace(s.back())) {
      s.chop(1);
   }
}

void VcdParser::trimLeft(QString& s)
{
   while (!s.isEmpty() && isWhitespace(s.front())) {
      s.remove(0, 1);
   }
}

bool VcdParser::isDeclarationCommandToken(const QString& token) const
{
   return token == QStringLiteral("$comment")        ||
          token == QStringLiteral("$date")           ||
          token == QStringLiteral("$enddefinitions") ||
          token == QStringLiteral("$scope")          ||
          token == QStringLiteral("$timescale")      ||
          token == QStringLiteral("$upscope")        ||
          token == QStringLiteral("$var")            ||
          token == QStringLiteral("$version");
}

bool VcdParser::isSimulationCommandToken(const QString& token) const
{
   return token == QStringLiteral("$dumpall")  ||
          token == QStringLiteral("$dumpoff")  ||
          token == QStringLiteral("$dumpon")   ||
          token == QStringLiteral("$dumpvars") ||
          token == QStringLiteral("$comment");
}

bool VcdParser::isDeclarationCommandStart()
{
   skipWhitespace();
   if (eof()) {
      return false;
   }

   if (peek() != QLatin1Char('$')) {
      return false;
   }

   // Lookahead by reading the token into a small parser-side buffer would be
   // cleaner, but to keep one-char lookahead, use token dispatch functions
   // that consume. For start checks, use a lightweight prefix matcher.
   return startsWithAnyKeyword({
      QStringLiteral("$comment"),
      QStringLiteral("$date"),
      QStringLiteral("$enddefinitions"),
      QStringLiteral("$scope"),
      QStringLiteral("$timescale"),
      QStringLiteral("$upscope"),
      QStringLiteral("$var"),
      QStringLiteral("$version")
   });
}

bool VcdParser::isSimulationCommandStart()
{
   skipWhitespace();

   if (eof()) {
      return false;
   }

   const QChar c = peek();
   return c == QLatin1Char('#') ||
          isValueChangeStart(c) ||
          startsWithAnyKeyword({
             QStringLiteral("$dumpall"),
             QStringLiteral("$dumpoff"),
             QStringLiteral("$dumpon"),
             QStringLiteral("$dumpvars"),
             QStringLiteral("$comment")
          });
}

bool VcdParser::startsWithAnyKeyword(const QVector<QString>& keywords)
{
   // QTextStream is sequential from the parser's point of view. To avoid
   // consuming real input during lookahead, this routine peeks by temporarily
   // reading into m_tokenLookahead via parseTokenWithReplay().
   const QString token = peekToken();

   for (const QString& keyword : keywords) {
      if (token == keyword) {
         return true;
      }
   }

   return false;
}

QString VcdParser::peekToken()
{
   if (!m_hasPeekedToken) {
      m_peekedToken = parseTokenRawNoPeekToken();
      m_hasPeekedToken = true;
   }

   return m_peekedToken;
}

QString VcdParser::parseTokenFromPeekBufferOrInput()
{
   if (m_hasPeekedToken) {
      const QString token = m_peekedToken;
      m_peekedToken.clear();
      m_hasPeekedToken = false;
      return token;
   }

   return parseTokenRawNoPeekToken();
}

QString VcdParser::parseTokenRawNoPeekToken()
{
   skipWhitespace();

   if (eof()) {
      fail(QStringLiteral("expected token"));
   }

   QString token;
   while (!eof() && !isWhitespace(peek())) {
      token.append(get());
   }

   return token;
}

// Override parseToken users through this macro-like convention by keeping this
// function near dispatch code. Existing parse routines call parseToken(), so
// parseToken() itself delegates to this once token peeking exists.

bool VcdParser::isValueChangeStart(QChar c)
{
   return c == QLatin1Char('0') ||
          c == QLatin1Char('1') ||
          c == QLatin1Char('x') ||
          c == QLatin1Char('X') ||
          c == QLatin1Char('z') ||
          c == QLatin1Char('Z') ||
          c == QLatin1Char('b') ||
          c == QLatin1Char('B') ||
          c == QLatin1Char('r') ||
          c == QLatin1Char('R');
}

Declaration VcdParser::parseDeclarationCommand()
{
   skipWhitespace();

   const QString keyword = parseTokenFromPeekBufferOrInput();

   if (keyword == QStringLiteral("$comment"))        return parseDeclarationCommentAfterKeyword();
   if (keyword == QStringLiteral("$date"))           return parseDeclarationDateAfterKeyword();
   if (keyword == QStringLiteral("$enddefinitions")) return parseDeclarationEndDefinitionsAfterKeyword();
   if (keyword == QStringLiteral("$scope"))          return parseDeclarationScopeAfterKeyword();
   if (keyword == QStringLiteral("$timescale"))      return parseDeclarationTimescaleAfterKeyword();
   if (keyword == QStringLiteral("$upscope"))        return parseDeclarationUpscopeAfterKeyword();
   if (keyword == QStringLiteral("$var"))            return parseDeclarationVarAfterKeyword();
   if (keyword == QStringLiteral("$version"))        return parseDeclarationVersionAfterKeyword();

   fail(QStringLiteral("expected declaration command, got '%1'").arg(keyword));
}

Declaration VcdParser::parseDeclarationCommentAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Comment;
   d.text = parseTextUntilEnd();
   return d;
}

Declaration VcdParser::parseDeclarationDateAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Date;
   d.text = parseTextUntilEnd();
   return d;
}

Declaration VcdParser::parseDeclarationEndDefinitionsAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::EndDefinitions;
   expectKeyword(QStringLiteral("$end"));
   return d;
}

Declaration VcdParser::parseDeclarationScopeAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Scope;
   d.scope.type       = parseScopeType();
   d.scope.identifier = parseTokenFromPeekBufferOrInput();
   expectKeyword(QStringLiteral("$end"));
   return d;
}

ScopeType VcdParser::parseScopeType()
{
   const QString token = parseTokenFromPeekBufferOrInput();

   if (token == QStringLiteral("begin"))    return ScopeType::Begin;
   if (token == QStringLiteral("fork"))     return ScopeType::Fork;
   if (token == QStringLiteral("function")) return ScopeType::Function;
   if (token == QStringLiteral("module"))   return ScopeType::Module;
   if (token == QStringLiteral("task"))     return ScopeType::Task;

   fail(QStringLiteral("invalid scope_type '%1'").arg(token));
}

Declaration VcdParser::parseDeclarationTimescaleAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Timescale;
   d.timescale.number = parseTimeNumber();
   d.timescale.unit   = parseTimeUnit();
   expectKeyword(QStringLiteral("$end"));
   return d;
}

int VcdParser::parseTimeNumber()
{
   const int n = parseDecimalNumber();
   if (n != 1 && n != 10 && n != 100) {
      fail(QStringLiteral("invalid time_number; expected 1, 10, or 100"));
   }
   return n;
}

TimeUnit VcdParser::parseTimeUnit()
{
   const QString token = parseTokenFromPeekBufferOrInput();

   if (token == QStringLiteral("s"))  return TimeUnit::S;
   if (token == QStringLiteral("ms")) return TimeUnit::Ms;
   if (token == QStringLiteral("us")) return TimeUnit::Us;
   if (token == QStringLiteral("ns")) return TimeUnit::Ns;
   if (token == QStringLiteral("ps")) return TimeUnit::Ps;
   if (token == QStringLiteral("fs")) return TimeUnit::Fs;

   fail(QStringLiteral("invalid time_unit '%1'").arg(token));
}

Declaration VcdParser::parseDeclarationUpscopeAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Upscope;
   expectKeyword(QStringLiteral("$end"));
   return d;
}

Declaration VcdParser::parseDeclarationVarAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Var;
   d.var.type        = parseVarType();
   d.var.size        = parseDecimalNumber();
   d.var.idCode      = parseTokenFromPeekBufferOrInput();
   d.var.reference   = parseReferenceUntilEnd();
   expectKeyword(QStringLiteral("$end"));
   return d;
}

VarType VcdParser::parseVarType()
{
   const QString token = parseTokenFromPeekBufferOrInput();

   if (token == QStringLiteral("event"))     return VarType::Event;
   if (token == QStringLiteral("integer"))   return VarType::Integer;
   if (token == QStringLiteral("parameter")) return VarType::Parameter;
   if (token == QStringLiteral("real"))      return VarType::Real;
   if (token == QStringLiteral("realtime"))  return VarType::Realtime;
   if (token == QStringLiteral("reg"))       return VarType::Reg;
   if (token == QStringLiteral("supply0"))   return VarType::Supply0;
   if (token == QStringLiteral("supply1"))   return VarType::Supply1;
   if (token == QStringLiteral("time"))      return VarType::Time;
   if (token == QStringLiteral("tri"))       return VarType::Tri;
   if (token == QStringLiteral("triand"))    return VarType::TriAnd;
   if (token == QStringLiteral("trior"))     return VarType::TriOr;
   if (token == QStringLiteral("trireg"))    return VarType::TriReg;
   if (token == QStringLiteral("tri0"))      return VarType::Tri0;
   if (token == QStringLiteral("tri1"))      return VarType::Tri1;
   if (token == QStringLiteral("wand"))      return VarType::WAnd;
   if (token == QStringLiteral("wire"))      return VarType::Wire;
   if (token == QStringLiteral("wor"))       return VarType::WOr;

   fail(QStringLiteral("invalid var_type '%1'").arg(token));
}

Reference VcdParser::parseReferenceUntilEnd()
{
   // VCD references are often one token, but escaped identifiers and generated
   // names can be awkward. Collect everything up to standalone $end, then parse
   // a trailing [N] or [M:N] if present.
   skipWhitespace();

   QString text;
   while (!eof()) {
      if (peek() == QLatin1Char('$')) {
         const QString token = readRawTokenFromCurrentPosition();
         if (token == QStringLiteral("$end")) {
            // Keep $end consumed; caller's expectKeyword("$end") would be wrong.
            // To maintain the simple call pattern, stash it as a peeked token.
            m_peekedToken = token;
            m_hasPeekedToken = true;
            break;
         }
         text.append(token);
         continue;
      }

      text.append(get());
   }

   trimRight(text);
   trimLeft(text);

   if (text.isEmpty()) {
      fail(QStringLiteral("expected reference"));
   }

   Reference ref;

   const qsizetype open  = text.lastIndexOf(QLatin1Char('['));
   const qsizetype close = text.lastIndexOf(QLatin1Char(']'));

   if (open >= 0 && close == text.size() - 1 && open < close) {
      QString base = text.left(open);
      trimRight(base);

      const QString inside = text.mid(open + 1, close - open - 1);
      const qsizetype colon = inside.indexOf(QLatin1Char(':'));

      bool ok = false;
      int msb = 0;
      int lsb = 0;

      if (colon < 0) {
         msb = inside.toInt(&ok, 10);
         if (!ok) {
            fail(QStringLiteral("invalid bit select index in reference '%1'").arg(text));
         }
         lsb = msb;
      }
      else {
         const QString lhs = inside.left(colon);
         const QString rhs = inside.mid(colon + 1);

         msb = lhs.toInt(&ok, 10);
         if (!ok) {
            fail(QStringLiteral("invalid range in reference '%1'").arg(text));
         }

         lsb = rhs.toInt(&ok, 10);
         if (!ok) {
            fail(QStringLiteral("invalid range in reference '%1'").arg(text));
         }
      }

      ref.name     = base;
      ref.hasRange = true;
      ref.msb      = msb;
      ref.lsb      = lsb;
   }
   else {
      ref.name = text;
   }

   return ref;
}

Declaration VcdParser::parseDeclarationVersionAfterKeyword()
{
   Declaration d;
   d.kind = Declaration::Kind::Version;
   d.text = parseTextUntilEnd();
   return d;
}

SimulationCommand VcdParser::parseSimulationCommand()
{
   skipWhitespace();

   if (peek() == QLatin1Char('#')) {
      return parseSimulationTime();
   }

   if (isValueChangeStart(peek()) && peek() != QLatin1Char('$')) {
      return parseSimulationValueChange();
   }

   const QString keyword = parseTokenFromPeekBufferOrInput();

   if (keyword == QStringLiteral("$dumpall"))  return parseDumpCommand(SimulationCommand::Kind::DumpAll);
   if (keyword == QStringLiteral("$dumpoff"))  return parseDumpCommand(SimulationCommand::Kind::DumpOff);
   if (keyword == QStringLiteral("$dumpon"))   return parseDumpCommand(SimulationCommand::Kind::DumpOn);
   if (keyword == QStringLiteral("$dumpvars")) return parseDumpCommand(SimulationCommand::Kind::DumpVars);
   if (keyword == QStringLiteral("$comment"))  return parseSimulationCommentAfterKeyword();

   fail(QStringLiteral("expected simulation command, got '%1'").arg(keyword));
}

SimulationCommand VcdParser::parseDumpCommand(SimulationCommand::Kind kind)
{
   SimulationCommand c;
   c.kind = kind;

   skipWhitespace();

   if (startsWithAnyKeyword({QStringLiteral("$end")})) {
      fail(QStringLiteral("dump command requires at least one value_change"));
   }

   while (!eof()) {
      skipWhitespace();

      if (startsWithAnyKeyword({QStringLiteral("$end")})) {
         expectKeyword(QStringLiteral("$end"));
         break;
      }

      c.dumpChanges.push_back(parseValueChange());
   }

   if (c.dumpChanges.isEmpty()) {
      fail(QStringLiteral("dump command requires at least one value_change"));
   }

   return c;
}

SimulationCommand VcdParser::parseSimulationCommentAfterKeyword()
{
   SimulationCommand c;
   c.kind = SimulationCommand::Kind::Comment;
   c.comment = parseTextUntilEnd();
   return c;
}

SimulationCommand VcdParser::parseSimulationTime()
{
   SimulationCommand c;
   c.kind = SimulationCommand::Kind::Time;
   c.time = parseUnsigned64AfterHash();
   return c;
}

SimulationCommand VcdParser::parseSimulationValueChange()
{
   SimulationCommand c;
   c.kind = SimulationCommand::Kind::ValueChange;
   c.valueChange = parseValueChange();
   return c;
}

ValueChange VcdParser::parseValueChange()
{
   skipWhitespace();

   const QChar c = peek();
   if (c == QLatin1Char('0') ||
       c == QLatin1Char('1') ||
       c == QLatin1Char('x') ||
       c == QLatin1Char('X') ||
       c == QLatin1Char('z') ||
       c == QLatin1Char('Z')) {
      return parseScalarValueChange();
   }

   if (c == QLatin1Char('b') ||
       c == QLatin1Char('B') ||
       c == QLatin1Char('r') ||
       c == QLatin1Char('R')) {
      return parseVectorOrRealValueChange();
   }

   fail(QStringLiteral("expected value_change"));
}

ValueChange VcdParser::parseScalarValueChange()
{
   ValueChange vc;
   vc.value = get();

   QString id;
   while (!eof() && !isWhitespace(peek())) {
      id.append(get());
   }

   if (id.isEmpty()) {
      fail(QStringLiteral("expected identifier_code after scalar value"));
   }

   vc.idCode = id;
   return vc;
}

ValueChange VcdParser::parseVectorOrRealValueChange()
{
   ValueChange vc;
   vc.radix = get();

   QString value;
   while (!eof() && !isWhitespace(peek())) {
      value.append(get());
   }

   if (value.isEmpty()) {
      fail(QStringLiteral("expected value text after vector/real value prefix"));
   }

   vc.value = value;
   vc.idCode = parseTokenFromPeekBufferOrInput();

   if (vc.idCode.isEmpty()) {
      fail(QStringLiteral("expected identifier_code after vector/real value"));
   }

   return vc;
}

// -----------------------------------------------------------------------------
// Small helpers
// -----------------------------------------------------------------------------

inline QString toString(TimeUnit unit)
{
   switch (unit) {
   case TimeUnit::S:  return QStringLiteral("s");
   case TimeUnit::Ms: return QStringLiteral("ms");
   case TimeUnit::Us: return QStringLiteral("us");
   case TimeUnit::Ns: return QStringLiteral("ns");
   case TimeUnit::Ps: return QStringLiteral("ps");
   case TimeUnit::Fs: return QStringLiteral("fs");
   }
   return QStringLiteral("?");
}

inline QString toString(ScopeType type)
{
   switch (type) {
   case ScopeType::Begin:    return QStringLiteral("begin");
   case ScopeType::Fork:     return QStringLiteral("fork");
   case ScopeType::Function: return QStringLiteral("function");
   case ScopeType::Module:   return QStringLiteral("module");
   case ScopeType::Task:     return QStringLiteral("task");
   }
   return QStringLiteral("?");
}

inline QString toString(VarType type)
{
   switch (type) {
   case VarType::Event:     return QStringLiteral("event");
   case VarType::Integer:   return QStringLiteral("integer");
   case VarType::Parameter: return QStringLiteral("parameter");
   case VarType::Real:      return QStringLiteral("real");
   case VarType::Realtime:  return QStringLiteral("realtime");
   case VarType::Reg:       return QStringLiteral("reg");
   case VarType::Supply0:   return QStringLiteral("supply0");
   case VarType::Supply1:   return QStringLiteral("supply1");
   case VarType::Time:      return QStringLiteral("time");
   case VarType::Tri:       return QStringLiteral("tri");
   case VarType::TriAnd:    return QStringLiteral("triand");
   case VarType::TriOr:     return QStringLiteral("trior");
   case VarType::TriReg:    return QStringLiteral("trireg");
   case VarType::Tri0:      return QStringLiteral("tri0");
   case VarType::Tri1:      return QStringLiteral("tri1");
   case VarType::WAnd:      return QStringLiteral("wand");
   case VarType::Wire:      return QStringLiteral("wire");
   case VarType::WOr:       return QStringLiteral("wor");
   }
   return QStringLiteral("?");
}

