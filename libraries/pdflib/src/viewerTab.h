#pragma once

#include <QTabWidget>
#include "pdflib_global.h"

namespace pdfLib
{
   namespace Ui {class hexEditor;}

   class viewerWidget;
   class PDFLIB_EXPORT viewerTab : public QTabWidget
   {
      Q_OBJECT

      private:
         int                  _untitledCount = 0;
         QStringList          _recentFiles;

      public:
                              viewerTab (QWidget* parent = nullptr);
                              ~viewerTab() override;

         QStringList          recentFiles () const;
         void                 setRecentFiles (const QStringList& list);
         QStringList          filenames () const;

         QString              currentFilename () const;

      public Q_SLOTS:
         virtual void         view (const QString& filename);        // external access to opening a file

         virtual void         preferences ();
         virtual void         summary ();

         // file slots
         virtual void         fileOpen ();
         virtual void         fileOpen (const QStringList& filenames);
         virtual bool         fileOpen (const QString& filename);
         virtual void         fileCloseAll ();
         virtual void         fileClose ();
         virtual void         filePrint ();

         virtual void         zoomIn ();
         virtual void         zoomOut ();
         virtual void         zoomFit ();
         virtual void         zoom100 ();

      Q_SIGNALS:
         void                 viewerChanged ();
         void                 contextMenuRequest (QPoint pos);

      protected Q_SLOTS:
         // system slots
         virtual void         closeTab (int index);

      protected:
         void                 updatePreferences ();
         viewerWidget*        createViewer (QString filename = QString());
         bool                 viewerExists (QString filename) const;
         viewerWidget*        viewer (QString filename = QString()) const;    // gets current editor if filename is empty
         QList<viewerWidget*> viewers () const;
         void                 closeViewer (viewerWidget* ed);
   };
}
