#pragma once

#include <QString>
#include <QObject>
#include <QMap>
#include <QSettings>
#include "commonlib_global.h"

// message types
#define MessageType     int

#define SysStatus       0x00000002
#define SysInfo         0x00000004
#define SysWarning      0x00000008
#define SysError        0x00000010
#define SysFatal        0x00000020

#define UserDebug       0x00000100
#define UserStatus      0x00000200
#define UserInfo        0x00000400
#define UserWarning     0x00000800
#define UserError       0x00001000
#define UserFatal       0x00002000

#define MESSAGE         emit message

namespace commonLib {
   extern const QMap<int, QColor> Color;
   extern const QMap<int, QString> Prefix;
   extern const QMap<int, QString> Suffix;

   // local functions
   QString                    id (QString cat, int type);
   QString                    format (QString text, const QString& filename = {}, const quint32 line = 0, const quint32 col = 0);

   // externally visible functions
   QColor  COMMONLIB_EXPORT   color (QSettings& settings, int type);
   QString COMMONLIB_EXPORT   prefix (QSettings& settings, int type, const QString& filename = {}, const quint32 line = 0, const quint32 col = 0);
   QString COMMONLIB_EXPORT   rawPrefix (QSettings& settings, int type);
   QString COMMONLIB_EXPORT   suffix (QSettings& settings, int type, const QString& filename = {}, const quint32 line = 0, const quint32 col = 0);
   QString COMMONLIB_EXPORT   rawSuffix (QSettings& settings, int type);

   void COMMONLIB_EXPORT      setColor (QSettings& settings, int type, const QColor& color);
   void COMMONLIB_EXPORT      setPrefix (QSettings& settings, int type, const QString& prefix);
   void COMMONLIB_EXPORT      setSuffix (QSettings& settings, int type, const QString& suffix);
}
