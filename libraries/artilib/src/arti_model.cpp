//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================
#include "arti_model.h"
#include "qdebug.h"

void artiLib::ArtiNode::dump(int indent)
{
   qDebug() << QString("%1(%2) %3").arg(QString(indent, ' ')).arg(artiLib::TokenName(token)).arg(text);
   foreach (auto child, children)
      child->dump(indent+3);
}

QString artiLib::TokenName (const quint32& token)
{
   if (token < 256) return QChar(token);

   switch (token)
   {
      case ArtiToken::Arti         : return QStringLiteral("<ARTI>>");
      case ArtiToken::Align        : return QStringLiteral("align");
      case ArtiToken::Else         : return QStringLiteral("else");
      case ArtiToken::Elsif        : return QStringLiteral("elsif");
      case ArtiToken::False        : return QStringLiteral("false");
      case ArtiToken::For          : return QStringLiteral("for");
      case ArtiToken::If           : return QStringLiteral("if");
      case ArtiToken::Include      : return QStringLiteral("include");
      case ArtiToken::Load         : return QStringLiteral("load");
      case ArtiToken::Info         : return QStringLiteral("info");
      case ArtiToken::Debug        : return QStringLiteral("debug");
      case ArtiToken::Status       : return QStringLiteral("status");
      case ArtiToken::Warning      : return QStringLiteral("warning");
      case ArtiToken::Error        : return QStringLiteral("error");
      case ArtiToken::Fatal        : return QStringLiteral("fatal");
      case ArtiToken::Null         : return QStringLiteral("null");
      case ArtiToken::Option       : return QStringLiteral("option");
      case ArtiToken::Print        : return QStringLiteral("print");
      case ArtiToken::Set          : return QStringLiteral("set");
      case ArtiToken::Template     : return QStringLiteral("template");
      case ArtiToken::True         : return QStringLiteral("true");
      case ArtiToken::While        : return QStringLiteral("while");

            // general tokens
      case ArtiToken::Array        : return QStringLiteral("<array>");
      case ArtiToken::Assign       : return QStringLiteral("<assign>");
      case ArtiToken::Eval         : return QStringLiteral("<eval>");
      case ArtiToken::Func         : return QStringLiteral("<func>");
      case ArtiToken::Id           : return QStringLiteral("<id>");
      case ArtiToken::Sysid        : return QStringLiteral("<sysid>");
      case ArtiToken::Map          : return QStringLiteral("<map>");
      case ArtiToken::Path         : return QStringLiteral("<path>");
      case ArtiToken::DString      : return QStringLiteral("<dstring>");
      case ArtiToken::SString      : return QStringLiteral("<sstring>");
      case ArtiToken::Regex        : return QStringLiteral("<regex>");
      case ArtiToken::Integer      : return QStringLiteral("<integer>");
      case ArtiToken::Real         : return QStringLiteral("<real number>");
      case ArtiToken::Stmt         : return QStringLiteral("stmt");
      case ArtiToken::Text         : return QStringLiteral("<text>");

            // 2-character operators
      case ArtiToken::Eq           : return QStringLiteral("==");
      case ArtiToken::Neq          : return QStringLiteral("!=");
      case ArtiToken::Lte          : return QStringLiteral("<=");
      case ArtiToken::Gte          : return QStringLiteral(">=");
      case ArtiToken::EqTilde      : return QStringLiteral("=~");
      case ArtiToken::Minus        : return QStringLiteral("<unary minus>");

            //--------------------------------------------
            // lexer-specific tokens
      case ArtiToken::TagStart     : return QStringLiteral("{%");
      case ArtiToken::TagEnd       : return QStringLiteral("%}");
      case ArtiToken::EvalStart    : return QStringLiteral("{{");
      case ArtiToken::EvalEnd      : return QStringLiteral("}}");
      case ArtiToken::CtagStart    : return QStringLiteral("<%");
      case ArtiToken::CtagEnd      : return QStringLiteral("%>");

            // helper rules
      case ArtiToken::DotDot       : return QStringLiteral("..");

            // numbers
      case ArtiToken::Bin          : return QStringLiteral("<binary number>");
      case ArtiToken::Oct          : return QStringLiteral("<octal number>");
      case ArtiToken::Dec          : return QStringLiteral("<decimal number>");
      case ArtiToken::Hex          : return QStringLiteral("<hex number>");

      case ArtiToken::End          : return QStringLiteral("<EOF>");
   }

   return QStringLiteral("unknown");
}
