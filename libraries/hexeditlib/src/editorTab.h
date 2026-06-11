#pragma once

#include <QTabWidget>
#include "editorlib_global.h"

namespace hexeditLib
{
   namespace Ui {class hexEditor;}

   class editorWidget;
   class HEXEDITLIB_EXPORT editorTab : public QTabWidget
   {
      Q_OBJECT

      private:
         int                  _untitledCount = 0;
         QStringList          _recentFiles;

      public:
                              editorTab (QWidget* parent = nullptr);
                              ~editorTab() override;

         QStringList          recentFiles () const;
         void                 setRecentFiles (const QStringList& list);
         QStringList          filenames () const;

         QString              currentFilename () const;
         qint64               currentAddress () const;
         qint64               currentSize () const;
         bool                 currentMode () const;
         bool                 isUndoAvailable () const;
         bool                 isRedoAvailable () const;
         bool                 isModified () const;

      public Q_SLOTS:
         virtual void         edit (const QString& filename, int line = -1, int pos = -1);        // external access to opening a file

         virtual void         preferences ();
         virtual void         summary ();

         // file slots
         virtual void         fileNew ();
         virtual void         fileOpen ();
         virtual void         fileOpen (const QStringList& filenames);
         virtual bool         fileOpen (const QString& filename, int line = -1, int pos = -1);
         virtual void         fileCloseAll ();
         virtual void         fileClose ();
         virtual void         fileRevert ();
         virtual bool         fileSave(editorWidget* ed = nullptr);
         virtual bool         fileSaveAs();
         virtual void         fileSaveAll();
         virtual void         filePrint ();

         // edit slots
         virtual void         undo ();
         virtual void         redo ();
         virtual void         cut ();
         virtual void         copy ();
         virtual void         del ();
         virtual void         paste ();

         // find/replace slots
         virtual void         find ();
         virtual void         find (QString text, bool hex, bool rev);
         virtual void         replace (QString ftext, bool fhex, QString rtext, bool rhex, bool rev);
         virtual void         replaceAll (QString ftext, bool fhex, QString rtext, bool rhex);
         virtual void         gotoAddress ();
         virtual void         tabSelected (QAction* action);

      Q_SIGNALS:
         void                 editorChanged ();
         void                 currentAddressChanged (qint64 address);
         void                 currentSizeChanged (qint64 size);
         void                 dataChanged ();
         void                 overwriteModeChanged (bool state);
         void                 contextMenuRequest (QPoint pos);

      protected Q_SLOTS:
         // system slots
         virtual void         closeTab (int index);
         virtual void         modificationChanged ();

      protected:
         void                 updatePreferences ();
         editorWidget*        createEditor (QString filename = QString(), bool usePristine = true);
         bool                 editorExists (QString filename) const;
         editorWidget*        editor (QString filename = QString()) const;    // gets current editor if filename is empty
         QList<editorWidget*> editors () const;
         void                 closeEditor (editorWidget* ed);
   };
}
