//=============================================================================
// RockSolid Logic Corporation                         www.RockSolidLogic.com  
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, RockSolid Solutions
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

#include "find_replace_panel.h"
#include "ui_find_replace_panel.h"

#include <QWidgetAction>
#include <QCheckBox>

using namespace commonLib;

FindReplacePanel::FindReplacePanel (QWidget* parent) : QWidget(parent) {setup();}
FindReplacePanel::~FindReplacePanel () {delete m_ui;}
//-----------------------------------------------------------------------------
void FindReplacePanel::setFindText (QString text)
{
   m_ui->_findEntry->setText(text);
   m_ui->_findEntry->selectAll();
}
void FindReplacePanel::setFocus () {m_ui->_findEntry->setFocus();}
void FindReplacePanel::replacedCount (int count) {m_ui->_countLabel->setText(QString("  %1 occurrences were replaced").arg(count));}
void FindReplacePanel::findNext ()
{
   m_ui->_countLabel->clear();
   QString text = m_ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit find(text, true, m_ui->_caseCheck->isChecked(), m_ui->_regexCheck->isChecked(), m_ui->_wholeWordsCheck->isChecked(), true);
}
void FindReplacePanel::findPrevious ()
{
   m_ui->_countLabel->clear();
   QString text = m_ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit find(text, false, m_ui->_caseCheck->isChecked(), m_ui->_regexCheck->isChecked(), m_ui->_wholeWordsCheck->isChecked(), true);
}
void FindReplacePanel::replaceNext ()
{
   m_ui->_countLabel->clear();
   QString text = m_ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit replace(text, m_ui->_replaceEntry->text(), false, false, true, m_ui->_caseCheck->isChecked(), m_ui->_regexCheck->isChecked(), m_ui->_wholeWordsCheck->isChecked(), true);
}
void FindReplacePanel::replaceFindNext ()
{
   m_ui->_countLabel->clear();
   QString text = m_ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit replace(text, m_ui->_replaceEntry->text(), false, true, true, m_ui->_caseCheck->isChecked(), m_ui->_regexCheck->isChecked(), m_ui->_wholeWordsCheck->isChecked(), true);
}
void FindReplacePanel::replaceAll ()
{
   m_ui->_countLabel->clear();
   QString text = m_ui->_findEntry->text();
   if (text.isEmpty()) return;

   emit replace(text, m_ui->_replaceEntry->text(), true, false, true, m_ui->_caseCheck->isChecked(), m_ui->_regexCheck->isChecked(), m_ui->_wholeWordsCheck->isChecked(), true);
}
//-----------------------------------------------------------------------------
void FindReplacePanel::setup ()
{
   m_ui = new Ui::FindReplacePanel;
   m_ui->setupUi(this);

   connect(m_ui->_findEntry,         SIGNAL(returnPressed()),   this, SLOT(findNext()));
   connect(m_ui->_replaceEntry,      SIGNAL(returnPressed()),   this, SLOT(replaceNext()));
   connect(m_ui->_nextButton,        SIGNAL(clicked()),         this, SLOT(findNext()));
   connect(m_ui->_previousButton,    SIGNAL(clicked()),         this, SLOT(findPrevious()));
   connect(m_ui->_replaceButton,     SIGNAL(clicked()),         this, SLOT(replaceNext()));
   connect(m_ui->_replaceFindButton, SIGNAL(clicked()),         this, SLOT(replaceFindNext()));
   connect(m_ui->_replaceAllButton, SIGNAL(clicked()),          this, SLOT(replaceAll()));
   connect(m_ui->_closeButton,       SIGNAL(clicked()),         this, SLOT(hide()));
}
