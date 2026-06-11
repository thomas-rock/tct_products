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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1334 $
//
// $Date: 2016-09-23 16:46:30 -0400 (Fri, 23 Sep 2016) $
//=============================================================================
// $Id: xTextEditWidget_qsci.h 1334 2016-09-23 20:46:30Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#pragma once

#include <QDialog>
#include "commonlib_global.h"

namespace commonLib
{
   namespace Ui {class SummaryDialog;}

   class COMMONLIB_EXPORT SummaryDialog : public QDialog
   {
      Q_OBJECT

      public:
                              SummaryDialog (QString text, QWidget* parent = nullptr);
                              ~SummaryDialog () override;

      private:
         void                 setup ();

      private:
         Ui::SummaryDialog*   m_ui;
   };
}
