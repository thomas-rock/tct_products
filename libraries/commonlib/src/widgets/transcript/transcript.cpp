#include "transcript.h"
#include "message.h"

#include <QScrollBar>
#include <QSettings>
#include <QColor>

using namespace commonLib;

Transcript::Transcript (QWidget* parent) : QTextEdit(parent)
{
   setReadOnly(true);
   setFontFamily("courier");
}
//-----------------------------------------------------------------------------
void Transcript::addMessage (int type, const QString& msg)
{
   QSettings settings;

   setTextColor(commonLib::color(settings, type));
   append(msg);
   QScrollBar* sb = verticalScrollBar();
   sb->setSliderPosition(sb->maximum());
}
//-----------------------------------------------------------------------------
