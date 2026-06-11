#pragma once

#include <QMainWindow>
#include "pdflib_global.h"


namespace pdfLib
{
   namespace Ui {class pdfView;}

   class PDFLIB_EXPORT pdfView : public QMainWindow
   {
      Q_OBJECT

      private:
         Ui::pdfView*         _ui;

      public:
                              pdfView (QWidget* parent = nullptr);
                              ~pdfView () override;

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
