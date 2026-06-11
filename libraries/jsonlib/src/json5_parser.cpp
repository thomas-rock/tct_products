// json5_parser.cpp
#include "json5_parser.h"

#include <QtMath>

using namespace jsonLib;
using namespace commonLib;

Json5Parser::Json5Parser(QTextStream& in)
   : m_in(in)
{
}

QVariant Json5Parser::parse()
{
   QJsonDocument doc;
   QJsonValue root;

   Token t = peekToken();
   if (t.type != Tok::End) {
      if (parseValue(root)) {
         t = peekToken();
         if (t.type != Tok::End)
            setError("Unexpected trailing input", t);

         else if (root.isObject())
            doc = QJsonDocument(root.toObject());

         else if (root.isArray())
            doc = QJsonDocument(root.toArray());
      }
   }

   else
      setError("QJsonDocument cannot hold a top-level scalar value", t);

   return doc.toVariant();
}

Json5Parser::Token Json5Parser::peekToken()
{
   if (!m_hasPeekToken) {
      m_peekToken = nextToken();
      m_hasPeekToken = true;
   }

   return m_peekToken;
}

void Json5Parser::consumeToken()
{
   if (m_hasPeekToken) {
      m_hasPeekToken = false;
      return;
   }

   nextToken();
}

bool Json5Parser::expect(Tok t)
{
   Token got = peekToken();
   if (got.type != t) {
      setError("Unexpected token", got);
      return false;
   }

   consumeToken();
   return true;
}

bool Json5Parser::parseValue(QJsonValue& out)
{
   Token t = peekToken();

   switch (t.type) {
   case Tok::String:
      out = t.text;
      consumeToken();
      return true;

   case Tok::Literal:
      if (t.text == "true")
         out = true;
      else if (t.text == "false")
         out = false;
      else if (t.text == "null")
         out = QJsonValue::Null;
      else {
         setError("Unknown literal", t);
         return false;
      }

      consumeToken();
      return true;

   case Tok::LBrace: {
      QJsonObject obj;
      if (!parseObject(obj))
         return false;
      out = obj;
      return true;
   }

   case Tok::LBracket: {
      QJsonArray arr;
      if (!parseArray(arr))
         return false;
      out = arr;
      return true;
   }

   case Tok::Plus:
   case Tok::Minus:
   case Tok::Number:
   case Tok::NumericLiteral:
      return parseNumber(out);

   default:
      setError("Expected value", t);
      return false;
   }
}

bool Json5Parser::parseObject(QJsonObject& out)
{
   if (!expect(Tok::LBrace))
      return false;

   if (peekToken().type == Tok::RBrace) {
      consumeToken();
      return true;
   }

   while (true) {
      if (!parsePair(out))
         return false;

      Token t = peekToken();

      if (t.type == Tok::Comma) {
         consumeToken();

         if (peekToken().type == Tok::RBrace) {
            consumeToken();
            return true;
         }

         continue;
      }

      if (t.type == Tok::RBrace) {
         consumeToken();
         return true;
      }

      setError("Expected ',' or '}'", t);
      return false;
   }
}

bool Json5Parser::parsePair(QJsonObject& obj)
{
   QString key;
   if (!parseKey(key))
      return false;

   if (!expect(Tok::Colon))
      return false;

   QJsonValue value;
   if (!parseValue(value))
      return false;

   obj.insert(key, value);
   return true;
}

bool Json5Parser::parseKey(QString& key)
{
   Token t = peekToken();

   switch (t.type) {
   case Tok::String:
   case Tok::Identifier:
   case Tok::Literal:
   case Tok::NumericLiteral:
   case Tok::Number:
      key = t.text;
      consumeToken();
      return true;

   default:
      setError("Expected object key", t);
      return false;
   }
}

bool Json5Parser::parseArray(QJsonArray& out)
{
   if (!expect(Tok::LBracket))
      return false;

   if (peekToken().type == Tok::RBracket) {
      consumeToken();
      return true;
   }

   while (true) {
      QJsonValue value;
      if (!parseValue(value))
         return false;

      out.append(value);

      Token t = peekToken();

      if (t.type == Tok::Comma) {
         consumeToken();

         if (peekToken().type == Tok::RBracket) {
            consumeToken();
            return true;
         }

         continue;
      }

      if (t.type == Tok::RBracket) {
         consumeToken();
         return true;
      }

      setError("Expected ',' or ']'", t);
      return false;
   }
}

bool Json5Parser::parseNumber(QJsonValue& out)
{
   Token sign = peekToken();

   bool negative = false;

   if (sign.type == Tok::Plus || sign.type == Tok::Minus) {
      negative = sign.type == Tok::Minus;
      consumeToken();
   }

   Token t = peekToken();

   if (t.type == Tok::NumericLiteral) {
      consumeToken();

      if (m_nonFinitePolicy == NonFinitePolicy::Reject) {
         setError("QJsonDocument cannot represent NaN or Infinity", t);
         return false;
      }

      if (m_nonFinitePolicy == NonFinitePolicy::ConvertToNull) {
         out = QJsonValue::Null;
         return true;
      }

      out = negative ? "-" + t.text : t.text;
      return true;
   }

   if (t.type != Tok::Number) {
      setError("Expected number", t);
      return false;
   }

   consumeToken();

   QString s = t.text;
   if (negative)
      s.prepend('-');

   bool ok = false;
   double d = 0.0;

   if (s.startsWith("-0x", Qt::CaseInsensitive) ||
       s.startsWith("+0x", Qt::CaseInsensitive) ||
       s.startsWith("0x", Qt::CaseInsensitive)) {
      bool neg = s.startsWith('-');
      QString hex = s;

      if (hex.startsWith('+') || hex.startsWith('-'))
         hex.remove(0, 1);

      hex.remove(0, 2);

      qulonglong v = hex.toULongLong(&ok, 16);
      d = static_cast<double>(v);
      if (neg)
         d = -d;
   } else {
      d = s.toDouble(&ok);
   }

   if (!ok) {
      setError("Invalid number", t);
      return false;
   }

   out = d;
   return true;
}

Json5Parser::Token Json5Parser::nextToken()
{
   skipWhitespaceAndComments();

   Token t;
   t.line = m_line;
   t.col = m_col + 1;

   if (atEnd()) {
      t.type = Tok::End;
      return t;
   }

   QChar c = get();

   switch (c.unicode()) {
   case '{': t.type = Tok::LBrace;   return t;
   case '}': t.type = Tok::RBrace;   return t;
   case '[': t.type = Tok::LBracket; return t;
   case ']': t.type = Tok::RBracket; return t;
   case ':': t.type = Tok::Colon;    return t;
   case ',': t.type = Tok::Comma;    return t;
   case '+': t.type = Tok::Plus;     return t;
   case '-': t.type = Tok::Minus;    return t;

   case '"':
   case '\'': {
      t.type = Tok::String;
      if (!readString(c, t.text))
         t.type = Tok::End;
      return t;
   }

   default:
      break;
   }

   if (c.isDigit() || c == '.') {
      m_hasPushback = true;
      m_pushback = c;

      t.type = Tok::Number;
      readNumberLike(t.text);
      return t;
   }

   if (isIdentifierStart(c)) {
      m_hasPushback = true;
      m_pushback = c;

      readIdentifier(t.text);

      if (t.text == "true" || t.text == "false" || t.text == "null")
         t.type = Tok::Literal;
      else if (t.text == "Infinity" || t.text == "NaN")
         t.type = Tok::NumericLiteral;
      else
         t.type = Tok::Identifier;

      return t;
   }

   setErrorHere(QString("Unexpected character '%1'").arg(c));
   t.type = Tok::End;
   return t;
}

void Json5Parser::skipWhitespaceAndComments()
{
   while (!atEnd()) {
      QChar c = peek();

      if (c.isSpace() || c == QChar(0x00A0) || c == QChar(0xFEFF) || c == QChar(0x2003)) {
         get();
         continue;
      }

      if (c == '/') {
         get();

         if (peek() == '/') {
            while (!atEnd()) {
               QChar n = get();
               if (n == '\n' || n == '\r' || n == QChar(0x2028) || n == QChar(0x2029))
                  break;
            }
            continue;
         }

         if (peek() == '*') {
            get();

            while (!atEnd()) {
               QChar a = get();
               if (a == '*' && peek() == '/') {
                  get();
                  break;
               }
            }

            continue;
         }

         m_hasPushback = true;
         m_pushback = '/';
         return;
      }

      return;
   }
}

bool Json5Parser::readString(QChar quote, QString& out)
{
   while (!atEnd()) {
      QChar c = get();

      if (c == quote)
         return true;

      if (c == '\n' || c == '\r') {
         setErrorHere("Unescaped newline in string");
         return false;
      }

      if (c != '\\') {
         out.append(c);
         continue;
      }

      if (atEnd()) {
         setErrorHere("Unterminated escape sequence");
         return false;
      }

      QChar e = get();

      switch (e.unicode()) {
      case '"':  out.append('"');  break;
      case '\'': out.append('\''); break;
      case '\\': out.append('\\'); break;
      case '/':  out.append('/');  break;
      case 'b':  out.append('\b'); break;
      case 'f':  out.append('\f'); break;
      case 'n':  out.append('\n'); break;
      case 'r':  out.append('\r'); break;
      case 't':  out.append('\t'); break;
      case 'v':  out.append(QChar(0x000B)); break;
      case '0':  out.append(QChar(0)); break;

      case '\n':
      case '\r':
         // JSON5 line continuation.
         break;

      case 'x': {
         int h1 = hexValue(get());
         int h2 = hexValue(get());

         if (h1 < 0 || h2 < 0) {
            setErrorHere("Invalid hex escape");
            return false;
         }

         out.append(QChar((h1 << 4) | h2));
         break;
      }

      case 'u': {
         int value = 0;

         for (int i = 0; i < 4; ++i) {
            int h = hexValue(get());
            if (h < 0) {
               setErrorHere("Invalid unicode escape");
               return false;
            }

            value = (value << 4) | h;
         }

         out.append(QChar(value));
         break;
      }

      default:
         // JSON5 allows many non-escape escaped characters.
         out.append(e);
         break;
      }
   }

   setErrorHere("Unterminated string");
   return false;
}

bool Json5Parser::readIdentifier(QString& out)
{
   if (atEnd())
      return false;

   QChar c = get();

   if (c == '\\') {
      if (get() != 'u') {
         setErrorHere("Invalid unicode identifier escape");
         return false;
      }

      int value = 0;
      for (int i = 0; i < 4; ++i) {
         int h = hexValue(get());
         if (h < 0) {
            setErrorHere("Invalid unicode identifier escape");
            return false;
         }

         value = (value << 4) | h;
      }

      out.append(QChar(value));
   } else {
      out.append(c);
   }

   while (!atEnd()) {
      QChar p = peek();

      if (!isIdentifierPart(p))
         break;

      out.append(get());
   }

   return true;
}

bool Json5Parser::readNumberLike(QString& out)
{
   while (!atEnd()) {
      QChar c = peek();

      if (c.isDigit() ||
          c == '.' ||
          c == 'x' || c == 'X' ||
          c == 'e' || c == 'E' ||
          c == '+' || c == '-' ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F')) {
         out.append(get());
      } else {
         break;
      }
   }

   return !out.isEmpty();
}

QChar Json5Parser::get()
{
   QChar c;

   if (m_hasPushback) {
      m_hasPushback = false;
      c = m_pushback;
   } else {
      m_in >> c;
   }

   if (c == '\n') {
      ++m_line;
      m_col = 0;
   } else {
      ++m_col;
   }

   return c;
}

QChar Json5Parser::peek()
{
   if (!m_hasPushback) {
      if (m_in.atEnd())
         return QChar();

      m_in >> m_pushback;
      m_hasPushback = true;
   }

   return m_pushback;
}

bool Json5Parser::atEnd()
{
   return !m_hasPushback && m_in.atEnd();
}

bool Json5Parser::isIdentifierStart(QChar c)
{
   return c.isLetter() || c == '$' || c == '_' || c == '\\';
}

bool Json5Parser::isIdentifierPart(QChar c)
{
   return isIdentifierStart(c) ||
          c.isMark() ||
          c.isNumber() ||
          c.category() == QChar::Punctuation_Connector ||
          c == QChar(0x200C) ||
          c == QChar(0x200D);
}

int Json5Parser::hexValue(QChar c)
{
   ushort u = c.unicode();

   if (u >= '0' && u <= '9')
      return u - '0';

   if (u >= 'a' && u <= 'f')
      return 10 + u - 'a';

   if (u >= 'A' && u <= 'F')
      return 10 + u - 'A';

   return -1;
}

void Json5Parser::setError(const QString& msg, const Token& t)
{
   MESSAGE(SysError, QString("%1 at line %2, column %3")
                    .arg(msg)
                    .arg(t.line)
                    .arg(t.col));
}

void Json5Parser::setErrorHere(const QString& msg)
{
   MESSAGE(SysError, QString("%1 at line %2, column %3")
                    .arg(msg)
                    .arg(m_line)
                    .arg(m_col));
}