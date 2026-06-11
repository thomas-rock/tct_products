#pragma once

#include <QMainWindow>
#include "imagelib_global.h"


namespace imageLib
{
   namespace Ui {class imageView;}

   class IMAGELIB_EXPORT imageView : public QMainWindow
   {
      Q_OBJECT

      private:
         Ui::imageView*       _ui;

      public:
                              imageView (QWidget* parent = nullptr);
                              ~imageView () override;

      public Q_SLOTS:
         void                 view (const QString& filename);

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
