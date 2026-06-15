//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================
#pragma once

#include <QTabWidget>
#include "texteditlib_global.h"
#include "editor_widget.h"
#include "message.h"

namespace texteditLib
{
   class TEXTEDITLIB_EXPORT EditorTab : public QTabWidget
   {
      Q_OBJECT

      public:
                              EditorTab (QWidget* parent = nullptr);
                              ~EditorTab () override;

         QStringList          languages () const;
         QStringList          recentFiles () const;
         void                 setRecentFiles (const QStringList& list);
         QStringList          filenames () const;

         QString              currentLanguage () const;
         QString              currentFilename () const;
         void                 cursorPosition (int* line, int* index) const;
         int                  currentLines () const;
         bool                 isPristine () const;
         bool                 isUndoAvailable () const;
         bool                 isRedoAvailable () const;
         bool                 hasSelectedText () const;
         int                  selectedCount () const;

      public Q_SLOTS:
         virtual void         edit (const QString& filename, int line = -1, int pos = -1);        // external access to opening a file
         virtual void         check ();

         virtual void         gotoLine ();
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
         virtual bool         fileSave (EditorWidget* ed = nullptr);
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
         virtual void         selectWord ();
         virtual void         selectLine ();
         virtual void         selectMatch ();
         virtual void         selectAll ();
         virtual void         indent ();
         virtual void         unindent ();
         virtual void         uppercase ();
         virtual void         lowercase ();
         virtual void         capitalize ();
         virtual void         changeCase ();
         virtual void         comment ();
         virtual void         uncomment ();
         virtual void         windowsEol ();
         virtual void         unixEol ();
         virtual void         macEol ();
         virtual void         trimTrailing ();
         virtual void         trimLeading ();
         virtual void         trimLeadingTrailing ();
         virtual void         tabs2Spaces ();
         virtual void         spaces2Tabs ();
         virtual void         setReadOnly ();
         virtual void         clearReadOnly ();
         virtual void         copyPathname ();
         virtual void         copyFilename ();
         virtual void         copyPath ();
         virtual void         copyAllPathnames ();
         virtual void         copyAllFilenames ();
         virtual void         copyAllPaths ();

         // find/replace slots
         virtual void         find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap);
         virtual void         replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap);
         virtual void         gotoLine (int line);
         virtual void         gotoMatch (bool select = false);
         virtual void         nextMarker ();
         virtual void         prevMarker ();
         virtual void         clearMarkers ();

         // view slots
         virtual void         lineNumbers (bool state);
         virtual bool         lineNumbers ();
         virtual void         markers (bool state);
         virtual bool         markers ();
         virtual void         changeBars (bool state);
         virtual bool         changeBars ();
//         void                  folding (bool state);
         virtual void         indentation (bool state);
         virtual bool         indentation ();
         virtual void         whitespace (bool state);
         virtual bool         whitespace ();
         virtual void         eols (bool state);
         virtual bool         eols ();
         virtual void         highlight (bool state);
         virtual bool         highlight ();
         virtual void         match (bool state);
         virtual bool         match ();
//         void                  foldAll ();
//         void                  unfoldAll ();
//         void                  hideSelectedLines ();

         virtual void         languageSelected (QAction* action);
         virtual void         tabSelected (QAction* action);

      Q_SIGNALS:
         void                 message (MessageType type, const QString& msg);
         void                 message (MessageType type, const QString& msg,  const QString& file, int line, int col);
         void                 editorChanged ();
         void                 cursorPositionChanged(int line, int index);
         void                 selectionChanged ();
         void                 textChanged ();
         void                 contextMenuRequest (QPoint pos);
         void                 selectedCount (int count);
         void                 replacedCount (int count);

      protected Q_SLOTS:
         // system slots
         virtual void         closeTab (int index);
         virtual void         modificationChanged (EditorWidget* ed, bool state);
         virtual void         fileChanged (QString filename);

      protected:
         void                 updatePreferences ();
         EditorWidget*        createEditor (QString filename = QString(), bool usePristine = true);
         bool                 editorExists (QString filename) const;
         EditorWidget*        editor (QString filename = QString()) const;    // gets current editor if filename is empty
         QList<EditorWidget*> editors () const;
         void                 closeEditor (EditorWidget* ed);


      private:
         int                  m_untitled_count = 0;
         QStringList          m_languages;
         QStringList          m_recent_files;

   };
}
