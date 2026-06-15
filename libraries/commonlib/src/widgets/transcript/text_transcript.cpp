#include "text_transcript.h"
#include "message.h"

#include <QScrollBar>
#include <QSettings>
#include <QColor>

using namespace commonLib;

TextTranscript::TextTranscript (QWidget* parent) : QTextEdit(parent), TranscriptBase()
{
   setReadOnly(true);
   setFontFamily("courier");
}
//-----------------------------------------------------------------------------
void TextTranscript::addMessage (MessageType type, const QString& msg)
{
   setTextColor(getColor(type));
   append(format(type, msg));

   QScrollBar* sb = verticalScrollBar();
   sb->setSliderPosition(sb->maximum());
}
//-----------------------------------------------------------------------------
void TextTranscript::addMessage (MessageType type, const QString& msg, const QString& file, int line, int col)
{
   setTextColor(getColor(type));
   append(format(type, msg, file, line, col));

   QScrollBar* sb = verticalScrollBar();
   sb->setSliderPosition(sb->maximum());
}
//-----------------------------------------------------------------------------
