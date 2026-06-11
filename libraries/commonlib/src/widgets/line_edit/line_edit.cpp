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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: XLineEdit.cpp 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "line_edit.h"
#include <QToolButton>
#include <QStyle>
#include <QKeyEvent>

using namespace commonLib;

LineEdit::LineEdit(QWidget* parent)
   : QLineEdit(parent)
{
   m_row = -1;
   m_col = -1;
   m_editing = false;
   m_editButton = NULL;
   m_clearButton = new QToolButton(this);
   m_clearButton->setIcon(QIcon(":xcommon/close2"));
   m_clearButton->setIconSize(QSize(12,12));
   m_clearButton->setCursor(Qt::ArrowCursor);
   m_clearButton->setStyleSheet("QToolButton {color: rgb(0, 0, 0); background-color: rgb(255, 255, 255); border: none; padding: 0px; }");
   m_clearButton->hide();
   connect(m_clearButton, SIGNAL(clicked()), this, SLOT(clear()));
//   connect(this, SIGNAL(returnPressed()), this, SIGNAL(doneEditing()));
   connect(this, SIGNAL(returnPressed()), this, SLOT(processReturn()));
   connect(this, SIGNAL(selectionChanged()), this, SLOT(textSelectionChanged()));

   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
   QSize msz = minimumSizeHint();
   setMinimumSize(qMax(msz.width(), m_clearButton->sizeHint().width() + frameWidth * 2 + 100),
                  qMax(msz.height(), m_clearButton->sizeHint().height() + frameWidth * 2 + 2));
}
//-----------------------------------------------------------------------------
void LineEdit::addEditButton ()
{
   m_editButton = new QToolButton(this);
   m_editButton->setIcon(QIcon(":xcommon/fileEdit"));
   m_editButton->setIconSize(QSize(12,12));
   m_editButton->setCursor(Qt::ArrowCursor);
   m_editButton->setStyleSheet("QToolButton {color: rgb(0, 0, 0); background-color: rgb(255, 255, 255); border: none; padding: 0px; }");
   m_editButton->hide();
   connect(m_editButton, SIGNAL(clicked()), this, SLOT(editClicked()));

   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
//   setStyleSheet(QString("QLineEdit {background-color: rgb(255, 255, 255); padding-right: %1px; } ").arg(_clearButton->sizeHint().width() + m_editButton->sizeHint().width() + frameWidth + 1));
   QSize msz = minimumSizeHint();
   setMinimumSize(qMax(msz.width(), m_editButton->sizeHint().width() + frameWidth * 2 + 100),
                  qMax(msz.height(), m_editButton->sizeHint().height() + frameWidth * 2 + 2));
}
void LineEdit::setLocation (int row, int col)
{
   m_row = row;
   m_col = col;
}
quint32 LineEdit::row () {return m_row;}
quint32 LineEdit::col () {return m_col;}

//-----------------------------------------------------------------------------
void LineEdit::startEdit ()
{
   setFocus();
   selectAll();
}
//-----------------------------------------------------------------------------
void LineEdit::unselect () {/* QLineEdit::deselect(); */ }
void LineEdit::remove (){QLineEdit::clear();}

//-----------------------------------------------------------------------------
// resizeEvent
void LineEdit::resizeEvent (QResizeEvent* e)
{
   Q_UNUSED(e)

   QSize clearSize = m_clearButton->sizeHint();
   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
   m_clearButton->move(rect().right() - frameWidth - clearSize.width(),
                     (rect().bottom() + 1 - clearSize.height())/2);

   if (m_editButton != nullptr) {
      QSize editSize = m_editButton->sizeHint();
      if (m_editButton != nullptr)
         m_editButton->move(rect().right() - frameWidth - editSize.width() - clearSize.width(),
                          (rect().bottom() + 1 - editSize.height())/2);
   }
}
void LineEdit::focusInEvent (QFocusEvent* e)
{
   QLineEdit::focusInEvent(e);
   showButtons();
}
void LineEdit::focusOutEvent (QFocusEvent* e)
{
   hideButtons();
   QLineEdit::focusOutEvent(e);
}
void LineEdit::keyPressEvent (QKeyEvent* e)
{
   if (e->key() == Qt::Key_Escape) {
      hideButtons();
      parentWidget()->setFocus();
   }
   else
      QLineEdit::keyPressEvent(e);
}

//-----------------------------------------------------------------------------
void LineEdit::editClicked () {emit editClicked(this);}
void LineEdit::showButtons ()
{
   m_clearButton->setVisible(true);
   if (m_editButton != nullptr)
      m_editButton->setVisible(true);
   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
   setStyleSheet(QString("QLineEdit {color: rgb(255, 0, 0); padding-right: %1px; } ").arg(m_clearButton->sizeHint().width() + ((m_editButton != NULL) ? m_editButton->sizeHint().width() : 0) + frameWidth + 1));
   m_editing = true;
}
void LineEdit::hideButtons ()
{
   bool editing = m_editing;      // get local copy of m_editing
   m_editing = false;
   m_clearButton->setVisible(false);
   if (m_editButton != nullptr)
      m_editButton->setVisible(false);
   setStyleSheet(QString("QLineEdit {color: rgb(0, 0, 0); padding-right: %1px; } ").arg(1));
   if (editing)
      emit doneEditing(this);
}
void LineEdit::processReturn ()
{
   hideButtons();
   parentWidget()->setFocus();
}
void LineEdit::textSelectionChanged ()
{
   if (hasSelectedText())
      emit hasSelection(this);
}

