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
// $Id: XTreeWidget.cpp 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "tree_widget.h"
#include <QMouseEvent>

using namespace commonLib;

TreeWidget::TreeWidget (QWidget* parent) : QTreeWidget(parent) {}

// custom currentItem that returns the first selected item
QTreeWidgetItem* TreeWidget::currentItem ()
{
   QList<QTreeWidgetItem*> items = selectedItems();
   if (items.isEmpty()) return nullptr;        // nothing selected

   return items.at(0);
}

void TreeWidget::mousePressEvent (QMouseEvent* e)
{
   if (itemAt(e->pos()) == nullptr)
   {
      clearSelection();
      emit blankClicked();
   }
   QTreeWidget::mousePressEvent(e);
}

