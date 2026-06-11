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

#include <QTextBlockUserData>
#include <QFontMetrics>
#include "message.h"
#include "artilib_global.h"

namespace artiLib
{
   typedef QPair<QString, int> alignment;       // QPair<align_label, column>.
   class alignData : public QTextBlockUserData
   {
      public:
         QList<alignment*> alignments;          // Should be sorted lowest column first

      protected:
         int      _blockNumber;

      public:
                  alignData (int blockNumber) : QTextBlockUserData() {_blockNumber = blockNumber;}
                  ~alignData () {qDeleteAll(alignments);}

         int      blockNumber () {return _blockNumber;}
   };

   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   class ARTILIB_EXPORT Doc     : public QTextDocument
   {
      Q_OBJECT

      public:
                                    Doc ();

         void                       addText (const QString& text);
         void                       addHtml (const QString& html);
         void                       addMarkdown (const QString& md);
         void                       align (const QString& label);
         void                       setIndentDelta (int delta);
         QString                    toString ();
         QString                    lastChar();
         bool                       isText () {return m_text;}
         bool                       isHtml () {return m_html;}
         bool                       isMd () {return m_md;}

      Q_SIGNALS:
         void                       message (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

      private:
         QString                    formatBlock (QTextBlock block, bool nocr = false);
         void                       reposition (const QString& label);

      protected:
         QTextCursor                      m_cursor;
         QMultiMap<QString, alignData*>   m_alignData;    // <align_label, alignData objects containing align_label>
         QTextBlock                       m_iterBlock;
         bool                             m_text = false;
         bool                             m_html = false;
         bool                             m_md   = false;
   };
   typedef std::shared_ptr<Doc> DocPtr;
}
