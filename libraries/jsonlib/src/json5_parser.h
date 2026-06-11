// json5_parser.h
#pragma once

#include "jsonlib_global.h"
#include "message.h"

#include <QChar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QTextStream>

namespace jsonLib
{
   class JSONLIB_EXPORT Json5Parser : public QObject
   {
      Q_OBJECT

      public:
         enum class NonFinitePolicy {
            Reject,
            ConvertToNull,
            ConvertToString
         };

         explicit             Json5Parser(QTextStream& in);

         QVariant             parse();

         void                 setNonFinitePolicy(NonFinitePolicy p) { m_nonFinitePolicy = p; }

      signals:
         void                 message (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

      private:
         enum class Tok {
            End,
            LBrace, RBrace,
            LBracket, RBracket,
            Colon, Comma,
            String,
            Identifier,
            Literal,
            Number,
            NumericLiteral,
            Plus,
            Minus
         };

         struct Token {
            Tok     type = Tok::End;
            QString text;
            int     line = 1;
            int     col = 1;
         };

      private:
         Token                nextToken();
         Token                peekToken();
         void                 consumeToken();

         bool                 parseValue(QJsonValue& out);
         bool                 parseObject(QJsonObject& out);
         bool                 parseArray(QJsonArray& out);
         bool                 parsePair(QJsonObject& obj);
         bool                 parseKey(QString& key);
         bool                 parseNumber(QJsonValue& out);

         void                 skipWhitespaceAndComments();

         bool                 readString(QChar quote, QString& out);
         bool                 readIdentifier(QString& out);
         bool                 readNumberLike(QString& out);

         QChar                get();
         QChar                peek();
         bool                 atEnd();

         bool                 match(QChar c);
         bool                 expect(Tok t);

         void                 setError(const QString& msg, const Token& t);
         void                 setErrorHere(const QString& msg);

         static bool          isIdentifierStart(QChar c);
         static bool          isIdentifierPart(QChar c);
         static int           hexValue(QChar c);

      private:
         QTextStream&         m_in;

         bool                 m_hasPushback = false;
         QChar                m_pushback;

         bool                 m_hasPeekToken = false;
         Token                m_peekToken;

         int                  m_line = 1;
         int                  m_col = 0;

         NonFinitePolicy      m_nonFinitePolicy = NonFinitePolicy::Reject;
   };
}