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

#include <QString>
#include <QMouseEvent>
#include <Qsci/qsciscintilla.h>

#include "editor_settings_p.h"
#include "texteditlib_global.h"

namespace texteditLib
{
   class TEXTEDITLIB_EXPORT EditorWidget : public QsciScintilla
   {
      Q_OBJECT

      public:
         enum MarkerNumber
         {
            Status         = 0x0,
            Warning        = 0x1,
            Error          = 0x2,
            User           = 0x3,
            All            = 0x7
         };

      protected:
         enum SelectionType
         {
            NoSelection = 0,
            SingleSelection,
            RectSelection,
            MultiSelection,
            UserSelection
         };

         enum ChangeType
         {
            ToLower        = 0x0,
            ToUpper        = 0x1,
            Capitalize     = 0x2,
            ChangeCase     = 0x3
         };


      public:
                                    EditorWidget (QWidget* parent = nullptr);
                                    ~EditorWidget () override;

         bool                       isPristine () const;
         QString                    filename () const;
         void                       setFilename (const QString& filename);
         QString                    language () const;
         void                       setLanguage (const QString& lang);
         void                       setText (const QString& text, bool readonly);
         void                       fileSaved (const QString& filename = {});

         bool                       hasSelectedText () const;
         QString                    selectedText () const;
         int                        selectedCount () const;
         void                       updatePreferences ();

      public Q_SLOTS:
         // general slots
         void                       clearMarkers (MarkerNumber number = All);
         void                       setMarker (int line, MarkerNumber number, bool state);

         // edit slots
         void                       copy () override;
         void                       cut () override;
         void                       del ();
         void                       selectWord ();
         void                       selectLine ();
         void                       selectAll ();
         void                       selectText (int line, int position, int length);
         void                       selectText (int sline, int spos, int eline, int epos);
         void                       indent ();
         void                       unindent ();
         void                       uppercase ();
         void                       lowercase ();
         void                       capitalize ();
         void                       changeCase ();
         void                       comment ();
         void                       uncomment ();
         void                       windowsEol ();
         void                       unixEol ();
         void                       macEol ();
         void                       trimTrailing ();
         void                       trimLeading ();
         void                       trimLeadingTrailing ();
         void                       tabs2Spaces ();
         void                       spaces2Tabs ();
         void                       setReadOnly ();
         void                       clearReadOnly ();

         // search slots
         bool                       find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap);
         int                        replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap);
         void                       gotoLine (int line);
         void                       gotoMatch (bool select = false);
         void                       nextMarker ();
         void                       prevMarker ();

         // view slots
         void                       lineNumbers (bool state);
         bool                       lineNumbers ();
         void                       markers (bool state);
         bool                       markers ();
         void                       changeBars (bool state);
         bool                       changeBars ();
//         void                       folding (bool state);
//         bool                       folding ();
         void                       indentation (bool state);
         bool                       indentation ();
         void                       whitespace (bool state);
         bool                       whitespace ();
         void                       eols (bool state);
         bool                       eols ();
         void                       highlight (bool state);
         bool                       highlight ();
         void                       match (bool state);
         bool                       match ();
         void                       markerIcon (int id);
//         void                       foldAll ();
//         void                       unfoldAll ();
//         void                       hideSelectedLines ();

      Q_SIGNALS:
         void                       active (EditorWidget* self);
         void                       modificationChanged (EditorWidget* self, bool state);
         void                       markerChanged (const QString& filename, int line, bool state);
         void                       contextMenuRequest (const QPoint& pos);
         void                       selectedCount (int count);

      protected Q_SLOTS:
         void                       modChanged (bool state);
         void                       updateLineMargin ();
         void                       textModified (int pos, int mtype, const char *text, int len, int added, int line, int foldNow, int foldPrev, int token, int annotationLinesAdded);
         void                       marginClicked (int margin, int line);
         void                       mouseReleaseEvent (QMouseEvent* e) override;
         void                       mouseMoveEvent (QMouseEvent* e) override;
         void                       mouseDoubleClickEvent (QMouseEvent* e) override;
         void                       mousePressEvent (QMouseEvent* e) override;
         void                       contextMenuEvent (QContextMenuEvent *e);

      protected:
         QString                    changeCase (QString text);
         QString                    capitalize (QString text);
         void                       changeText (ChangeType type);

         SelectionType              selectionType ();
         QList<QPair<int, int> >    getRectangleInfo (int& start, int& startv, int& end, int& endv);
         QList<QPair<int, int> >    getSelections ();
         void                       selectRectangle (int start, int startv, int end, int endv);
         void                       clearSelections ();

         int                        qtcolor2Int (const QString& color) const;

      private:
         void                       setup ();

      private:
         bool                       m_user_multi_select        = false;
         QString                    m_filename;
         bool                       m_pristine;
         bool                       m_initializing           = false;

         // preferences
         QString                    m_select_background        = SELECTION_BG_COLOR_DEFAULT;
         QString                    m_select_foreground        = SELECTION_FG_COLOR_DEFAULT;
         QString                    m_main_select_background   = MAIN_SELECTION_BG_COLOR_DEFAULT;
         QString                    m_main_select_foreground   = MAIN_SELECTION_FG_COLOR_DEFAULT;
         QString                    m_multi_select_background  = MULTI_SELECTION_BG_COLOR_DEFAULT;
         QString                    m_multi_select_foreground  = MULTI_SELECTION_FG_COLOR_DEFAULT;
         bool                       m_auto_pair                = AUTO_PAIR_DEFAULT;
         bool                       m_auto_enclose             = AUTO_ENCLOSE_DEFAULT;

   };
}
