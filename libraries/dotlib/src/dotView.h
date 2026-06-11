#pragma once

#include <QMainWindow>
#include <QFileSystemModel>
#include <QCheckBox>
#include <QLabel>
#include "dotlib_global.h"


namespace dotLib
{
   namespace Ui {class dotView;}

   class DOTLIB_EXPORT dotView : public QMainWindow
   {
      Q_OBJECT

      private:
         Ui::dotView*         _ui;
         QStringList          _recentFiles;
         int                  _unnamedCount = 0;

         QMenu*               _recentMenu;
         QMenu*               _tabContextMenu;

      public:
                              dotView (QWidget* parent = nullptr);
                              ~dotView () override;

      public Q_SLOTS:
         void                 viewFile (const QString& filename);
         void                 viewDot (const QString& dot, const QString& title = QString());

      protected Q_SLOTS:
         void                 updateFileMenu ();
         void                 recentSelected (QAction* file);
         void                 updateViewMenu ();
         void                 updateWindowMenu ();
         void                 viewerChanged ();

      private:
         void                 setup ();
   };
}
