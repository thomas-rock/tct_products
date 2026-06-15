#include "list_transcript.h"
#include "message.h"

#include <QScrollBar>
#include <QFileInfo>
#include <QColor>

using namespace commonLib;

ListTranscript::ListTranscript (QWidget* parent) : QListWidget(parent), TranscriptBase()
{
   connect(this, &QListWidget::itemDoubleClicked,
           this, &ListTranscript::itemDoubleClicked);
}
//-----------------------------------------------------------------------------
void ListTranscript::addMessage (MessageType type, const QString& msg)
{
   QListWidgetItem* item = new QListWidgetItem(format(type, msg));
   item->setForeground(getColor(type));
   addItem(item);

   item->setData(m_line_id, -1);
   item->setData(m_col_id, -1);

   QScrollBar* sb = verticalScrollBar();
   sb->setSliderPosition(sb->maximum());
}
void ListTranscript::addMessage (MessageType type, const QString& msg, const QString& file, int line, int col)
{
   QListWidgetItem* item = new QListWidgetItem(format(type, msg, file, line, col));
   item->setData(m_file_id, file);
   item->setData(m_line_id, line);
   item->setData(m_col_id, col);
   item->setForeground(getColor(type));
   addItem(item);

   QScrollBar* sb = verticalScrollBar();
   sb->setSliderPosition(sb->maximum());
}
//-----------------------------------------------------------------------------
void ListTranscript::itemDoubleClicked (QListWidgetItem* item)
{
   emit positionRequest(item->data(m_file_id).toString(), item->data(m_line_id).toInt(), item->data(m_col_id).toInt());
}