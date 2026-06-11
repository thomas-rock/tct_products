#pragma once

#include "commonlib_global.h"
#include <QTreeWidget>
#include <QFileSystemWatcher>

QT_BEGIN_NAMESPACE
namespace commonLib
{
   class COMMONLIB_EXPORT DirectoryList : public QTreeWidget
   {
      Q_OBJECT

      public:
                              DirectoryList (QWidget* parent = nullptr);

         void                 addPath (const QString& path);
         void                 removePath (const QString& path);

      Q_SIGNALS:
         void                 fileDoubleClicked (const QString& file);

      protected Q_SLOTS:
         void                 itemDoubleClicked (QTreeWidgetItem* item);

      private:
         QFileSystemWatcher   m_watcher;
   };
}
QT_END_NAMESPACE
