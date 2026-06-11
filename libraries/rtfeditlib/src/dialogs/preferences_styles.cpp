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
// $Id: xTextEditPreferences_styles.cpp 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "preferencesDialog.h"
#include "ui_preferencesDialog.h"
#include "editorSettings_p.h"

using namespace texteditLib;

void preferencesDialog::defaultStyle ()
{
   if (_lexer == nullptr) return;

   for (int style = 0; style < 128; style++)
   {
      if (_lexer->description(style).isEmpty()) continue;

      _lexer->setFont(_lexer->defaultFont(style), style);
      _lexer->setColor(_lexer->defaultColor(style), style);
      _lexer->setPaper(_lexer->defaultPaper(style), style);
   }

   styleSelected(_ui->_styleList->currentItem());
}
void preferencesDialog::loadStyles ()
{
   clearStyles();
   _lexers = lexerManager::lexers();

   foreach (QString lang, _lexers.keys())
      _ui->_langList->addItem(new QListWidgetItem(lang));
}
void preferencesDialog::saveStyles ()
{
   QSettings settings;
   settings.beginGroup(STYLE_SETTINGS);

   foreach (QsciLexer* lexer, _lexers)
      lexer->writeSettings(settings);
}
void preferencesDialog::clearStyles ()
{
   _ui->_langList->clear();
   _ui->_langStyleLabel->clear();
   _ui->_defExtEntry->clear();
   _ui->_userExtEntry->clear();
   clearStyle();
}
void preferencesDialog::clearStyle ()
{
   _ui->_styleList->clear();
   _ui->_familyCombo->setCurrentIndex(0);
   _ui->_sizeSpin->setValue(0);
   _ui->_boldCheck->setChecked(false);
   _ui->_italicCheck->setChecked(false);
   _ui->_underCheck->setChecked(false);
   _ui->_overCheck->setChecked(false);
   _ui->_strikeCheck->setChecked(false);
   _ui->_styleFgButton->clear();
   _ui->_styleBgButton->clear();

   _ui->_styleGroup->setEnabled(false);
}
//-----------------------------------------------------------------------------
void preferencesDialog::langSelected (QListWidgetItem* item)
{
   clearStyle();
   _lang = item->text();

   _lexer = _lexers.value(_lang);

   // update file extensions
   _ui->_defExtEntry->setText(_lexer->extensions().join(' '));
   _ui->_userExtEntry->setText(_lexer->userExtensions().join(' '));

   // update styles
   for (int style = 0; style < 128; style++)
   {
      QString desc = _lexer->description(style);
      if (desc.isEmpty()) continue;

      QListWidgetItem* item = new QListWidgetItem(desc);
      item->setData(Qt::UserRole, style);
      _ui->_styleList->addItem(item);
   }

   _ui->_langStyleLabel->setText(_lang);
   _ui->_styleDefaultsButton->setEnabled(true);
}
void preferencesDialog::styleSelected (QListWidgetItem* item)
{
   if (item == nullptr) return;

   _style = item->data(Qt::UserRole).toInt();

   QFont font = _lexer->font(_style);
   _ui->_familyCombo->setCurrentText(font.family());
   _ui->_sizeSpin->setValue(font.pointSize());
   _ui->_boldCheck->setChecked(font.bold());
   _ui->_italicCheck->setChecked(font.italic());
   _ui->_underCheck->setChecked(font.underline());
   _ui->_overCheck->setChecked(font.overline());
   _ui->_strikeCheck->setChecked(font.strikeOut());

   _ui->_styleFgButton->setCurrentColor(_lexer->color(_style));
   _ui->_styleBgButton->setCurrentColor(_lexer->paper(_style));

   _ui->_langStyleLabel->setText(_lang + " : " + item->text());

   _ui->_styleGroup->setEnabled(true);
}
void preferencesDialog::extChanged (QString text)
{
   if (_lexer)
      _lexer->setUserExtensions(_ui->_userExtEntry->text().toLower().split(QRegularExpression("\\s+")));
}
void preferencesDialog::familyChanged (QString family)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setFamily(family);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::sizeChanged (int val)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setPointSize(val);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::boldChanged (bool state)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setBold(state);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::italicChanged (bool state)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setItalic(state);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::underChanged (bool state)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setUnderline(state);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::overChanged (bool state)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setOverline(state);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::strikeChanged (bool state)
{
   if (_lexer) {
      QFont font = _lexer->font(_style);
      font.setStrikeOut(state);
      _lexer->setFont(font, _style);
   }
}
void preferencesDialog::foregroundChanged (QColor color)
{
   if (_lexer)
      _lexer->setColor(color, _style);
}
void preferencesDialog::backgroundChanged (QColor color)
{
   if (_lexer)
      _lexer->setPaper(color, _style);
}
//-----------------------------------------------------------------------------
void preferencesDialog::setupStyles ()
{
   _ui->_familyCombo->insertItem(0, QString());

   connect(_ui->_userExtEntry,         SIGNAL(textChanged(QString)),          this,    SLOT(extChanged(QString)));
   connect(_ui->_langList,             SIGNAL(itemClicked(QListWidgetItem*)), this,    SLOT(langSelected(QListWidgetItem*)));
   connect(_ui->_styleList,            SIGNAL(itemClicked(QListWidgetItem*)), this,    SLOT(styleSelected(QListWidgetItem*)));
   connect(_ui->_familyCombo,          SIGNAL(currentTextChanged(QString)),   this,    SLOT(familyChanged(QString)));
   connect(_ui->_sizeSpin,             SIGNAL(valueChanged(int)),             this,    SLOT(sizeChanged(int)));
   connect(_ui->_boldCheck,            SIGNAL(clicked(bool)),                 this,    SLOT(boldChanged(bool)));
   connect(_ui->_italicCheck,          SIGNAL(clicked(bool)),                 this,    SLOT(italicChanged(bool)));
   connect(_ui->_underCheck,           SIGNAL(clicked(bool)),                 this,    SLOT(underChanged(bool)));
   connect(_ui->_overCheck,            SIGNAL(clicked(bool)),                 this,    SLOT(overChanged(bool)));
   connect(_ui->_strikeCheck,          SIGNAL(clicked(bool)),                 this,    SLOT(strikeChanged(bool)));
   connect(_ui->_styleFgButton,        SIGNAL(colorChanged(QColor)),          this,    SLOT(foregroundChanged(QColor)));
   connect(_ui->_styleBgButton,        SIGNAL(colorChanged(QColor)),          this,    SLOT(backgroundChanged(QColor)));
   connect(_ui->_styleDefaultsButton,  SIGNAL(clicked()),                     this,    SLOT(defaultStyle()));

   _ui->_styleDefaultsButton->setEnabled(false);

   loadStyles();
}
