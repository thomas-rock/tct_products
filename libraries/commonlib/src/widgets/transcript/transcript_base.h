#pragma once

#include <QColor>
#include <QString>
#include <QMap>
#include "message.h"

namespace commonLib
{
   class TranscriptBase
   {
      public:
                           TranscriptBase ();

         void              setColor (MessageType type, QColor color) {m_color.insert(type, color);}
         void              setPrefix (MessageType type, const QString& prefix, bool no_context = false)
                           {
                              if (no_context) m_prefix_no_context.insert(type, prefix);
                              else            m_prefix.insert(type, prefix);
                           }
         void              setSuffix (MessageType type, const QString& suffix, bool no_context = false)
                           {
                              if (no_context) m_suffix_no_context.insert(type, suffix);
                              else            m_suffix.insert(type, suffix);
                           }

      protected:
         QColor            getColor (MessageType type) {return m_color.value(type, QColorConstants::Black);}
         QString           format (MessageType type, const QString& msg, const QString& file, int line, int col);
         QString           format (MessageType type, const QString& msg);

      protected:
         QMap<MessageType, QColor>  m_color = {
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

         QMap<MessageType, QString> m_prefix = {
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

         QMap<MessageType, QString> m_prefix_no_context = {
            {SysStatus       , "Status: "},
            {SysInfo         , ""},
            {SysWarning      , "Warning: "},
            {SysError        , "Error: "},
            {SysFatal        , "Fatal: "},

            {UserDebug       , "Debug: "},
            {UserStatus      , ""},
            {UserInfo        , ""},
            {UserWarning     , "Warning: "},
            {UserError       , "Error: "},
            {UserFatal       , "Fatal: "}
         };

         QMap<MessageType, QString> m_suffix = {
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

         QMap<MessageType, QString> m_suffix_no_context = {
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
   };
}
