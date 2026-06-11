#pragma once

#include "message.h"
#include <QColor>
#include <QFileInfo>

// Default values
const QMap<int, QColor> commonLib::Color = {
   {SysStatus       , Qt::blue},
   {SysInfo         , Qt::black},
   {SysWarning      , Qt::yellow},
   {SysError        , Qt::red},
   {SysFatal        , Qt::darkRed},

   {UserDebug       , Qt::blue},
   {UserStatus      , Qt::blue},
   {UserInfo        , Qt::black},
   {UserWarning     , Qt::yellow},
   {UserError       , Qt::red},
   {UserFatal       , Qt::darkRed}
};

const QMap<int, QString> commonLib::Prefix = {
   {SysStatus       , "Status (%f:%l:%c): "},
   {SysInfo         , "(%f:%l:%c)"},
   {SysWarning      , "Warning (%f:%l:%c): "},
   {SysError        , "Error (%f:%l:%c): "},
   {SysFatal        , "Fatal (%f:%l:%c): "},

   {UserDebug       , "Debug: "},
   {UserStatus      , ""},
   {UserInfo        , ""},
   {UserWarning     , "Warning: "},
   {UserError       , "Error: "},
   {UserFatal       , "Fatal: "}
};

const QMap<int, QString> commonLib::Suffix = {
   {SysStatus       , ""},
   {SysInfo         , ""},
   {SysWarning      , ""},
   {SysError        , ""},
   {SysFatal        , ""},

   {UserDebug       , ""},
   {UserStatus      , ""},
   {UserInfo        , ""},
   {UserWarning     , ""},
   {UserError       , ""},
   {UserFatal       , ""}
};

//-----------------------------------------------------------------------------
QString commonLib::id (QString cat, int type) {return QString("%1_%2").arg(cat, QString::number(type,16).toUpper());}
QString commonLib::format (QString text, const QString& filename, const quint32 line, const quint32 col)
{
   if (filename.isEmpty() && line == -1 && col == -1 && (text.contains("%f") || text.contains("%F") || text.contains("%l") || text.contains("%c"))) return QString();
   if (!filename.isEmpty()) {
      text.replace("%f", QFileInfo(filename).fileName());
      text.replace("%F", filename);
   }
   else {
      text.replace("%f", QString());
      text.replace("%F", QString());
   }
   QString l = (line > -1) ? QString::number(line) : "";
   text.replace("%l", l);
   QString c = (col > -1) ? QString::number(col) : "";
   text.replace("%c", c);

   return text;
}

//-----------------------------------------------------------------------------
QColor commonLib::color (QSettings& settings, int type)
{
   QString i = id("color", type);
   return (settings.contains(i)) ? settings.value(i).value<QColor>() : Color.value(type, Qt::black);
}
QString commonLib::prefix (QSettings& settings, int type, const QString& filename, const quint32 line, const quint32 col) {return format(rawPrefix(settings, type), filename, line, col);}
QString commonLib::rawPrefix (QSettings& settings, int type)
{
   QString i = id("prefix", type);
   return (settings.contains(i)) ? settings.value(i).toString() : Prefix.value(type, QString());
}
QString commonLib::suffix (QSettings& settings, int type, const QString& filename, const quint32 line, const quint32 col) {return format(rawSuffix(settings, type), filename, line, col);}
QString commonLib::rawSuffix (QSettings& settings, int type)
{
   QString i = id("suffix", type);
   return (settings.contains(i)) ? settings.value(i).toString() : Suffix.value(type, QString());
}
//-----------------------------------------------------------------------------
void commonLib::setColor (QSettings& settings, int type, const QColor& color) {settings.setValue(id("color", type), color);}
void commonLib::setPrefix (QSettings& settings, int type, const QString& prefix) {settings.setValue(id("prefix", type), prefix);}
void commonLib::setSuffix (QSettings& settings, int type, const QString& suffix) {settings.setValue(id("suffix", type), suffix);}
