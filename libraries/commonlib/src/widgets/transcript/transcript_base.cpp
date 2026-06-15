#include "transcript_base.h"
#include "message.h"

#include <QScrollBar>
#include <QFileInfo>
#include <QColor>

using namespace commonLib;

TranscriptBase::TranscriptBase() {}
//-----------------------------------------------------------------------------
QString TranscriptBase::format (MessageType type, const QString& msg)
{
   QString prefix = m_prefix_no_context.value(type);
   QString suffix = m_suffix_no_context.value(type);

   return QString("%1%2%3").arg(prefix, msg, suffix);
}
QString TranscriptBase::format (MessageType type, const QString& msg, const QString& file, int line, int col)
{
   QString l = (line > -1) ? QString::number(line) : "";
   QString c = (col > -1) ? QString::number(col) : "";

   QString prefix = m_prefix.value(type);
   prefix.replace("%f", QFileInfo(file).fileName());
   prefix.replace("%F", file);
   prefix.replace("%l", l);
   prefix.replace("%c", c);

   QString suffix = m_suffix.value(type);
   suffix.replace("%f", QFileInfo(file).fileName());
   suffix.replace("%F", file);
   suffix.replace("%l", l);
   suffix.replace("%c", c);

   return QString("%1%2%3").arg(prefix, msg, suffix);
}
