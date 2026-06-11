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
#pragma once

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "artilib_global.h"

namespace artiLib
{
   enum ArtiToken : quint32
   {
      Arti = 256  ,      // for the root node

      // tag keywords
      Align       ,
      Else        ,
      Elsif       ,
      False       ,
      For         ,
      If          ,
      Include     ,
      Load        ,
      Info        ,
      Debug       ,
      Status      ,
      Warning     ,
      Error       ,
      Fatal       ,
      Null        ,
      Option      ,
      Print       ,
      Set         ,
      Template    ,
      True        ,
      While       ,

      // general tokens
      Array       ,
      Assign      ,
      Eval        ,
      Func        ,
      Id          ,
      Sysid       ,
      Map         ,
      Path        ,
      DString     ,
      SString     ,
      Regex       ,
      Integer     ,
      Real        ,
      Stmt        ,
      Text        ,

      // 2-character operators
      Eq          ,
      Neq         ,
      Lte         ,
      Gte         ,
      EqTilde     ,

      // make distinct unary operators
      Minus       ,

      //--------------------------------------------
      // lexer-specific tokens
      TagStart    ,
      TagEnd      ,
      EvalStart   ,
      EvalEnd     ,
      CtagStart   ,
      CtagEnd     ,

      // helper rules
      DotDot      ,

      // numbers
      Bin         ,
      Oct         ,
      Dec         ,
      Hex         ,

      End
   };

   QString ARTILIB_EXPORT TokenName (const quint32& token);

   struct ARTILIB_EXPORT Location
   {
      quint32 line       = 0;
      quint32 position   = 0;
   };

   struct ARTILIB_EXPORT ArtiNode
   {
      quint32                          token;
      QString                          name;
      QString                          text;
      Location                         loc;
      QList<std::shared_ptr<ArtiNode>> children;

      ArtiNode (const quint32& tok, const quint32 line = 0, const quint32 pos = 0)
         : token{tok}, loc{line, pos} {}
      ArtiNode (const quint32& tok, const QString& text, const quint32 line = 0, const quint32 pos = 0)
         : token{tok}, text{text}, loc{line, pos} {}

      void dump (int indent = 3);
   };
   typedef std::shared_ptr<ArtiNode> ArtiNodePtr;
   typedef QList<ArtiNodePtr>        ArtiNodeList;

   class ARTILIB_EXPORT ArtiData
   {
      public:
         quint32        warnings  = 0;
         quint32        errors    = 0;
         ArtiNodePtr    root      = std::make_shared<ArtiNode>(ArtiToken::Arti);
         QString        filename;
   };
   typedef std::shared_ptr<ArtiData> ArtiDataPtr;

}
