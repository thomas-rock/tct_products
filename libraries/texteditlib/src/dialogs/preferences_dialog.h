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

#include <QtWidgets>
#include "lexer_manager.h"

namespace texteditLib
{
   namespace Ui {class PreferencesDialog;}

   class PreferencesDialog : public QDialog
   {
      Q_OBJECT

      public:
                                    PreferencesDialog (QWidget* parent = nullptr);
                                    ~PreferencesDialog () override;

      public slots:
         void                       loadPreferences ();
         void                       savePreferences ();

      protected slots:
         // general preferences
         void                       defaultGeneral ();
         void                       loadGeneral ();
         void                       saveGeneral ();

         // style preferences
         void                       defaultStyle ();
         void                       loadStyles ();
         void                       saveStyles ();
         void                       clearStyles ();
         void                       clearStyle ();

      protected slots:
         // general preferences

         // style preferences
         void                       langSelected (QListWidgetItem* item);
         void                       styleSelected (QListWidgetItem* item);
         void                       extChanged (QString ext);
         void                       familyChanged (QString family);
         void                       sizeChanged (int val);
         void                       boldChanged (bool state);
         void                       italicChanged (bool state);
         void                       underChanged (bool state);
         void                       overChanged (bool state);
         void                       strikeChanged (bool state);
         void                       foregroundChanged (QColor color);
         void                       backgroundChanged (QColor color);

      private:
         void                       setup ();
         void                       setupGeneral ();
         void                       setupStyles ();


      private:
         Ui::PreferencesDialog*     m_ui;

         lexerMap                   m_lexers;

         // style variables
         QString                    m_lang;
         int                        m_style;
         QsciLexer*                 m_lexer;

   };
}
