#pragma once

#include <QString>
#include <memory>

enum class WaveQueryNodeKind
{
   Identifier,
   Number,
   Literal,

   UnaryNot,

   LogicalAnd,
   LogicalOr,

   Equal,
   NotEqual,
   Less,
   LessEqual,
   Greater,
   GreaterEqual
};

class WaveQueryNode
{
   public:
      WaveQueryNodeKind                kind;

      QString                          text;
      qint64                           number = 0;

      std::unique_ptr<WaveQueryNode>   left;
      std::unique_ptr<WaveQueryNode>   right;

      explicit WaveQueryNode(WaveQueryNodeKind k)
         : kind(k)
      {
      }

      static std::unique_ptr<WaveQueryNode> makeIdentifier(const QString& name)
      {
         auto node = std::make_unique<WaveQueryNode>(WaveQueryNodeKind::Identifier);
         node->text = name;
         return node;
      }

      static std::unique_ptr<WaveQueryNode> makeNumber(qint64 value, const QString& raw)
      {
         auto node = std::make_unique<WaveQueryNode>(WaveQueryNodeKind::Number);
         node->number = value;
         node->text = raw;
         return node;
      }

      static std::unique_ptr<WaveQueryNode> makeLiteral(const QString& value)
      {
         auto node = std::make_unique<WaveQueryNode>(WaveQueryNodeKind::Literal);
         node->text = value;
         return node;
      }

      static std::unique_ptr<WaveQueryNode> makeUnary(WaveQueryNodeKind kind,
                                                      std::unique_ptr<WaveQueryNode> child)
      {
         auto node = std::make_unique<WaveQueryNode>(kind);
         node->left = std::move(child);
         return node;
      }

      static std::unique_ptr<WaveQueryNode> makeBinary(WaveQueryNodeKind kind,
                                                       std::unique_ptr<WaveQueryNode> lhs,
                                                       std::unique_ptr<WaveQueryNode> rhs)
      {
         auto node = std::make_unique<WaveQueryNode>(kind);
         node->left = std::move(lhs);
         node->right = std::move(rhs);
         return node;
      }


      // debug functions
      static QString indentString(int indent)
      {
         return QString(indent * 3, ' ');
      }

      static QString nodeKindName(WaveQueryNodeKind kind)
      {
         switch (kind)
         {
         case WaveQueryNodeKind::Identifier:
            return "Identifier";

         case WaveQueryNodeKind::Number:
            return "Number";

         case WaveQueryNodeKind::Literal:
            return "Literal";

         case WaveQueryNodeKind::UnaryNot:
            return "UnaryNot";

         case WaveQueryNodeKind::LogicalAnd:
            return "LogicalAnd";

         case WaveQueryNodeKind::LogicalOr:
            return "LogicalOr";

         case WaveQueryNodeKind::Equal:
            return "Equal";

         case WaveQueryNodeKind::NotEqual:
            return "NotEqual";

         case WaveQueryNodeKind::Less:
            return "Less";

         case WaveQueryNodeKind::LessEqual:
            return "LessEqual";

         case WaveQueryNodeKind::Greater:
            return "Greater";

         case WaveQueryNodeKind::GreaterEqual:
            return "GreaterEqual";
         }

         return "Unknown";
      }

      static QString toString(const WaveQueryNode* node,
                              int indent)
      {
         if (!node)
            return indentString(indent) + "<null>\n";

         QString out;

         out += indentString(indent);
         out += nodeKindName(node->kind);

         switch (node->kind)
         {
         case WaveQueryNodeKind::Identifier:
            out += QString(" name=\"%1\"").arg(node->text);
            break;

         case WaveQueryNodeKind::Number:
            out += QString(" value=%1 raw=\"%2\"")
                      .arg(node->number)
                      .arg(node->text);
            break;

         case WaveQueryNodeKind::Literal:
            out += QString(" literal=\"%1\"").arg(node->text);
            break;

         default:
            break;
         }

         out += "\n";

         if (node->left)
            out += toString(node->left.get(), indent + 1);

         if (node->right)
            out += toString(node->right.get(), indent + 1);

         return out;
      }
};