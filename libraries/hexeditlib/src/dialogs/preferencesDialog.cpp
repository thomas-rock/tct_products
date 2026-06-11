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
// $Id: xTextEditPreferences.cpp 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
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

using namespace hexeditLib;

preferencesDialog::preferencesDialog (QWidget* parent) : QDialog(parent) {setup();}
preferencesDialog::~preferencesDialog () {delete _ui;}
//-----------------------------------------------------------------------------
void preferencesDialog::defaultPreferences ()
{
   _ui->overwriteCheck->setChecked(OVERWRITE_MODE_DEFAULT);    // true = tabs, false = spaces
   _ui->highlightCheck->setChecked(HIGHLIGHT_DEFAULT);
   _ui->readonlyCheck->setChecked(READ_ONLY_DEFAULT);
   _ui->showAddressCheck->setChecked(SHOW_ADDRESS_DEFAULT);
   _ui->showAsciiCheck->setChecked(SHOW_ASCII_DEFAULT);
   _ui->hexCapsCheck->setChecked(HEX_CAPS_DEFAULT);

   _ui->addressSpin->setValue(ADDRESS_WIDTH_DEFAULT);
   _ui->bytesSpin->setValue(BYTES_DEFAULT);

   _ui->addressTextButton->setCurrentColor(ADDRESS_TEXT_COLOR_DEFAULT);
   _ui->asciiTextButton->setCurrentColor(ASCII_TEXT_COLOR_DEFAULT);
   _ui->hexTextButton->setCurrentColor(HEX_TEXT_COLOR_DEFAULT);
   _ui->highlightButton->setCurrentColor(HIGHLIGHT_COLOR_DEFAULT);
   _ui->addressBgButton->setCurrentColor(ADDRESS_BG_COLOR_DEFAULT);
   _ui->asciiBgButton->setCurrentColor(ASCII_BG_COLOR_DEFAULT);
   _ui->selectionButton->setCurrentColor(SELECTION_COLOR_DEFAULT);
}
void preferencesDialog::loadPreferences ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   _ui->overwriteCheck->setChecked(settings.value(OVERWRITE_MODE, OVERWRITE_MODE_DEFAULT).toBool());
   _ui->highlightCheck->setChecked(settings.value(HIGHLIGHT, HIGHLIGHT_DEFAULT).toBool());
   _ui->readonlyCheck->setChecked(settings.value(READ_ONLY, READ_ONLY_DEFAULT).toBool());
   _ui->showAddressCheck->setChecked(settings.value(SHOW_ADDRESS, SHOW_ADDRESS_DEFAULT).toBool());
   _ui->showAsciiCheck->setChecked(settings.value(SHOW_ASCII, SHOW_ASCII_DEFAULT).toBool());
   _ui->hexCapsCheck->setChecked(settings.value(HEX_CAPS, HEX_CAPS_DEFAULT).toBool());

   _ui->addressSpin->setValue(settings.value(ADDRESS_WIDTH, ADDRESS_WIDTH_DEFAULT).toInt());
   _ui->bytesSpin->setValue(settings.value(BYTES, BYTES_DEFAULT).toInt());

   _ui->addressTextButton->setCurrentColor(settings.value(ADDRESS_TEXT_COLOR, ADDRESS_TEXT_COLOR_DEFAULT).toString());
   _ui->asciiTextButton->setCurrentColor(settings.value(ASCII_TEXT_COLOR, ASCII_TEXT_COLOR_DEFAULT).toString());
   _ui->hexTextButton->setCurrentColor(settings.value(HEX_TEXT_COLOR, HEX_TEXT_COLOR_DEFAULT).toString());
   _ui->highlightButton->setCurrentColor(settings.value(HIGHLIGHT_COLOR, HIGHLIGHT_COLOR_DEFAULT).toString());
   _ui->addressBgButton->setCurrentColor(settings.value(ADDRESS_BG_COLOR, ADDRESS_BG_COLOR_DEFAULT).toString());
   _ui->asciiBgButton->setCurrentColor(settings.value(ASCII_BG_COLOR, ASCII_BG_COLOR_DEFAULT).toString());
   _ui->selectionButton->setCurrentColor(settings.value(SELECTION_COLOR, SELECTION_COLOR_DEFAULT).toString());
}
void preferencesDialog::savePreferences ()
{
   QSettings settings;
   settings.beginGroup(PREFERENCE_SETTINGS);

   settings.setValue(OVERWRITE_MODE,      _ui->overwriteCheck->isChecked());
   settings.setValue(HIGHLIGHT,           _ui->highlightCheck->isChecked());
   settings.setValue(READ_ONLY,           _ui->readonlyCheck->isChecked());
   settings.setValue(SHOW_ADDRESS,        _ui->showAddressCheck->isChecked());
   settings.setValue(SHOW_ASCII,          _ui->showAsciiCheck->isChecked());
   settings.setValue(HEX_CAPS,            _ui->hexCapsCheck->isChecked());

   settings.setValue(ADDRESS_WIDTH,       _ui->addressSpin->value());
   settings.setValue(BYTES,               _ui->bytesSpin->value());

   settings.setValue(ADDRESS_TEXT_COLOR,  _ui->addressTextButton->currentColorName());
   settings.setValue(ASCII_TEXT_COLOR,    _ui->asciiTextButton->currentColorName());
   settings.setValue(HEX_TEXT_COLOR,      _ui->hexTextButton->currentColorName());
   settings.setValue(HIGHLIGHT_COLOR,     _ui->highlightButton->currentColorName());
   settings.setValue(ADDRESS_BG_COLOR,    _ui->addressBgButton->currentColorName());
   settings.setValue(ASCII_BG_COLOR,      _ui->asciiBgButton->currentColorName());
   settings.setValue(SELECTION_COLOR,     _ui->selectionButton->currentColorName());
}
//-----------------------------------------------------------------------------
void preferencesDialog::setup ()
{
   _ui = new Ui::preferencesDialog;
   _ui->setupUi(this);

   _ui->highlightButton->setAlpha(true);
   connect(_ui->defaultsButton,     SIGNAL(clicked()),    this,    SLOT(defaultPreferences()));

   loadPreferences();
}
