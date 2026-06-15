#pragma once

#include "commonlib_global.h"
#include <QTextEdit>

namespace commonLib
{
   class COMMONLIB_EXPORT Transcript : public QTextEdit
   {
      Q_OBJECT

      public:
                     Transcript (QWidget* parent = nullptr);

      public Q_SLOTS:
         void        addMessage (int type, const QString& msg);
   };
}
