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

#include <QStringList>
#include <QMainWindow>
#include <QDir>
#include <QLabel>

#include "texteditlib_global.h"


namespace texteditLib
{
   namespace Ui { class TextEdit; }

   class TEXTEDITLIB_EXPORT TextEdit : public QMainWindow
   {
      Q_OBJECT

      public:
                        TextEdit (QWidget* parent = nullptr);
                        ~TextEdit ();

      public Q_SLOTS:
         void           edit (const QString& filename, int line = 1, int col = 1);        // external access to open a file
         void           check ();

      protected Q_SLOTS:
         void           updateFileMenu ();
         void           recentSelected (QAction* file);
         void           updateViewMenu ();
         void           updateWindowMenu ();
         void           editorChanged ();
         void           cursorPositionChanged (int, int);
         void           selectionChanged ();
         void           selectedCount ();
         void           textChanged ();
         void           contextMenu (QPoint pos);
         void           setTranscriptVisible (bool state);

      private:
         void           setup ();


      private:
         Ui::TextEdit*  m_ui;
         QLabel*        m_position_label;
         QLabel*        m_filename_label;
         QLabel*        m_language_label;
         QLabel*        m_selected_count_label;
         QMenu*         m_context_menu;
   };
}
