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

#include <QDialog>
#include <QSettings>

namespace Ui {class PreferencesDialog;}

class PreferencesDialog : public QDialog
{
   Q_OBJECT

   public:
      explicit                PreferencesDialog (QWidget* parent = nullptr);
                              ~PreferencesDialog ();

   public Q_SLOTS:
      virtual int             exec () override;

   protected Q_SLOTS:
      void                    defaults();

   protected:
      void                    load ();
      void                    save ();

   private:
      Ui::PreferencesDialog*  m_ui;

};

