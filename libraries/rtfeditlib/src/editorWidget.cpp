//=============================================================================
// X-Tek Corporation                                   http://www.x-tekcorp.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from X-Tek Corporation.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, X-Tek Corporation
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: editorWidget.cpp 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#include "editorWidget.h"
//#include "XSTBlockData.h"

using namespace rtfeditLib;

editorWidget::editorWidget (QWidget* parent) : QTextEdit(parent) {setup();}
editorWidget::~editorWidget () {}
//-----------------------------------------------------------------------------
bool editorWidget::isPristine () const {return _pristine  && document()->isEmpty() && !document()->isModified();}
bool editorWidget::isModified () const {return document()->isModified();}
bool editorWidget::isRedoAvailable () const {return document()->isRedoAvailable();}
bool editorWidget::isUndoAvailable () const {return document()->isUndoAvailable();}
QString editorWidget::filename() const {return _filename;}
int editorWidget::lines () const {return document()->blockCount();}
void editorWidget::getCursorPosition (int* line, int* index)
{
   QTextCursor cursor = textCursor();
   *line = cursor.blockNumber();
   *index = cursor.positionInBlock();
}
void editorWidget::setCursorPosition (int line, int index)
{
   QTextCursor cursor = textCursor();
   QTextBlock b = cursor.document()->findBlockByNumber(line);
   cursor.setPosition(b.position() + index);
   setTextCursor(cursor);
}
//    does not affect pristine
void editorWidget::setFilename (const QString& filename)
{
   _filename = filename;
   updatePreferences();
}
//-----------------------------------------------------------------------------
void editorWidget::setText (const QString& text, bool readonly)
{
   setPlainText(text);
   document()->setModified(false);
   setReadOnly(readonly);
   _pristine = false;
}
void editorWidget::setData (const QString& data, bool readonly)
{
   // images will be in the xml file before the html, since the html will reference the images
   QXmlStreamReader reader(data);
   while (!reader.atEnd()) {
      reader.readNext();
      if (reader.isStartElement()) {
         QString name = reader.name().toString();
         if (name.startsWith("image_")) {
            // convert string image data back to binary
            QByteArray imageData = reader.readElementText().toLatin1();
            QImage image = QImage::fromData(QByteArray::fromHex(imageData), "PNG");
            if (!image.isNull()) {
               QUrl url = QUrl::fromEncoded(name.remove("image_").toUtf8());
               document()->addResource(QTextDocument::ImageResource, url, QVariant(image));
            }
         }
         else if (name == "text")
            setHtml(reader.readElementText());
         else if (name == "blockData") {
            int blockNumber = reader.attributes().value("blockNumber").toInt();
//            XSTBlockData* blockData = new XSTBlockData(reader);
//            QTextBlock block = document()->findBlockByNumber(blockNumber);
//            if (block.isValid())
//               block.setUserData(blockData);
         }
      }
   }
}
//-----------------------------------------------------------------------------
void editorWidget::fileSaved (const QString& filename)
{
   // the images need to be written before the html since the html will reference the images
   QString rtn;
   QXmlStreamWriter writer(&rtn);
   writer.setAutoFormatting(true);
   writer.writeStartDocument();
   writer.writeStartElement("contents");

   // write image and block information
   writer.writeStartElement("images");
   QTextBlock textBlock = document()->begin();
   while (textBlock.isValid()) {
      // find images
      QTextBlock::iterator it;
      for (it = textBlock.begin(); !(it.atEnd()); ++it) {
         QTextFragment fragment = it.fragment();
         if (fragment.isValid()) {
            QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();
            if (imageFormat.isValid()) {
               QString name = imageFormat.name();
               QUrl url = QUrl::fromEncoded(name.toUtf8());
               const QVariant data = document()->resource(QTextDocument::ImageResource, url);
               if (data.type() == QVariant::Image) {
                  QImage image = qvariant_cast<QImage>(data);
                  if (!image.isNull()) {
                     QBuffer imageBuffer;
                     imageBuffer.open(QIODevice::WriteOnly);
                     image.save(&imageBuffer, "PNG");
                     writer.writeTextElement("image_" + name, imageBuffer.buffer().toHex());       // can't have a numeric xml tag
                  }
               }
            }
         }
      }
      textBlock = textBlock.next();
   }
   writer.writeEndElement();     // images

   // write out text information
   QByteArray ba;
   QBuffer buffer(&ba);
   QTextDocumentWriter textWriter(&buffer, "html");
   textWriter.write(document());
   writer.writeTextElement("text", ba);

   // the block data needs to be written after the doc text because it gets inserted into the text blocks when being read
   textBlock = document()->begin();
   while (textBlock.isValid()) {
      // output block info
//      XSTBlockData* blockData = SC<XSTBlockData*>(textBlock.userData());
//      if (blockData)
//      {
//         writer.writeStartElement("blockData");
//         writer.writeAttribute("blockNumber", NUM2STRING(textBlock.blockNumber()));
//         blockData->write(writer);
//         writer.writeEndElement();     // blockData
//      }
      textBlock = textBlock.next();
   }
   writer.writeEndElement();    // contents
   writer.writeEndDocument();

   if (!filename.isEmpty())
      setFilename(filename);
//   _pristine = false;
}
//-----------------------------------------------------------------------------
bool editorWidget::hasSelectedText () const {return textCursor().hasSelection();}
QString editorWidget::selectedText () const
{
   return textCursor().selectedText();
/*
   if (_userMultiSelect) {
      int mainSelect = SendScintilla(SCI_GETMAINSELECTION);
      return text(SendScintilla(SCI_GETSELECTIONNSTART, mainSelect), SendScintilla(SCI_GETSELECTIONNEND, mainSelect));
   }
   else
      return QsciScintilla::selectedText();
*/
}
int editorWidget::selectedCount () const {return (hasSelectedText()) ? 1 : 0;}
void editorWidget::updatePreferences ()
{
   // setup preferences
   QSettings settings;
/*
   settings.beginGroup(PREFERENCE_SETTINGS);

   setIndentationsUseTabs(settings.value(TAB_POLICY, TAB_POLICY_DEFAULT).toBool());      // true = use tabs, false = use spaces
   setIndentationWidth(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   setTabWidth(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   setAutoIndent(settings.value(AUTO_INDENT, AUTO_INDENT_DEFAULT).toBool());
   setBackspaceUnindents(settings.value(BACK_UNINDENT, BACK_UNINDENT_DEFAULT).toBool());

   indentation(settings.value(INDENTATION, INDENTATION_DEFAULT).toBool());

   _selectBackground      = settings.value(SELECTION_BG_COLOR, SELECTION_BG_COLOR_DEFAULT).toString();
   _selectForeground      = settings.value(SELECTION_FG_COLOR, SELECTION_FG_COLOR_DEFAULT).toString();
   _mainSelectBackground  = settings.value(MAIN_SELECTION_BG_COLOR, MAIN_SELECTION_BG_COLOR_DEFAULT).toString();
   _mainSelectForeground  = settings.value(MAIN_SELECTION_FG_COLOR, MAIN_SELECTION_FG_COLOR_DEFAULT).toString();
   _multiSelectBackground = settings.value(MULTI_SELECTION_BG_COLOR, MULTI_SELECTION_BG_COLOR_DEFAULT).toString();
   _multiSelectForeground = settings.value(MULTI_SELECTION_FG_COLOR, MULTI_SELECTION_FG_COLOR_DEFAULT).toString();
*/
}
//-----------------------------------------------------------------------------
void editorWidget::del () {}
void editorWidget::selectWord () {textCursor().select(QTextCursor::WordUnderCursor);}
void editorWidget::selectLine () {textCursor().select(QTextCursor::BlockUnderCursor);}
void editorWidget::selectAll () {textCursor().select(QTextCursor::Document);}
//-----------------------------------------------------------------------------
void editorWidget::indent ()
{
   QTextBlockFormat format;
   format.setIndent(1);
   textCursor().mergeBlockFormat(format);
}
void editorWidget::unindent () {}
void editorWidget::uppercase ()
{
   QTextCharFormat format;
   format.setFontCapitalization(QFont::AllUppercase);
   textCursor().mergeBlockCharFormat(format);
}
void editorWidget::lowercase ()
{
   QTextCharFormat format;
   format.setFontCapitalization(QFont::AllLowercase);
   textCursor().mergeBlockCharFormat(format);
}
void editorWidget::capitalize ()
{
   QTextCharFormat format;
   format.setFontCapitalization(QFont::Capitalize);
   textCursor().mergeBlockCharFormat(format);
}
void editorWidget::changeCase () {}
void editorWidget::tabs2Spaces () {}
void editorWidget::spaces2Tabs () {}

//-----------------------------------------------------------------------------
void editorWidget::find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap) {}
void editorWidget::replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap) {}
//-----------------------------------------------------------------------------
void editorWidget::mousePressEvent (QMouseEvent* e)
{
   QTextEdit::mousePressEvent(e);
   emit copyAvailable(canPaste());
   emit redoAvailable(document()->isRedoAvailable());
   emit undoAvailable(document()->isUndoAvailable());
}
//-----------------------------------------------------------------------------
QString editorWidget::changeCase (QString text)
{
   int s = text.size();
   for (int i = 0; i < s; i++) {
      QChar c = text.at(i);
      text.replace(i, 1, (c.isUpper()) ? c.toLower() : c.toUpper());
   }
   return text;
}
QString editorWidget::capitalize (QString text)
{
   int s = text.size();
   bool whitespace = true;
   for (int i = 0; i < s; i++) {
      QChar c = text.at(i);
      text.replace(i, 1, (whitespace) ? c.toUpper() : c.toLower());
      whitespace = !c.isLetter();
   }
   return text;
}
void editorWidget::changeText (ChangeType type)
{
   QString text = selectedText();
   QString newText = (type == ToUpper)    ? text.toUpper()   :
                     (type == ToLower)    ? text.toLower()   :
                     (type == Capitalize) ? capitalize(text) :
                     (type == ChangeCase) ? changeCase(text) : QString();
//   replaceSelectedText(newText);
//   setSelection(lineFrom, indexFrom, lineTo, indexTo);
}
//-----------------------------------------------------------------------------
void editorWidget::setup ()
{
   setContextMenuPolicy(Qt::NoContextMenu);
   QFont f = font();
   f.setFamily("Courier");
   f.setPointSize(10);
   setFont(f);

   connect(document(), SIGNAL(modificationChanged(bool)), this, SLOT(updateModification(bool)));
}

