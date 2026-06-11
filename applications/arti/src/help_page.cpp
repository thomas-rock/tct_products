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
#include "help_page.h"
#include "ui_help_page.h"
#include "settings_p.h"

HelpPage::HelpPage(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::HelpPage) {m_ui->setupUi(this);}
HelpPage::~HelpPage() {delete m_ui;}
//-----------------------------------------------------------------------------
void HelpPage::loadState (QSettings& settings) {}
void HelpPage::saveState (QSettings& settings) {}
