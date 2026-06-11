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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1255 $
//
// $Date: 2016-05-27 08:50:47 -0400 (Fri, 27 May 2016) $
//=============================================================================
// $Id: xTextEditPreferences.h 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#pragma once

#include <QtWidgets>
#include "lexerManager.h"

namespace texteditLib
{
   namespace Ui {class preferencesDialog;}

   class preferencesDialog : public QDialog
   {
      Q_OBJECT

      private:
         Ui::preferencesDialog*     _ui;

         lexerMap                   _lexers;

         // style variables
         QString                    _lang;
         int                        _style;
         QsciLexer*                 _lexer;

      public:
                                    preferencesDialog (QWidget* parent = nullptr);
                                    ~preferencesDialog () override;

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
   };
}
