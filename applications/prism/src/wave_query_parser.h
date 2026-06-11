#pragma once

#include "wave_query_ast.h"

#include <QString>
#include <QVector>
#include <memory>

enum class WaveQueryTokenKind
{
   End,
   Identifier,
   Number,
   Literal,

   LParen,
   RParen,

   Not,
   And,
   Or,

   Equal,
   NotEqual,
   Less,
   LessEqual,
   Greater,
   GreaterEqual
};

struct WaveQueryToken
{
   WaveQueryTokenKind kind = WaveQueryTokenKind::End;
   QString text;
   qint64 number = 0;
   int pos = 0;
};

class WaveQueryParser
{
   public:
      struct Result
      {
         bool ok = false;
         QString error;
         std::unique_ptr<WaveQueryNode> root;
      };

      Result                           parse(const QString& text);

   private:
      bool                             tokenize(const QString& text, QString& error);

      const WaveQueryToken&            peek() const;
      const WaveQueryToken&            advance();
      bool                             match(WaveQueryTokenKind kind);
      bool                             expect(WaveQueryTokenKind kind, const QString& message);

      std::unique_ptr<WaveQueryNode>   parseExpression();
      std::unique_ptr<WaveQueryNode>   parseOr();
      std::unique_ptr<WaveQueryNode>   parseAnd();
      std::unique_ptr<WaveQueryNode>   parseComparison();
      std::unique_ptr<WaveQueryNode>   parseUnary();
      std::unique_ptr<WaveQueryNode>   parsePrimary();

      WaveQueryNodeKind                comparisonNodeKind(WaveQueryTokenKind kind) const;

   private:
      QVector<WaveQueryToken>          m_tokens;
      int                              m_index = 0;
      QString                          m_error;
};