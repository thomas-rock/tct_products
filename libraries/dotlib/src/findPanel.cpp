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
// Last checked in by $Author:: thomas@x-tekcorp.com      $   $Revision: 1236 $
//
// $Date: 2016-05-24 19:21:29 -0400 (Tue, 24 May 2016) $
//=============================================================================
// $Id: xFindReplacePanel.cpp 1236 2016-05-24 23:21:29Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "findPanel.h"
#include "ui_findPanel.h"

#include <QWidgetAction>
#include <QCheckBox>

using namespace dotLib;

findPanel::findPanel (QWidget* parent) : QWidget(parent) {setup();}
findPanel::~findPanel () {delete _ui;}
//-----------------------------------------------------------------------------
void findPanel::setViewAction (QAction* action)
{
   _viewAction = action;
   setVisible(_viewAction->isChecked());
   connect(_viewAction, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
}
void findPanel::setFocus () {_ui->_nameEntry->setFocus();}
//-----------------------------------------------------------------------------
void findPanel::hide ()
{
   QWidget::hide();
   _viewAction->setChecked(false);
}
void findPanel::propChanged (QString text)
{
   bool state = !text.isEmpty();
   _ui->_valueEntry->setEnabled(state);
   _ui->_valueCaseCheck->setEnabled(state);
   _ui->_valueRegexCheck->setEnabled(state);
}
void findPanel::findNext ()
{
   emit findNext (_ui->_nameEntry->text(),      _ui->_nameCaseCheck->isChecked(),      _ui->_nameRegexCheck->isChecked(),
                  _ui->_textEntry->text(),      _ui->_textCaseCheck->isChecked(),      _ui->_textRegexCheck->isChecked(),
                  _ui->_propertyEntry->text(),
                  _ui->_valueEntry->text(),     _ui->_valueCaseCheck->isChecked(),     _ui->_valueRegexCheck->isChecked());
}
void findPanel::findAll ()
{
   emit findAll (_ui->_nameEntry->text(),       _ui->_nameCaseCheck->isChecked(),      _ui->_nameRegexCheck->isChecked(),
                 _ui->_textEntry->text(),       _ui->_textCaseCheck->isChecked(),      _ui->_textRegexCheck->isChecked(),
                 _ui->_propertyEntry->text(),
                 _ui->_valueEntry->text(),      _ui->_valueCaseCheck->isChecked(),     _ui->_valueRegexCheck->isChecked());
}
//-----------------------------------------------------------------------------
void findPanel::setup ()
{
   _ui = new Ui::findPanel;
   _ui->setupUi(this);

   _ui->_valueEntry->setEnabled(false);
   _ui->_valueCaseCheck->setEnabled(false);
   _ui->_valueRegexCheck->setEnabled(false);

   connect(_ui->_nameEntry,      SIGNAL(returnPressed()),   this, SLOT(findNext()));
   connect(_ui->_propertyEntry,  SIGNAL(textChanged(QString)), this, SLOT(propChanged(QString)));
   connect(_ui->_nextButton,     SIGNAL(clicked()),         this, SLOT(findNext()));
   connect(_ui->_allButton,      SIGNAL(clicked()),         this, SLOT(findAll()));
   connect(_ui->_closeButton,    SIGNAL(clicked()),         this, SLOT(hide()));
}
