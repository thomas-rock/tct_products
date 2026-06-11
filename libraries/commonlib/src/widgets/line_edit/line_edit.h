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
// $Id: XLineEdit.h 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#pragma once

#include <QLineEdit>
#include "commonlib_global.h"

class QToolButton;

QT_BEGIN_NAMESPACE
namespace commonLib
{
   class COMMONLIB_EXPORT LineEdit : public QLineEdit
   {
      Q_OBJECT

      public:
                        LineEdit (QWidget *parent = nullptr);

         void           addEditButton ();
         void           setLocation (int row, int col);
         quint32        row ();
         quint32        col ();

      public slots:
         void           startEdit ();

         void           unselect ();
         void           remove ();

      signals:
         void           editClicked (commonLib::LineEdit* self);
         void           doneEditing (commonLib::LineEdit* self);
         void           hasSelection (commonLib::LineEdit* self);

      protected:
         void           resizeEvent (QResizeEvent* e) override;
         void           focusInEvent (QFocusEvent* e) override;
         void           focusOutEvent (QFocusEvent* e) override;
         void           keyPressEvent (QKeyEvent* e) override;

      private slots:
         void           editClicked ();
         void           showButtons();
         void           hideButtons ();
         void           processReturn ();
         void           textSelectionChanged ();

         private:
         QToolButton*   m_clearButton;
         QToolButton*   m_editButton;
         bool           m_editing;
         quint32        m_row;
         quint32        m_col;
   };
}
QT_END_NAMESPACE
