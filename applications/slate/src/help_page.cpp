#include "help_page.h"
#include "ui_help_page.h"
#include "settings_p.h"

HelpPage::HelpPage(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::HelpPage) {m_ui->setupUi(this);}
HelpPage::~HelpPage() {delete m_ui;}
//-----------------------------------------------------------------------------
void HelpPage::loadState (QSettings& settings) {}
void HelpPage::saveState (QSettings& settings) {}
