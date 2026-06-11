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
#include "doc.h"

using namespace artiLib;

Doc::Doc () : QTextDocument()
{
   m_cursor = QTextCursor(this);
   QTextCharFormat format = m_cursor.charFormat();
   QFont font = format.font();
   font.setFamily("courier");
   font.setPointSize(12);
   format.setFont(font);
   m_cursor.setCharFormat(format);
}
//-----------------------------------------------------------------------------
void Doc::addText (const QString& text) {m_cursor.insertText(text); m_text = true;}
void Doc::addHtml (const QString& html) {m_cursor.insertHtml(html); m_html = true;}
void Doc::addMarkdown (const QString& md) {m_cursor.insertMarkdown(md); m_md = true;}
void Doc::align (const QString& label)
{
   // check if block contains an existing alignData object
   QTextBlock block = m_cursor.block();
   alignData* blockData = static_cast<alignData*>(block.userData());
   if (blockData == nullptr) {
      blockData = new alignData(block.blockNumber());
      block.setUserData(blockData);
   }

   // check for duplicate align label in the block
   foreach (alignment* align, blockData->alignments) {
      if (align->first == label) {
         MESSAGE(SysWarning, QString("Duplicate alignment label '%1' - ignored").arg(label));
         return;
      }
   }

   // add alignment
   blockData->alignments.append(new alignment(label, m_cursor.columnNumber()));
   m_alignData.insert(label, blockData);

   // reposition each block with alignment 'label' plus any other subsequent labels
   reposition(label);
}
void Doc::setIndentDelta (int delta)
{
   QTextBlockFormat format = m_cursor.blockFormat();
   format.setIndent(format.indent()+delta);
   m_cursor.setBlockFormat(format);
}
QString Doc::toString ()
{
   QString text;
   QTextBlock block = firstBlock();
   while (block.isValid())
   {
      text.append(formatBlock(block));
      block = block.next();
   }

   return text;
}
QString Doc::lastChar ()
{
   QString text = m_cursor.block().text();
   if (!text.isEmpty()) return text.last(1);
   return QString();
}


//-----------------------------------------------------------------------------
QString Doc::formatBlock (QTextBlock block, bool nocr)
{
   QString text(block.blockFormat().indent(), ' ');
   text.append(block.text());
   if (!nocr) text.append("\n");
   return text;
}
void Doc::reposition (const QString& label)
{
   // find largest position for each 'label'
   int maxPos = 0;
   foreach (alignData* blockData, m_alignData.values(label))
   {
      QTextBlock block = findBlockByNumber(blockData->blockNumber());
      foreach (alignment* align, blockData->alignments)
      {
         if (align->first != label) continue;

         int pos = (block.blockFormat().indent()) + align->second;
         if (pos > maxPos)
            maxPos = pos;
         break;
      }
   }

   // reposition each block with alignment 'label' and save off subsequent labels for recursive repositioning
   QStringList subLabels;
   foreach (alignData* blockData, m_alignData.values(label))
   {
      QTextBlock block = findBlockByNumber(blockData->blockNumber());
      int indentWidth = (block.blockFormat().indent());
      int indentSize = 0;
      foreach (alignment* align, blockData->alignments)
      {
         if (align->first == label)
         {
            int pos = indentWidth + align->second;
            indentSize = maxPos - pos;      // compute additional required indent
            if (indentSize == 0) break;

            QTextCursor cursor(block);
            cursor.setPosition(block.position() + align->second);
            cursor.insertText(QString(indentSize, ' '));
            align->second += indentSize;
         }
         else if (indentSize != 0)
         {
            align->second += indentSize;
            if (!subLabels.contains(align->first))
               subLabels.append(align->first);
         }
      }
   }

   foreach (QString subLabel, subLabels)
      reposition(subLabel);
}
