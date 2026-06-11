//=============================================================================
// X-Tek Corporation                                   http://www.x-tekcorp.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from X-Tek Corporation.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, X-Tek Corporation
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: XRtfEdit.h 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#pragma once

#include <QtWidgets>
#include "rtfeditlib_global.h"

namespace rtfeditLib
{
   class editorWidget;
   class RTFEDITLIB_EXPORT editorTab : public QTabWidget
   {
      Q_OBJECT

      private:
         int                     _untitledCount = 0;
         QStringList             _languages;
         QStringList             _recentFiles;

      public:
                                 editorTab (QWidget* parent = nullptr);
                                 ~editorTab () override;

         QStringList             recentFiles () const;
         void                    setRecentFiles (const QStringList& list);
         QStringList             filenames ();

         QString                 currentFilename () const;
         void                    cursorPosition (int* line, int* index) const;
         int                     currentLines () const;
         bool                    isPristine () const;
         bool                    isUndoAvailable () const;
         bool                    isRedoAvailable () const;
         bool                    hasSelectedText () const;
         int                     selectedCount () const;

      public Q_SLOTS:
         virtual void            edit (const QString& filename = QString(), int line = -1, int pos = -1);

         virtual void            preferences ();
         virtual void            summary ();

         // file slots
         virtual void            fileNew ();
         virtual void            fileOpen ();
         virtual void            fileOpen (const QStringList& filenames);
         virtual bool            fileOpen (const QString& filename, int line = -1, int pos = -1);
         virtual void            fileCloseAll ();
         virtual void            fileClose ();
         virtual void            fileRevert ();
         virtual bool            fileSave (editorWidget* ed = nullptr);
         virtual bool            fileSaveAs();
         virtual void            fileSaveAll();
         virtual void            filePrint ();

         // edit slots
         virtual void            undo ();
         virtual void            redo ();
         virtual void            cut ();
         virtual void            copy ();
         virtual void            paste ();
         virtual void            del ();
         virtual void            selectWord ();
         virtual void            selectLine ();
         virtual void            selectAll ();

         // format slots
         virtual void            indent ();
         virtual void            unindent ();
         virtual void            uppercase ();
         virtual void            lowercase ();
         virtual void            capitalize ();
         virtual void            changeCase ();
         virtual void            tabs2Spaces ();
         virtual void            spaces2Tabs ();
         virtual void            setReadOnly ();
         virtual void            clearReadOnly ();
         virtual void            copyPathname ();
         virtual void            copyFilename ();
         virtual void            copyPath ();
         virtual void            copyAllPathnames ();
         virtual void            copyAllFilenames ();
         virtual void            copyAllPaths ();

         // search slots
         virtual void            find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap);
         virtual void            replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap);

         // windows slots
         void                    setStatus (bool state);

      Q_SIGNALS:
         void                    message (int type, QString msg);
         void                    editorChanged ();
         void                    cursorPositionChanged(int line, int index);
         void                    selectionChanged ();
         void                    textChanged ();
         void                    contextMenuRequest (QPoint pos);
         void                    selectedCount (int count);
         void                    replacedCount (int count);

      protected Q_SLOTS:
         // system slots
         virtual void            closeTab (int index);
         virtual void            modificationChanged (editorWidget* ed, bool state);
         virtual void            fileChanged (QString filename);

      protected:
         void                    updatePreferences ();
         editorWidget*           createEditor (QString filename = QString(), bool usePristine = true);
         bool                    editorExists (QString filename) const;
         editorWidget*           editor (QString filename = QString()) const;    // gets current editor if filename is empty
         QList<editorWidget*>    editors () const;
         void                    closeEditor (editorWidget* ed);
      };
}
