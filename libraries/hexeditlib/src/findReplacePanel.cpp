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

#include "findReplacePanel.h"
#include "ui_findReplacePanel.h"


findReplacePanel::findReplacePanel (QWidget* parent) : QWidget(parent) {setup();}
findReplacePanel::~findReplacePanel () {delete _ui;}
//-----------------------------------------------------------------------------
void findReplacePanel::setFindText (QString text)
{
   _ui->_findEntry->setText(text);
   _ui->_findEntry->selectAll();
}
void findReplacePanel::setFocus () {_ui->_findEntry->setFocus();}
//-----------------------------------------------------------------------------
void findReplacePanel::findNext ()
{
   QString text = _ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit find(text, (_ui->_findFormat->currentIndex() == 0), false);
}
void findReplacePanel::findPrevious ()
{
   QString text = _ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit find(text, (_ui->_findFormat->currentIndex() == 0), true);
}
void findReplacePanel::replaceNext ()
{
   QString ftext = _ui->_findEntry->text();
   if (ftext.isEmpty()) return;

   QString rtext = _ui->_replaceEntry->text();
   if (rtext.isEmpty()) return;

   emit replace(ftext, (_ui->_findFormat->currentIndex() == 0), rtext, (_ui->_replaceFormat->currentIndex() == 0), false);
}
void findReplacePanel::replaceFindNext ()
{
   replaceNext();
   findNext();
}
void findReplacePanel::replaceAll ()
{
   QString ftext = _ui->_findEntry->text();
   if (ftext.isEmpty()) return;

   QString rtext = _ui->_replaceEntry->text();
   if (rtext.isEmpty()) return;

   emit replaceAll(ftext, (_ui->_findFormat->currentIndex() == 0), rtext, (_ui->_replaceFormat->currentIndex() == 0));
}
//-----------------------------------------------------------------------------
// setup
void findReplacePanel::setup ()
{
   _ui = new Ui::findReplacePanel;
   _ui->setupUi(this);

   connect(_ui->_findEntry,         SIGNAL(returnPressed()),   this,    SLOT(findNext()));
   connect(_ui->_replaceEntry,      SIGNAL(returnPressed()),   this,    SLOT(replaceNext()));
   connect(_ui->_nextButton,        SIGNAL(clicked()),         this,    SLOT(findNext()));
   connect(_ui->_previousButton,    SIGNAL(clicked()),         this,    SLOT(findPrevious()));
   connect(_ui->_replaceButton,     SIGNAL(clicked()),         this,    SLOT(replaceNext()));
   connect(_ui->_replaceFindButton, SIGNAL(clicked()),         this,    SLOT(replaceFindNext()));
   connect(_ui->_replaceAllButton,  SIGNAL(clicked()),         this,    SLOT(replaceAll()));
   connect(_ui->_closeButton,       SIGNAL(clicked()),          this,    SLOT(hide()));
}
