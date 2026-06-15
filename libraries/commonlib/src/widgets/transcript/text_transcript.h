#pragma once

#include "commonlib_global.h"
#include "transcript_base.h"
#include <QTextEdit>

namespace commonLib
{
   class COMMONLIB_EXPORT TextTranscript : public QTextEdit, public TranscriptBase
   {
      Q_OBJECT

      public:
                     TextTranscript (QWidget* parent = nullptr);

      public Q_SLOTS:
         void        addMessage (MessageType type, const QString& msg);
         void        addMessage (MessageType type, const QString& msg, const QString& file, int line, int col);
   };
}
