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

#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include "editor_settings_p.h"

using namespace texteditLib;

void PreferencesDialog::defaultStyle ()
{
   if (m_lexer == nullptr) return;

   for (int style = 0; style < 128; style++)
   {
      if (m_lexer->description(style).isEmpty()) continue;

      m_lexer->setFont(m_lexer->defaultFont(style), style);
      m_lexer->setColor(m_lexer->defaultColor(style), style);
      m_lexer->setPaper(m_lexer->defaultPaper(style), style);
   }

   styleSelected(m_ui->_styleList->currentItem());
}
void PreferencesDialog::loadStyles ()
{
   clearStyles();
   m_lexers = LexerManager::lexers();

   foreach (QString lang, m_lexers.keys())
      m_ui->_langList->addItem(new QListWidgetItem(lang));
}
void PreferencesDialog::saveStyles ()
{
   QSettings settings;
   settings.beginGroup(STYLE_SETTINGS);

   foreach (QsciLexer* lexer, m_lexers)
      lexer->writeSettings(settings);
}
void PreferencesDialog::clearStyles ()
{
   m_ui->_langList->clear();
   m_ui->_langStyleLabel->clear();
   m_ui->_defExtEntry->clear();
   m_ui->_userExtEntry->clear();
   clearStyle();
}
void PreferencesDialog::clearStyle ()
{
   m_ui->_styleList->clear();
   m_ui->_familyCombo->setCurrentIndex(0);
   m_ui->_sizeSpin->setValue(0);
   m_ui->_boldCheck->setChecked(false);
   m_ui->_italicCheck->setChecked(false);
   m_ui->_underCheck->setChecked(false);
   m_ui->_overCheck->setChecked(false);
   m_ui->_strikeCheck->setChecked(false);
   m_ui->_styleFgButton->clear();
   m_ui->_styleBgButton->clear();

   m_ui->_styleGroup->setEnabled(false);
}
//-----------------------------------------------------------------------------
void PreferencesDialog::langSelected (QListWidgetItem* item)
{
   clearStyle();
   m_lang = item->text();

   m_lexer = m_lexers.value(m_lang);

   // update file extensions
//   m_ui->_defExtEntry->setText(m_lexer->extensions().join(' '));
//   m_ui->_userExtEntry->setText(m_lexer->userExtensions().join(' '));

   // update styles
   for (int style = 0; style < 128; style++)
   {
      QString desc = m_lexer->description(style);
      if (desc.isEmpty()) continue;

      QListWidgetItem* item = new QListWidgetItem(desc);
      item->setData(Qt::UserRole, style);
      m_ui->_styleList->addItem(item);
   }

   m_ui->_langStyleLabel->setText(m_lang);
   m_ui->_styleDefaultsButton->setEnabled(true);
}
void PreferencesDialog::styleSelected (QListWidgetItem* item)
{
   if (item == nullptr) return;

   m_style = item->data(Qt::UserRole).toInt();

   QFont font = m_lexer->font(m_style);
   m_ui->_familyCombo->setCurrentText(font.family());
   m_ui->_sizeSpin->setValue(font.pointSize());
   m_ui->_boldCheck->setChecked(font.bold());
   m_ui->_italicCheck->setChecked(font.italic());
   m_ui->_underCheck->setChecked(font.underline());
   m_ui->_overCheck->setChecked(font.overline());
   m_ui->_strikeCheck->setChecked(font.strikeOut());

   m_ui->_styleFgButton->setCurrentColor(m_lexer->color(m_style));
   m_ui->_styleBgButton->setCurrentColor(m_lexer->paper(m_style));

   m_ui->_langStyleLabel->setText(m_lang + " : " + item->text());

   m_ui->_styleGroup->setEnabled(true);
}
void PreferencesDialog::extChanged (QString text)
{
//   if (m_lexer)
//      m_lexer->setUserExtensions(m_ui->_userExtEntry->text().toLower().split(QRegularExpression("\\s+")));
}
void PreferencesDialog::familyChanged (QString family)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setFamily(family);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::sizeChanged (int val)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setPointSize(val);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::boldChanged (bool state)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setBold(state);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::italicChanged (bool state)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setItalic(state);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::underChanged (bool state)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setUnderline(state);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::overChanged (bool state)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setOverline(state);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::strikeChanged (bool state)
{
   if (m_lexer) {
      QFont font = m_lexer->font(m_style);
      font.setStrikeOut(state);
      m_lexer->setFont(font, m_style);
   }
}
void PreferencesDialog::foregroundChanged (QColor color)
{
   if (m_lexer)
      m_lexer->setColor(color, m_style);
}
void PreferencesDialog::backgroundChanged (QColor color)
{
   if (m_lexer)
      m_lexer->setPaper(color, m_style);
}
//-----------------------------------------------------------------------------
void PreferencesDialog::setupStyles ()
{
   m_ui->_familyCombo->insertItem(0, QString());

   connect(m_ui->_userExtEntry,         SIGNAL(textChanged(QString)),          this,    SLOT(extChanged(QString)));
   connect(m_ui->_langList,             SIGNAL(itemClicked(QListWidgetItem*)), this,    SLOT(langSelected(QListWidgetItem*)));
   connect(m_ui->_styleList,            SIGNAL(itemClicked(QListWidgetItem*)), this,    SLOT(styleSelected(QListWidgetItem*)));
   connect(m_ui->_familyCombo,          SIGNAL(currentTextChanged(QString)),   this,    SLOT(familyChanged(QString)));
   connect(m_ui->_sizeSpin,             SIGNAL(valueChanged(int)),             this,    SLOT(sizeChanged(int)));
   connect(m_ui->_boldCheck,            SIGNAL(clicked(bool)),                 this,    SLOT(boldChanged(bool)));
   connect(m_ui->_italicCheck,          SIGNAL(clicked(bool)),                 this,    SLOT(italicChanged(bool)));
   connect(m_ui->_underCheck,           SIGNAL(clicked(bool)),                 this,    SLOT(underChanged(bool)));
   connect(m_ui->_overCheck,            SIGNAL(clicked(bool)),                 this,    SLOT(overChanged(bool)));
   connect(m_ui->_strikeCheck,          SIGNAL(clicked(bool)),                 this,    SLOT(strikeChanged(bool)));
   connect(m_ui->_styleFgButton,        SIGNAL(colorChanged(QColor)),          this,    SLOT(foregroundChanged(QColor)));
   connect(m_ui->_styleBgButton,        SIGNAL(colorChanged(QColor)),          this,    SLOT(backgroundChanged(QColor)));
   connect(m_ui->_styleDefaultsButton,  SIGNAL(clicked()),                     this,    SLOT(defaultStyle()));

   m_ui->_styleDefaultsButton->setEnabled(false);

   loadStyles();
}
