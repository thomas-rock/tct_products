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
// $Id: XRtfEditWidget.h 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
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
   class RTFEDITLIB_EXPORT editorWidget : public QTextEdit
   {
      Q_OBJECT

      protected:
         enum ChangeType
         {
            ToLower        = 0x0,
            ToUpper        = 0x1,
            Capitalize     = 0x2,
            ChangeCase     = 0x3
         };

         QString                    _filename;            // don't use documentTitle - it gets cleared if new text is loaded
         bool                       _pristine;

      public:
                                    editorWidget (QWidget* parent = nullptr);
                                    ~editorWidget () override;

         bool                       isPristine () const;
         bool                       isModified () const;
         bool                       isRedoAvailable () const;
         bool                       isUndoAvailable () const;
         QString                    filename () const;
         int                        lines () const;
         void                       getCursorPosition (int* line, int* index);
         void                       setCursorPosition (int line, int index);
         void                       setFilename (const QString& filename);
         void                       setData (const QString& data, bool readonly);
         void                       setText (const QString& text, bool readonly);
         void                       fileSaved (const QString& filename = {});

         bool                       hasSelectedText () const;
         QString                    selectedText () const;
         int                        selectedCount () const;
         void                       updatePreferences ();

      public Q_SLOTS:
         // edit slots
         void                       del ();
         void                       selectWord ();
         void                       selectLine ();
         void                       selectAll ();

         // format slots
         void                       indent ();
         void                       unindent ();
         void                       uppercase ();
         void                       lowercase ();
         void                       capitalize ();
         void                       changeCase ();
         void                       tabs2Spaces ();
         void                       spaces2Tabs ();

         // search slots
         void                       find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap);
         void                       replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap);

      Q_SIGNALS:
         void                       modChanged (bool state);
         void                       textModified (int pos, int mtype, const char *text, int len, int added, int line, int foldNow, int foldPrev, int token, int annotationLinesAdded);
         void                       mouseReleaseEvent (QMouseEvent* e) override;
         void                       mouseMoveEvent (QMouseEvent* e) override;
         void                       mouseDoubleClickEvent (QMouseEvent* e) override;
         void                       contextMenuEvent (QContextMenuEvent *e) override;

      protected:
         void                       mousePressEvent (QMouseEvent* e) override;
         QString                    changeCase (QString text);
         QString                    capitalize (QString text);
         void                       changeText (ChangeType type);

      private:
         void                       setup ();
   };
}
