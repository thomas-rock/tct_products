#include "wave_query_parser.h"

#include <QChar>

WaveQueryParser::Result WaveQueryParser::parse(const QString& text)
{
   Result result;

   m_tokens.clear();
   m_index = 0;
   m_error.clear();

   QString error;
   if (!tokenize(text, error))
   {
      result.error = error;
      return result;
   }

   auto root = parseExpression();

   if (!root)
   {
      result.error = m_error.isEmpty() ? "Invalid expression" : m_error;
      return result;
   }

   if (peek().kind != WaveQueryTokenKind::End)
   {
      result.error = QString("Unexpected token '%1' at position %2")
                        .arg(peek().text)
                        .arg(peek().pos);
      return result;
   }

   result.ok = true;
   result.root = std::move(root);
   return result;
}

bool WaveQueryParser::tokenize(const QString& text, QString& error)
{
   int i = 0;

   auto add = [&](WaveQueryTokenKind kind, const QString& s, int pos)
   {
      WaveQueryToken tok;
      tok.kind = kind;
      tok.text = s;
      tok.pos = pos;
      m_tokens.push_back(tok);
   };

   while (i < text.size())
   {
      const QChar c = text[i];

      if (c.isSpace())
      {
         ++i;
         continue;
      }

      const int pos = i;

      if (c == '(')
      {
         add(WaveQueryTokenKind::LParen, "(", pos);
         ++i;
         continue;
      }

      if (c == ')')
      {
         add(WaveQueryTokenKind::RParen, ")", pos);
         ++i;
         continue;
      }

      if (c == '!')
      {
         if (i + 1 < text.size() && text[i + 1] == '=')
         {
            add(WaveQueryTokenKind::NotEqual, "!=", pos);
            i += 2;
         }
         else
         {
            add(WaveQueryTokenKind::Not, "!", pos);
            ++i;
         }
         continue;
      }

      if (c == '=')
      {
         if (i + 1 < text.size() && text[i + 1] == '=')
         {
            add(WaveQueryTokenKind::Equal, "==", pos);
            i += 2;
         }
         else
         {
            add(WaveQueryTokenKind::Equal, "=", pos);
            ++i;
         }
         continue;
      }

      if (c == '<')
      {
         if (i + 1 < text.size() && text[i + 1] == '=')
         {
            add(WaveQueryTokenKind::LessEqual, "<=", pos);
            i += 2;
         }
         else
         {
            add(WaveQueryTokenKind::Less, "<", pos);
            ++i;
         }
         continue;
      }

      if (c == '>')
      {
         if (i + 1 < text.size() && text[i + 1] == '=')
         {
            add(WaveQueryTokenKind::GreaterEqual, ">=", pos);
            i += 2;
         }
         else
         {
            add(WaveQueryTokenKind::Greater, ">", pos);
            ++i;
         }
         continue;
      }

      if (c == '&')
      {
         if (i + 1 < text.size() && text[i + 1] == '&')
            i += 2;
         else
            ++i;

         add(WaveQueryTokenKind::And, "&", pos);
         continue;
      }

      if (c == '|')
      {
         if (i + 1 < text.size() && text[i + 1] == '|')
            i += 2;
         else
            ++i;

         add(WaveQueryTokenKind::Or, "|", pos);
         continue;
      }

      if (c == '\'' || c == '"')
      {
         const QChar quote = c;
         ++i;

         QString value;
         while (i < text.size() && text[i] != quote)
         {
            value += text[i];
            ++i;
         }

         if (i >= text.size())
         {
            error = QString("Unterminated string literal at position %1").arg(pos);
            return false;
         }

         ++i;

         WaveQueryToken tok;
         tok.kind = WaveQueryTokenKind::Literal;
         tok.text = value;
         tok.pos = pos;
         m_tokens.push_back(tok);
         continue;
      }

      if (c.isDigit())
      {
         QString raw;

         if (c == '0' &&
             i + 1 < text.size() &&
             (text[i + 1] == 'x' || text[i + 1] == 'X'))
         {
            raw += text[i++];
            raw += text[i++];

            while (i < text.size() &&
                   (text[i].isDigit() ||
                    (text[i].toLower() >= 'a' && text[i].toLower() <= 'f') ||
                    text[i] == '_'))
            {
               if (text[i] != '_')
                  raw += text[i];
               ++i;
            }

            bool ok = false;
            const qint64 value = raw.mid(2).toLongLong(&ok, 16);
            if (!ok)
            {
               error = QString("Invalid hex number '%1' at position %2").arg(raw).arg(pos);
               return false;
            }

            WaveQueryToken tok;
            tok.kind = WaveQueryTokenKind::Number;
            tok.text = raw;
            tok.number = value;
            tok.pos = pos;
            m_tokens.push_back(tok);
            continue;
         }

         if (c == '0' &&
             i + 1 < text.size() &&
             (text[i + 1] == 'b' || text[i + 1] == 'B'))
         {
            raw += text[i++];
            raw += text[i++];

            while (i < text.size() &&
                   (text[i] == '0' || text[i] == '1' || text[i] == '_'))
            {
               if (text[i] != '_')
                  raw += text[i];
               ++i;
            }

            bool ok = false;
            const qint64 value = raw.mid(2).toLongLong(&ok, 2);
            if (!ok)
            {
               error = QString("Invalid binary number '%1' at position %2").arg(raw).arg(pos);
               return false;
            }

            WaveQueryToken tok;
            tok.kind = WaveQueryTokenKind::Number;
            tok.text = raw;
            tok.number = value;
            tok.pos = pos;
            m_tokens.push_back(tok);
            continue;
         }

         while (i < text.size() && (text[i].isDigit() || text[i] == '_'))
         {
            if (text[i] != '_')
               raw += text[i];
            ++i;
         }

         bool ok = false;
         const qint64 value = raw.toLongLong(&ok, 10);
         if (!ok)
         {
            error = QString("Invalid number '%1' at position %2").arg(raw).arg(pos);
            return false;
         }

         WaveQueryToken tok;
         tok.kind = WaveQueryTokenKind::Number;
         tok.text = raw;
         tok.number = value;
         tok.pos = pos;
         m_tokens.push_back(tok);
         continue;
      }

      if (c.isLetter() || c == '_' || c == '$')
      {
         QString ident;

         while (i < text.size())
         {
            const QChar ch = text[i];

            if (ch.isLetterOrNumber() ||
                ch == '_' ||
                ch == '$' ||
                ch == '.' ||
                ch == '[' ||
                ch == ']' ||
                ch == ':' ||
                ch == '/')
            {
               ident += ch;
               ++i;
            }
            else
            {
               break;
            }
         }

         const QString upper = ident.toUpper();

         if (upper == "X" || upper == "Z")
         {
            WaveQueryToken tok;
            tok.kind = WaveQueryTokenKind::Literal;
            tok.text = upper;
            tok.pos = pos;
            m_tokens.push_back(tok);
         }
         else
         {
            WaveQueryToken tok;
            tok.kind = WaveQueryTokenKind::Identifier;
            tok.text = ident;
            tok.pos = pos;
            m_tokens.push_back(tok);
         }

         continue;
      }

      error = QString("Unexpected character '%1' at position %2").arg(c).arg(pos);
      return false;
   }

   add(WaveQueryTokenKind::End, "", text.size());
   return true;
}

const WaveQueryToken& WaveQueryParser::peek() const
{
   return m_tokens[m_index];
}

const WaveQueryToken& WaveQueryParser::advance()
{
   const WaveQueryToken& tok = m_tokens[m_index];

   if (m_index < m_tokens.size() - 1)
      ++m_index;

   return tok;
}

bool WaveQueryParser::match(WaveQueryTokenKind kind)
{
   if (peek().kind != kind)
      return false;

   advance();
   return true;
}

bool WaveQueryParser::expect(WaveQueryTokenKind kind, const QString& message)
{
   if (match(kind))
      return true;

   m_error = QString("%1 at position %2").arg(message).arg(peek().pos);
   return false;
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parseExpression()
{
   return parseOr();
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parseOr()
{
   auto node = parseAnd();

   while (peek().kind == WaveQueryTokenKind::Or)
   {
      advance();
      auto rhs = parseAnd();

      if (!node || !rhs)
         return nullptr;

      node = WaveQueryNode::makeBinary(WaveQueryNodeKind::LogicalOr,
                                       std::move(node),
                                       std::move(rhs));
   }

   return node;
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parseAnd()
{
   auto node = parseComparison();

   while (peek().kind == WaveQueryTokenKind::And)
   {
      advance();
      auto rhs = parseComparison();

      if (!node || !rhs)
         return nullptr;

      node = WaveQueryNode::makeBinary(WaveQueryNodeKind::LogicalAnd,
                                       std::move(node),
                                       std::move(rhs));
   }

   return node;
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parseComparison()
{
   auto node = parseUnary();

   while (peek().kind == WaveQueryTokenKind::Equal ||
          peek().kind == WaveQueryTokenKind::NotEqual ||
          peek().kind == WaveQueryTokenKind::Less ||
          peek().kind == WaveQueryTokenKind::LessEqual ||
          peek().kind == WaveQueryTokenKind::Greater ||
          peek().kind == WaveQueryTokenKind::GreaterEqual)
   {
      const WaveQueryTokenKind op = advance().kind;

      auto rhs = parseUnary();

      if (!node || !rhs)
         return nullptr;

      node = WaveQueryNode::makeBinary(comparisonNodeKind(op),
                                       std::move(node),
                                       std::move(rhs));
   }

   return node;
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parseUnary()
{
   if (match(WaveQueryTokenKind::Not))
   {
      auto child = parseUnary();
      if (!child)
         return nullptr;

      return WaveQueryNode::makeUnary(WaveQueryNodeKind::UnaryNot,
                                      std::move(child));
   }

   return parsePrimary();
}

std::unique_ptr<WaveQueryNode> WaveQueryParser::parsePrimary()
{
   const WaveQueryToken& tok = peek();

   if (match(WaveQueryTokenKind::Identifier))
      return WaveQueryNode::makeIdentifier(tok.text);

   if (match(WaveQueryTokenKind::Number))
      return WaveQueryNode::makeNumber(tok.number, tok.text);

   if (match(WaveQueryTokenKind::Literal))
      return WaveQueryNode::makeLiteral(tok.text);

   if (match(WaveQueryTokenKind::LParen))
   {
      auto node = parseExpression();

      if (!expect(WaveQueryTokenKind::RParen, "Expected ')'"))
         return nullptr;

      return node;
   }

   m_error = QString("Expected expression at position %1").arg(tok.pos);
   return nullptr;
}

WaveQueryNodeKind WaveQueryParser::comparisonNodeKind(WaveQueryTokenKind kind) const
{
   switch (kind)
   {
      case WaveQueryTokenKind::Equal:
         return WaveQueryNodeKind::Equal;

      case WaveQueryTokenKind::NotEqual:
         return WaveQueryNodeKind::NotEqual;

      case WaveQueryTokenKind::Less:
         return WaveQueryNodeKind::Less;

      case WaveQueryTokenKind::LessEqual:
         return WaveQueryNodeKind::LessEqual;

      case WaveQueryTokenKind::Greater:
         return WaveQueryNodeKind::Greater;

      case WaveQueryTokenKind::GreaterEqual:
         return WaveQueryNodeKind::GreaterEqual;

      default:
         return WaveQueryNodeKind::Equal;
   }
}