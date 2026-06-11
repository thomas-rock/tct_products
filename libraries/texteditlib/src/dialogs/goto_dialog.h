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
#pragma once

#include <QDialog>

namespace texteditLib
{
   namespace Ui {class GotoDialog;}

   class GotoDialog : public QDialog
   {
      Q_OBJECT

      public:
                           GotoDialog (int maxLine, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
                           ~GotoDialog () override;

         int               line ();

      private:
         Ui::GotoDialog*   m_ui;
   };
}
