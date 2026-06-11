#include "editorWidget.h"
#include "editorSettings_p.h"
#include <QApplication>

using namespace hexeditLib;

editorWidget::editorWidget (QWidget* parent) : QHexEdit(parent)
{
   setFont(QFont("Courier New", 12));
   updatePreferences();
}
//-----------------------------------------------------------------------------
QString editorWidget::filename () {return _filename;}
void editorWidget::setFilename (QString filename) {_filename = filename;}
bool editorWidget::setData(QString filename, bool readonly)
{
   _file.setFileName(filename);
   bool ok = QHexEdit::setData(_file);
   setReadOnly(readonly);
   if (ok)
      _pristine = false;

   return ok;
}
bool editorWidget::saveData ()
{
   QString tmpFileName = _filename + ".~tmp";

   QApplication::setOverrideCursor(Qt::WaitCursor);
   QFile file(tmpFileName);
   bool ok = write(file);
   ok = QFile::remove(_filename);
   if (ok) {
      file.setFileName(tmpFileName);
      ok = file.copy(_filename);
      QFile::remove(tmpFileName);
   }
   QApplication::restoreOverrideCursor();

   return ok;
}
//-----------------------------------------------------------------------------
bool editorWidget::isPristine () {return _pristine;}
bool editorWidget::isRedoAvailable () {return _undoStack->canRedo();}
bool editorWidget::isUndoAvailable () {return _undoStack->canUndo();}
qint64 editorWidget::currentAddress () {return _bPosCurrent;}
qint64 editorWidget::currentSize () {return _lastEventSize;}
void editorWidget::updatePreferences ()
{
   // setup preferences
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   setAddressArea(settings.value(SHOW_ADDRESS, SHOW_ADDRESS_DEFAULT).toBool());
   setAddressWidth(settings.value(ADDRESS_WIDTH, ADDRESS_WIDTH_DEFAULT).toInt());
   setAddressAreaColor(QColor(settings.value(ADDRESS_BG_COLOR, ADDRESS_BG_COLOR_DEFAULT).toString()));
   setAddressFontColor(QColor(settings.value(ADDRESS_TEXT_COLOR, ADDRESS_TEXT_COLOR_DEFAULT).toString()));

   setAsciiArea(settings.value(SHOW_ASCII, SHOW_ASCII_DEFAULT).toBool());
   setAsciiAreaColor(QColor(settings.value(ASCII_BG_COLOR, ASCII_BG_COLOR_DEFAULT).toString()));
   setAsciiFontColor(QColor(settings.value(ASCII_TEXT_COLOR, ASCII_TEXT_COLOR_DEFAULT).toString()));

   setHexCaps(settings.value(HEX_CAPS, HEX_CAPS_DEFAULT).toBool());
   setBytesPerLine(settings.value(BYTES, BYTES_DEFAULT).toInt());
   setHexFontColor(QColor(settings.value(HEX_TEXT_COLOR, HEX_TEXT_COLOR_DEFAULT).toString()));

   setHighlighting(settings.value(HIGHLIGHT, HIGHLIGHT_DEFAULT).toBool());
   setHighlightingColor(QColor(settings.value(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR_DEFAULT).toString()));

   setOverwriteMode(settings.value(OVERWRITE_MODE, OVERWRITE_MODE_DEFAULT).toBool());

   setSelectionColor(QColor(settings.value(SELECTION_COLOR, SELECTION_COLOR_DEFAULT).toString()));

   setReadOnly(settings.value(READ_ONLY, READ_ONLY_DEFAULT).toBool());
}
//-----------------------------------------------------------------------------
qint64 editorWidget::find (QString text, bool hex, bool rev)
{
   QByteArray findBa = getContent(hex, text);
   return findNext(findBa, rev);
}
void editorWidget::replace (QString ftext, bool fhex, QString rtext, bool rhex, bool rev)
{
   QByteArray findBa = getContent(fhex, ftext);
   QByteArray replaceBa = getContent(rhex, rtext);
   int idx = findNext(findBa, rev);
   if (idx >= 0)
      QHexEdit::replace(idx, findBa.length(), replaceBa);
}
int editorWidget::replaceAll (QString ftext, bool fhex, QString rtext, bool rhex)
{
   QByteArray findBa = getContent(fhex, ftext);
   QByteArray replaceBa = getContent(rhex, rtext);

   int flen = findBa.length();
   int replaceCounter = 0;
   int idx = findNext(findBa, false);

   while (idx >= 0) {
      QHexEdit::replace(idx, flen, replaceBa);
      replaceCounter++;
   }
   return replaceCounter;
}
//-----------------------------------------------------------------------------
qint64 editorWidget::findNext (QByteArray& ba, bool rev)
{
   qint64 from = cursorPosition() / 2;
   qint64 idx = -1;

   if (ba.length() > 0)
      idx = (rev) ? lastIndexOf(ba, from) : indexOf(ba, from);

   return idx;
}
QByteArray editorWidget::getContent (bool hex, const QString& input) {return (hex) ? QByteArray::fromHex(input.toLatin1()) : input.toUtf8();}
