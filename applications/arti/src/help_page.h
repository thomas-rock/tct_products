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

#include <QMainWindow>
#include <QSettings>

namespace Ui {class HelpPage;}

class HelpPage : public QMainWindow
{
   Q_OBJECT

   public:
      explicit          HelpPage(QWidget* parent = nullptr);
                        ~HelpPage();

      void              loadState (QSettings& settings);
      void              saveState (QSettings& settings);

   private:
      Ui::HelpPage*     m_ui;
};

