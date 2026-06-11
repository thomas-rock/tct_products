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

#include <QtWidgets>
#include "editor_widget.h"
#include "lexer_manager.h"

#define MARKER_MASK(X)        (0x1 << X)

#define MARGIN_MARKERS        0
#define MARGIN_LINE_NUMBERS   1
#define MARGIN_CHANGE_BARS    2
#define MARGIN_FOLDING        3

#define CHANGE_BAR_RED        8
#define CHANGE_BAR_GREEN      9
#define CHANGE_BAR_WIDTH      2
#define MARKER_WIDTH          20

#define SELECT_COUNT          SendScintilla(SCI_GETSELECTIONS)

using namespace texteditLib;

EditorWidget::EditorWidget (QWidget* parent) : QsciScintilla(parent) {setup();}
EditorWidget::~EditorWidget () {}

//-----------------------------------------------------------------------------
bool EditorWidget::isPristine () const {return m_pristine  && QsciScintilla::text().isEmpty() && !QsciScintilla::isModified();}
QString EditorWidget::filename() const {return m_filename;}

//    does not affect pristine
void EditorWidget::setFilename (const QString& filename)
{
   m_filename = filename;
   updatePreferences();
}
QString EditorWidget::language () const
{
   QsciLexer* l = lexer();
   if (l != nullptr) return l->language();

   return "Text";
}

//-----------------------------------------------------------------------------
void EditorWidget::setLanguage (const QString& lang) {setLexer(LexerManager::lexerByLang(lang));}
void EditorWidget::setText (const QString& text, bool readonly)
{
   m_initializing = true;
   QsciScintilla::setText(text);
   QsciScintilla::setModified(false);
   QsciScintilla::setReadOnly(readonly);
   m_pristine = false;
   m_initializing = false;
}

//-----------------------------------------------------------------------------
void EditorWidget::fileSaved (const QString& filename)
{
   int vl = SendScintilla(SCI_GETFIRSTVISIBLELINE);
   int changeMask = MARKER_MASK(CHANGE_BAR_RED);
   for (int line = 0; line < lines(); line++)
      if ((markersAtLine(line) & changeMask) != 0) {
         markerDelete(line, CHANGE_BAR_RED);
         markerAdd(line, CHANGE_BAR_GREEN);
      }

   if (!filename.isEmpty())
      setFilename(filename);
//   _pristine = false;
   QsciScintilla::setModified(false);
   SendScintilla(SCI_SETFIRSTVISIBLELINE, vl);
}
//-----------------------------------------------------------------------------
bool EditorWidget::hasSelectedText () const {return !SendScintilla(SCI_GETSELECTIONEMPTY);}
QString EditorWidget::selectedText () const
{
   if (m_user_multi_select) {
      int mainSelect = SendScintilla(SCI_GETMAINSELECTION);
      return text(SendScintilla(SCI_GETSELECTIONNSTART, mainSelect), SendScintilla(SCI_GETSELECTIONNEND, mainSelect));
   }
   else
      return QsciScintilla::selectedText();
}
int EditorWidget::selectedCount () const {return (hasSelectedText()) ? SELECT_COUNT : 0;}
void EditorWidget::updatePreferences ()
{
   // setup preferences
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   setIndentationsUseTabs(settings.value(TAB_POLICY, TAB_POLICY_DEFAULT).toBool());      // true = use tabs, false = use spaces
   setIndentationWidth(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   setTabWidth(settings.value(TAB_SIZE, TAB_SIZE_DEFAULT).toInt());
   setAutoIndent(settings.value(AUTO_INDENT, AUTO_INDENT_DEFAULT).toBool());
   setBackspaceUnindents(settings.value(BACK_UNINDENT, BACK_UNINDENT_DEFAULT).toBool());
   m_auto_pair = settings.value(AUTO_PAIR, AUTO_PAIR_DEFAULT).toBool();
   m_auto_enclose = settings.value(AUTO_ENCLOSE, AUTO_ENCLOSE_DEFAULT).toBool();
   setWrapMode((settings.value(LINE_WRAP, LINE_WRAP_DEFAULT).toBool()) ? WrapWhitespace : WrapNone);
   int rightEdge = settings.value(RIGHT_EDGE, RIGHT_EDGE_DEFAULT).toInt();
   setEdgeMode((rightEdge == -1) ? EdgeNone : EdgeLine);
   if (rightEdge != -1)
      setEdgeColumn(rightEdge);

   lineNumbers(settings.value(LINE_NUMBERS, LINE_NUMBERS_DEFAULT).toBool());
   markers(settings.value(MARKERS, MARKERS_DEFAULT).toBool());
   changeBars(settings.value(CHANGE_BARS, CHANGE_BARS_DEFAULT).toBool());
//   folding(settings.value(FOLDING, FOLDING_DEFAULT).toBool());
   indentation(settings.value(INDENTATION, INDENTATION_DEFAULT).toBool());
   whitespace(settings.value(WHITESPACE, WHITESPACE_DEFAULT).toBool());
   eols(settings.value(END_OF_LINE, END_OF_LINE_DEFAULT).toBool());
   highlight(settings.value(HIGHLIGHT_LINE, HIGHLIGHT_LINE_DEFAULT).toBool());
   match(settings.value(MATCH_BRACES, MATCH_BRACES_DEFAULT).toBool());
   markerIcon(settings.value(MARKER_ICON, MARKER_ICON_DEFAULT).toInt());
   setPaper(QColor(settings.value(BACKGROUND_COLOR, BACKGROUND_COLOR_DEFAULT).toString()));

   m_select_background       = settings.value(SELECTION_BG_COLOR, SELECTION_BG_COLOR_DEFAULT).toString();
   m_select_foreground       = settings.value(SELECTION_FG_COLOR, SELECTION_FG_COLOR_DEFAULT).toString();
   m_main_select_background  = settings.value(MAIN_SELECTION_BG_COLOR, MAIN_SELECTION_BG_COLOR_DEFAULT).toString();
   m_main_select_foreground  = settings.value(MAIN_SELECTION_FG_COLOR, MAIN_SELECTION_FG_COLOR_DEFAULT).toString();
   m_multi_select_background = settings.value(MULTI_SELECTION_BG_COLOR, MULTI_SELECTION_BG_COLOR_DEFAULT).toString();
   m_multi_select_foreground = settings.value(MULTI_SELECTION_FG_COLOR, MULTI_SELECTION_FG_COLOR_DEFAULT).toString();
   setCaretLineBackgroundColor(QColor(settings.value(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR_DEFAULT).toString()));
   setEdgeColor(QColor(settings.value(RIGHT_EDGE_COLOR, RIGHT_EDGE_COLOR_DEFAULT).toString()));
   setMarginsForegroundColor(QColor(settings.value(MARGIN_FG_COLOR, MARGIN_FG_COLOR_DEFAULT).toString()));
   setMarginsBackgroundColor(QColor(settings.value(MARGIN_BG_COLOR, MARGIN_BG_COLOR_DEFAULT).toString()));
   setIndentationGuidesForegroundColor(QColor(settings.value(INDENTATION_FG_COLOR, INDENTATION_FG_COLOR_DEFAULT).toString()));
   setIndentationGuidesBackgroundColor(QColor(settings.value(INDENTATION_BG_COLOR, INDENTATION_BG_COLOR_DEFAULT).toString()));
   setMatchedBraceForegroundColor(QColor(settings.value(MATCHED_FG_COLOR, MATCHED_FG_COLOR_DEFAULT).toString()));
   setMatchedBraceBackgroundColor(QColor(settings.value(MATCHED_BG_COLOR, MATCHED_BG_COLOR_DEFAULT).toString()));
   setUnmatchedBraceForegroundColor(QColor(settings.value(UNMATCHED_FG_COLOR, UNMATCHED_FG_COLOR_DEFAULT).toString()));
   setUnmatchedBraceBackgroundColor(QColor(settings.value(UNMATCHED_BG_COLOR, UNMATCHED_BG_COLOR_DEFAULT).toString()));
   setWhitespaceForegroundColor(QColor(settings.value(WHITESPACE_FG_COLOR, WHITESPACE_FG_COLOR_DEFAULT).toString()));
   setWhitespaceBackgroundColor(QColor(settings.value(WHITESPACE_BG_COLOR, WHITESPACE_BG_COLOR_DEFAULT).toString()));

   setLexer(LexerManager::lexerByExt(QFileInfo(m_filename).suffix()));
}

//-----------------------------------------------------------------------------
// general slots
void EditorWidget::clearMarkers (MarkerNumber number)
{
   if (number == All) {
      markerDeleteAll(Status);
      markerDeleteAll(Warning);
      markerDeleteAll(Error);
      markerDeleteAll(User);
   }
   else
      markerDeleteAll(number);
}
void EditorWidget::setMarker (int line, MarkerNumber number, bool state)
{
   if (state) {
      if (number == All) return;    // no can do
      markerAdd(line, number);
   }
   else
      markerDelete(line, number);
}
//-----------------------------------------------------------------------------
// edit slots
void EditorWidget::copy ()
{
   if (m_user_multi_select) QApplication::clipboard()->setText(selectedText());
   else QsciScintilla::copy();
}
void EditorWidget::cut ()
{
   SelectionType seltype = selectionType();
   if (seltype == UserSelection) {
      int mainSel = SendScintilla(SCI_GETMAINSELECTION);
      int start = SendScintilla(SCI_GETSELECTIONNSTART, mainSel);
      int end = SendScintilla(SCI_GETSELECTIONNEND, mainSel);
      SendScintilla(SCI_CLEARSELECTIONS);
      SendScintilla(SCI_SETSEL, start, end);
   }
   QsciScintilla::cut();
}
void EditorWidget::del ()
{
   SelectionType seltype = selectionType();
   if (seltype == UserSelection) {
      int mainSel = SendScintilla(SCI_GETMAINSELECTION);
      int start = SendScintilla(SCI_GETSELECTIONNSTART, mainSel);
      int end = SendScintilla(SCI_GETSELECTIONNEND, mainSel);
      SendScintilla(SCI_CLEARSELECTIONS);
      SendScintilla(SCI_SETSEL, start, end);
   }
   removeSelectedText();
}
////    paste provided text - don't use clipboard text
//void EditorWidget::paste (QString text)
//{
//   if (hasSelection())
//      replaceSelectedText(text);
//   else
//      insert(text);
//}
void EditorWidget::selectWord ()
{
   clearSelections();
   int position = SendScintilla(SCI_GETCURRENTPOS);
   SendScintilla(SCI_SETANCHOR, SendScintilla(SCI_WORDSTARTPOSITION, position, true));
   SendScintilla(SCI_SETCURRENTPOS, SendScintilla(SCI_WORDENDPOSITION, position, true));
}
void EditorWidget::selectLine ()
{
   clearSelections();
   int line;
   int index;
   getCursorPosition(&line, &index);
   SendScintilla(SCI_SETANCHOR, SendScintilla(SCI_POSITIONFROMLINE, line));
   SendScintilla(SCI_SETCURRENTPOS, SendScintilla(SCI_GETLINEENDPOSITION, line));
}
void EditorWidget::selectAll ()
{
   clearSelections();
   QsciScintilla::selectAll();
}
void EditorWidget::selectText (int line, int position, int length)
{
   int lineTo;
   int indexTo;
   lineIndexFromPosition(positionFromLineIndex(line, position)+length, &lineTo, &indexTo);
   setSelection (line, position, lineTo, indexTo);
}
void EditorWidget::selectText (int sline, int spos, int eline, int epos) {setSelection (sline, spos, eline, epos);}
void EditorWidget::indent ()
{
   if (hasSelectedText()) {
      beginUndoAction();
      int lineFrom, indexFrom, lineTo, indexTo;
      getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
      for (int line = lineFrom; line < lineTo; line++)
         QsciScintilla::indent(line);
      endUndoAction();
   }
   else {
      int line, index;
      getCursorPosition(&line, &index);
      QsciScintilla::indent(line);
   }
}
void EditorWidget::unindent ()
{
   if (hasSelectedText()) {
      beginUndoAction();
      int lineFrom, indexFrom, lineTo, indexTo;
      getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
      for (int line = lineFrom; line < lineTo; line++)
         QsciScintilla::unindent(line);
      endUndoAction();
   }
   else {
      int line, index;
      getCursorPosition(&line, &index);
      QsciScintilla::unindent(line);
   }
}
void EditorWidget::uppercase () {changeText(ToUpper);}
void EditorWidget::lowercase () {changeText(ToLower);}
void EditorWidget::capitalize () {changeText(Capitalize);}
void EditorWidget::changeCase () {changeText(ChangeCase);}
void EditorWidget::comment ()
{
   //    TODO: Text needs to stay selected after it is commented
   QString ext = QFileInfo(m_filename).suffix();
   if (ext.isEmpty()) return;
   QPair<QString, QString> comment = QPair<QString, QString>("//", "");   // TODO: xFileInfo::comment(ext);
   if (comment.first.isEmpty()) return;

   SelectionType seltype = selectionType();
   if (seltype == SingleSelection) {
      int lineFrom, indexFrom, lineTo, indexTo;
      getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
      beginUndoAction();
      if (comment.second.isEmpty())       // single-line comment
         for (int line = lineFrom; line < lineTo; line++)
            insertAt(comment.first, line, (line == lineFrom) ? indexFrom : 0);
      else {
         insertAt(comment.first, lineFrom, indexFrom);
         insertAt(comment.second, lineTo, indexTo);
      }
      endUndoAction();

      // reselect
      setSelection(lineFrom, indexFrom, lineTo, indexTo);
   }
   else if (seltype == NoSelection) {
      int line = SendScintilla(SCI_LINEFROMPOSITION, SendScintilla(SCI_GETCURRENTPOS));
      if (comment.second.isEmpty())       // single-line comment
         insertAt(comment.first, line, 0);
      else {
         insertAt(comment.first, line, 0);
         insertAt(comment.second, line, SendScintilla(SCI_LINELENGTH, line));
      }
   }
}
void EditorWidget::uncomment ()
{
   //    TODO: Make this better
   QString ext = QFileInfo(m_filename).suffix();
   if (ext.isEmpty()) return;
   QPair<QString, QString> comment = QPair<QString, QString>("//", "");   // TODO: xFileInfo::comment(ext);
   if (comment.first.isEmpty()) return;

   int length = comment.first.length();
   QString newText;
   if (hasSelectedText()) {
      if (!comment.second.isEmpty()) {           // multi-line
         QString text = selectedText();
         if (text.endsWith(comment.second))
            text.chop(comment.second.length());
         if (text.startsWith(comment.first))
            newText = text.mid(length);
      }
      else {
         QStringList text = selectedText().split("\n");
         for (int index = 0; index < text.size(); index++) {
            QString line = text.at(index);
            if (line.startsWith(comment.first))
               newText.append(line.mid(length) + "\n");
         }
      }
      replaceSelectedText(newText);
   }
}
void EditorWidget::windowsEol ()
{
   convertEols(EolWindows);
   setEolMode(EolWindows);
}
void EditorWidget::unixEol ()
{
   convertEols(EolUnix);
   setEolMode(EolUnix);
}
void EditorWidget::macEol ()
{
   convertEols(EolMac);
   setEolMode(EolMac);
}
void EditorWidget::trimTrailing () {replace("\\s+$", QString(), true, false, true, true, true, false, true);}
void EditorWidget::trimLeading () {replace("^\\s+", QString(), true, false, true, true, true, false, true);}
void EditorWidget::trimLeadingTrailing ()
{
   beginUndoAction();
   trimTrailing();
   trimLeading();
   endUndoAction();
}
void EditorWidget::tabs2Spaces ()
{
   QString spaces(indentationWidth(), ' ');
   replace("\\t", spaces, true, false, true, true, true, false, true);
}
void EditorWidget::spaces2Tabs ()
{
   QString spaces(indentationWidth(), ' ');
   replace(spaces, "\\t", true, false, true, true, true, false, true);
}
void EditorWidget::setReadOnly () {QsciScintilla::setReadOnly(true);}
void EditorWidget::clearReadOnly () {QsciScintilla::setReadOnly(false);}
//-----------------------------------------------------------------------------
// search slots
bool EditorWidget::find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap)
{
   if (text.isEmpty()) return false;

   if (!dir) SendScintilla(SCI_SWAPMAINANCHORCARET);

   bool ok = findFirst(text, re, cs, wo, wrap, dir);
   emit selectedCount(SELECT_COUNT);
   return ok;
}
int EditorWidget::replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap)
{
   if (text.isEmpty()) return 0;

   beginUndoAction();
   int count = 0;
   bool found = true;
   do {
      found = findFirst(text, re, cs, wo, wrap, dir);
      if (found) {
         QsciScintilla::replace(newtext);
         count++;
      }
   } while (all && found);

   if (!all && next) findFirst(text, re, cs, wo, wrap, dir);
   endUndoAction();

   return count;
}
void EditorWidget::gotoLine (int line) {setCursorPosition(line-1, 0);}
void EditorWidget::gotoMatch (bool select)
{
   if (select)
      selectToMatchingBrace();
   else
      moveToMatchingBrace();
}
void EditorWidget::nextMarker ()
{
   int line, index;
   getCursorPosition(&line, &index);

   if (line < lines()) {
      int l = markerFindNext(line+1, All);
      if (l != -1)
         setCursorPosition(l, 0);
   }
}
void EditorWidget::prevMarker ()
{
   int line, index;
   getCursorPosition(&line, &index);
   if (line > 0) {
      int l = markerFindPrevious(line-1, All);
      if (l != -1)
         setCursorPosition(l, 0);
   }
}
//-----------------------------------------------------------------------------
// view slots
void EditorWidget::lineNumbers (bool state)
{
   setMarginLineNumbers(MARGIN_LINE_NUMBERS, state);
   updateLineMargin();
}
bool EditorWidget::lineNumbers () {return marginLineNumbers(MARGIN_LINE_NUMBERS);}
void EditorWidget::markers (bool state) {setMarginWidth(MARGIN_MARKERS, (state) ? MARKER_WIDTH : 0);}
bool EditorWidget::markers () {return marginWidth(MARGIN_MARKERS) != 0;}
void EditorWidget::changeBars (bool state) {setMarginWidth(MARGIN_CHANGE_BARS, (state) ? CHANGE_BAR_WIDTH : 0);}
bool EditorWidget::changeBars () {return marginWidth(MARGIN_CHANGE_BARS) != 0;}
void EditorWidget::indentation (bool state) {setIndentationGuides(state);}
bool EditorWidget::indentation () {return indentationGuides();}
void EditorWidget::whitespace (bool state) {setWhitespaceVisibility((state) ? WsVisibleAfterIndent : WsInvisible);}
bool EditorWidget::whitespace () {return whitespaceVisibility() != WsInvisible;}
void EditorWidget::eols (bool state) {setEolVisibility(state);}
bool EditorWidget::eols () {return eolVisibility();}
void EditorWidget::highlight (bool state) {setCaretLineVisible(state);}
bool EditorWidget::highlight () {return SendScintilla(SCI_GETCARETLINEVISIBLE);}
void EditorWidget::match (bool state) {setBraceMatching((state) ? SloppyBraceMatch : NoBraceMatch );}
bool EditorWidget::match () {return braceMatching() != NoBraceMatch;}
void EditorWidget::markerIcon (int id)
{
   QImage icon;
   switch (id) {
      case 0  : icon = QImage(":textedit/greenCircle");    break;
      case 1  : icon = QImage(":textedit/yellowCircle");   break;
      case 2  : icon = QImage(":textedit/redCircle");      break;
      case 3  : icon = QImage(":textedit/whiteFlag");      break;
      case 4  : icon = QImage(":textedit/greenFlag");      break;
      case 5  : icon = QImage(":textedit/yellowFlag");     break;
      case 6  : icon = QImage(":textedit/redFlag");        break;
      case 7  : icon = QImage(":textedit/rightArrow");     break;
      case 8  : icon = QImage(":textedit/leftArrow");      break;
      case 9  : icon = QImage(":textedit/check");          break;
      case 10 : icon = QImage(":textedit/warning");        break;
      case 11 : icon = QImage(":textedit/error");          break;
      default : icon = QImage(":textedit/greenCircle");    break;
   }
   QImage icon16 = icon.scaled(16, 16);
   markerDefine(icon16, Status);
}
//-----------------------------------------------------------------------------
void EditorWidget::modChanged (bool state) {emit modificationChanged(this, state);}
void EditorWidget::updateLineMargin ()
{
   if (!marginLineNumbers(MARGIN_LINE_NUMBERS))     // line numbers not enabled
      setMarginWidth(MARGIN_LINE_NUMBERS, 0);
   else
      setMarginWidth(MARGIN_LINE_NUMBERS, QString::number(lines()) + "  ");
}
void EditorWidget::textModified (int pos, int mtype, const char *text, int len, int added, int line1, int foldNow, int foldPrev, int token, int annotationLinesAdded)
{
   if (!m_initializing)
   {
      if (mtype & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
      {
         int line, index;
         lineIndexFromPosition(pos, &line, &index);
         if ((markersAtLine(line) & MARKER_MASK(CHANGE_BAR_GREEN)) != 0)
            markerDelete(line, CHANGE_BAR_GREEN);
         markerAdd(line, CHANGE_BAR_RED);
      }
   }
}
void EditorWidget::marginClicked (int margin, int line)
{
   if (margin == MARGIN_MARKERS)
   {
      if ((markersAtLine(line) & (0x1 << Status)) == 0)
         markerAdd(line, Status);
      else
         markerDelete(line, Status);
   }
}
void EditorWidget::mouseReleaseEvent (QMouseEvent* e) {QsciScintilla::mouseReleaseEvent(e);}
void EditorWidget::mouseMoveEvent (QMouseEvent *e) {QsciScintilla::mouseMoveEvent(e);}
void EditorWidget::mouseDoubleClickEvent (QMouseEvent* e)
{
   QsciScintilla::mouseDoubleClickEvent(e);
   if (e->modifiers() != 0) return;

   QString text = QsciScintilla::selectedText();
   if (text.isEmpty() || !text.contains(QRegularExpression("^\\w+$"))) return;

   int vl = SendScintilla(SCI_GETFIRSTVISIBLELINE);
   int hl = SendScintilla(SCI_GETXOFFSET);

   SendScintilla(SCI_SETSELBACK, true, qtcolor2Int(m_main_select_background));
   SendScintilla(SCI_SETSELFORE, true, qtcolor2Int(m_main_select_foreground));
   SendScintilla(SCI_SETADDITIONALSELBACK, qtcolor2Int(m_multi_select_background));
   SendScintilla(SCI_SETADDITIONALSELFORE, qtcolor2Int(m_multi_select_foreground));
   m_user_multi_select = true;
   SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_WHOLEWORD | SCFIND_MATCHCASE);    // TODO: Make match case configurable
   SendScintilla(SCI_TARGETWHOLEDOCUMENT);
   SendScintilla(SCI_MULTIPLESELECTADDEACH);

   SendScintilla(SCI_ROTATESELECTION);

   SendScintilla(SCI_SETADDITIONALSELECTIONTYPING, false);

   SendScintilla(SCI_SETFIRSTVISIBLELINE, vl);
   SendScintilla(SCI_SETXOFFSET, hl);
   emit selectedCount(SELECT_COUNT);
}
void EditorWidget::mousePressEvent (QMouseEvent* e)
{
   if (e->button() != Qt::RightButton) {
      SendScintilla(SCI_SETADDITIONALSELECTIONTYPING, true);
      clearSelections();
      QsciScintilla::mousePressEvent(e);
      e->accept();
   }
}
void EditorWidget::contextMenuEvent (QContextMenuEvent *e)
{
   emit contextMenuRequest(e->globalPos());
   e->accept();
}
//-----------------------------------------------------------------------------
QString EditorWidget::changeCase (QString text)
{
   int s = text.size();
   for (int i = 0; i < s; i++) {
      QChar c = text.at(i);
      text.replace(i, 1, (c.isUpper()) ? c.toLower() : c.toUpper());
   }
   return text;
}
QString EditorWidget::capitalize (QString text)
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
void EditorWidget::changeText (ChangeType type)
{
   SelectionType seltype = selectionType();
   if (seltype == SingleSelection) {
      int lineFrom, indexFrom, lineTo, indexTo;
      getSelection (&lineFrom, &indexFrom, &lineTo, &indexTo);
      QString text = selectedText();
      QString newText = (type == ToUpper)    ? text.toUpper()   :
                        (type == ToLower)    ? text.toLower()   :
                        (type == Capitalize) ? capitalize(text) :
                        (type == ChangeCase) ? changeCase(text) : QString();
      replaceSelectedText(newText);
      setSelection(lineFrom, indexFrom, lineTo, indexTo);
   }
   else if (seltype == RectSelection) {
      // get rectangular selection area info
      int start, startv, end, endv;
      QList<QPair <int, int> > selections = getRectangleInfo(start, startv, end, endv);

      // change each selected line
      beginUndoAction();
      SendScintilla(SCI_CLEARSELECTIONS);
      for (int i = 0; i < selections.size(); i++) {
         SendScintilla(SCI_SETSELECTIONSTART, selections.at(i).first);
         SendScintilla(SCI_SETSELECTIONEND, selections.at(i).second);
         QString text = selectedText();
         QString newText = (type == ToUpper)    ? text.toUpper()   :
                           (type == ToLower)    ? text.toLower()   :
                           (type == Capitalize) ? capitalize(text) :
                           (type == ChangeCase) ? changeCase(text) : QString();
         replaceSelectedText(newText);
      }
      endUndoAction();

      // reselect rectangular area
      selectRectangle(start, startv, end, endv);
   }
   else if (seltype == UserSelection) {
      // get main selection
      int mainSelect = SendScintilla(SCI_GETMAINSELECTION);
      int start = SendScintilla(SCI_GETSELECTIONNANCHOR, mainSelect);
      int end   = SendScintilla(SCI_GETSELECTIONNCARET, mainSelect);

      // reselect just main selection
      SendScintilla(SCI_CLEARSELECTIONS);
      SendScintilla(SCI_SETSELECTIONSTART, start);
      SendScintilla(SCI_SETSELECTIONEND, end);

      // convert main selection to upper case
      QString text = selectedText();
      QString newText = (type == ToUpper)    ? text.toUpper()   :
                        (type == ToLower)    ? text.toLower()   :
                        (type == Capitalize) ? capitalize(text) :
                        (type == ChangeCase) ? changeCase(text) : QString();
      replaceSelectedText(newText);

      // reselect everything
      SendScintilla(SCI_SETSELECTIONSTART, start);
      SendScintilla(SCI_SETSELECTIONEND, end);
      SendScintilla(SCI_MULTIPLESELECTADDEACH);
      SendScintilla(SCI_SETMAINSELECTION, mainSelect);
   }
   else if (seltype == MultiSelection) {
      QList<QPair <int, int> > selections = getSelections();

      // change each selection
      beginUndoAction();
      SendScintilla(SCI_CLEARSELECTIONS);
      for (int i = 0; i < selections.size(); i++) {
         SendScintilla(SCI_SETSELECTIONSTART, selections.at(i).first);
         SendScintilla(SCI_SETSELECTIONEND, selections.at(i).second);
         QString text = selectedText();
         QString newText = (type == ToUpper)    ? text.toUpper()   :
                           (type == ToLower)    ? text.toLower()   :
                           (type == Capitalize) ? capitalize(text) :
                           (type == ChangeCase) ? changeCase(text) : QString();
         replaceSelectedText(newText);
      }

      // recreate selections
      SendScintilla(SCI_SETSELECTION, selections.first().second, selections.first().first);     // parameters are: caret, anchor. Selection pair is: <anchor, caret>
      for (int i = 1; i < selections.size(); i++)
         SendScintilla(SCI_ADDSELECTION, selections.at(i).second, selections.at(i).first);      // parameters are: caret, anchor. Selection pair is: <anchor, caret>

      endUndoAction();
   }
}
//-----------------------------------------------------------------------------
// selectionType
//    Four possibilities
//       1) Single selection
//       2) Rectangular selection
//       3) User multi-selection (i.e double-clicked a word)
//       4) Multi-selection (i.e. ctrl-mouse multi-selection)
EditorWidget::SelectionType EditorWidget::selectionType ()
{
   if (!hasSelectedText()) return NoSelection;

   int count = SELECT_COUNT;
   if      (count == 1)                               return SingleSelection;
   else if (SendScintilla(SCI_SELECTIONISRECTANGLE))  return RectSelection;
   else if (m_user_multi_select)                      return UserSelection;
   else                                               return MultiSelection;
}
QList<QPair<int, int> > EditorWidget::getRectangleInfo (int &start, int &startv, int &end, int &endv)
{
   start  = SendScintilla(SCI_GETRECTANGULARSELECTIONCARET);
   startv = SendScintilla(SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE);
   end    = SendScintilla(SCI_GETRECTANGULARSELECTIONANCHOR);
   endv   = SendScintilla(SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE);

   return getSelections();
}
QList<QPair<int, int> > EditorWidget::getSelections ()
{
   // get each selection
   int count = SELECT_COUNT;
   QList<QPair<int, int> > selections;
   for (int i = 0; i < count; i++)
      selections.append(qMakePair(SendScintilla(SCI_GETSELECTIONNANCHOR, i), SendScintilla(SCI_GETSELECTIONNCARET, i)));

   return selections;
}
void EditorWidget::selectRectangle (int start, int startv, int end, int endv)
{
   SendScintilla(SCI_CLEARSELECTIONS);
   SendScintilla(SCI_SETRECTANGULARSELECTIONCARET, start);
   SendScintilla(SCI_SETRECTANGULARSELECTIONCARETVIRTUALSPACE, startv);
   SendScintilla(SCI_SETRECTANGULARSELECTIONANCHOR, end);
   SendScintilla(SCI_SETRECTANGULARSELECTIONANCHORVIRTUALSPACE, endv);
}
void EditorWidget::clearSelections ()
{
   m_user_multi_select = false;
   SendScintilla(SCI_SETSELBACK, true, qtcolor2Int(m_select_background));
   SendScintilla(SCI_SETSELFORE, true, qtcolor2Int(m_select_foreground));
}
//-----------------------------------------------------------------------------
int EditorWidget::qtcolor2Int (const QString& color) const
{
   QColor c(color);
   return c.red() | c.green() << 8 | c.blue() << 16;
}
//-----------------------------------------------------------------------------
// setup
void EditorWidget::setup ()
{
   // setup defaults
#ifdef Q_OS_WIN
   setEolMode(EolWindows);
#else
   setEolMode(EolUnix);
#endif
   setWhitespaceSize(3);
   setWrapVisualFlags(WrapFlagByBorder);

   // setup multi-selections
   SendScintilla(SCI_SETMULTIPLESELECTION, true);
   SendScintilla(SCI_SETADDITIONALSELECTIONTYPING, true);
   SendScintilla(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION);
   SendScintilla(SCI_SETMULTIPASTE, true);

   // setup margins
   //------------------------------------------------------
   // initialize all margins
   for (int i = 0; i < 6; i++) {
      setMarginWidth(i, 0);
      setMarginLineNumbers(i, false);
      setMarginSensitivity(i, false);
      setMarginMarkerMask(i, 0x0);
   }

   // setup default font
   QFont f = font();
   f.setFamily("Courier New");
   f.setPointSize(12);
   setFont(f);
   setMarginsFont(f);

   // setup markers margin
   setMarginMarkerMask(MARGIN_MARKERS, 0xF);        // support 4 markers - status, warning, error, user
   setMarginSensitivity(MARGIN_MARKERS, true);

   // setup change bars margin
   markerDefine(FullRectangle, CHANGE_BAR_RED);
   markerDefine(FullRectangle, CHANGE_BAR_GREEN);
   setMarkerBackgroundColor(Qt::red, CHANGE_BAR_RED);
   setMarkerBackgroundColor(Qt::darkGreen, CHANGE_BAR_GREEN);
   setMarginMarkerMask(MARGIN_CHANGE_BARS, MARKER_MASK(CHANGE_BAR_RED) | MARKER_MASK(CHANGE_BAR_GREEN));

   // setup general connections
   connect(this, SIGNAL(linesChanged()),
                 SLOT(updateLineMargin()));
   connect(this, SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),
                 SLOT(marginClicked(int,int)));
   connect(this, SIGNAL(modificationChanged(bool)),
                 SLOT(modChanged(bool)));
   connect(this, SIGNAL(SCN_MODIFIED(int,int,const char *,int,int,int,int,int,int,int)),
                 SLOT(textModified(int,int,const char*,int,int,int,int,int,int,int)));

   updatePreferences();
}
