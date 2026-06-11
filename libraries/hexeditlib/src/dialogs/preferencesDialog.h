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

namespace hexeditLib
{
   namespace Ui {class preferencesDialog;}

   class preferencesDialog : public QDialog
   {
      Q_OBJECT

      private:
         Ui::preferencesDialog*     _ui;

      public:
                                    preferencesDialog (QWidget* parent = nullptr);
                                    ~preferencesDialog () override;

      public slots:
         void                       defaultPreferences ();
         void                       loadPreferences ();
         void                       savePreferences ();

      private:
         void                       setup ();
   };
}
