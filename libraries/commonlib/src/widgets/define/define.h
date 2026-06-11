#pragma once

#include <QTreeWidget>
#include "commonlib_global.h"

QT_BEGIN_NAMESPACE
namespace commonLib
{
   namespace Ui {
      class Define;
   }

   class COMMONLIB_EXPORT Define : public QWidget
   {
      Q_OBJECT

      public:
                                 Define (QWidget* parent = nullptr);

         void                    clear ();
         QMap<QString, QString>  defines () const;
         void                    addDefine (const QString& name, const QString& value);
         void                    removeDefine (const QString& name);
         void                    setTitle (const QString& title);

      public Q_SLOTS:
         void                    addDefine ();
         void                    removeDefine ();

      Q_SIGNALS:
         void                    defineDoubleClicked (const QString& name, const QString& value);

      protected Q_SLOTS:
         void                    defineDoubleClicked (QTreeWidgetItem* item);
         void                    selectionChanged ();

      protected:
         QList<QTreeWidgetItem*> topLevelItems () const;

      private:
         Ui::Define*             m_ui;
         QString                 m_title;

   };
}
QT_END_NAMESPACE
