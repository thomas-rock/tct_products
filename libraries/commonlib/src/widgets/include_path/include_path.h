#pragma once

#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QMenu>
#include "commonlib_global.h"

QT_BEGIN_NAMESPACE
namespace commonLib
{
   namespace Ui {
      class IncludePath;
   }

   class COMMONLIB_EXPORT IncludePath : public QWidget
   {
      Q_OBJECT

      public:
                                 IncludePath (QWidget* parent = nullptr);

         void                    clear ();
         void                    addPath (const QString& path);
         void                    removePath (const QString& path);
         QStringList             paths () const;
         void                    setTitle (const QString& title);

      public Q_SLOTS:
         void                    addPath ();
         void                    removePath ();

      Q_SIGNALS:
         void                    pathAdded (const QString& path);
         void                    pathRemoved (const QString& path);
         void                    pathDoubleClicked (const QString& path);

      protected Q_SLOTS:
         void                    pathDoubleClicked (QTreeWidgetItem* item);
         void                    selectionChanged ();

      protected:
         QList<QTreeWidgetItem*> topLevelItems () const;

      private:
         Ui::IncludePath*        m_ui;
         QString                 m_title;
   };
}
QT_END_NAMESPACE
