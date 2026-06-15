#pragma once

#include "commonlib_global.h"
#include "transcript_base.h"
#include <QListWidget>
#include "message.h"

namespace commonLib
{
   class COMMONLIB_EXPORT ListTranscript : public QListWidget, public TranscriptBase
   {
      Q_OBJECT

      public:
                        ListTranscript (QWidget* parent = nullptr);

      public Q_SLOTS:
         void           addMessage (MessageType type, const QString& msg);
         void           addMessage (MessageType type, const QString& msg, const QString& file, int line, int col);

      Q_SIGNALS:
         void           positionRequest (QString file, int line, int col);

      protected Q_SLOTS:
         void           itemDoubleClicked (QListWidgetItem* item);

      private:
         const int      m_file_id = 257;
         const int      m_line_id = 258;
         const int      m_col_id  = 259;
   };
}
