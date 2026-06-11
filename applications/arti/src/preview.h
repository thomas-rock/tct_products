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

#include <QMainWindow>
#include "doc.h"

namespace Ui {class Preview;}
class Preview : public QMainWindow
{
   Q_OBJECT

   public:
      explicit          Preview (QWidget* parent = nullptr);
                        ~Preview () override;

      void              addDocument (const QString& filename, artiLib::DocPtr doc);

   protected Q_SLOTS:
      void              tabChanged (int index);
      void              saveFile ();
      void              closeTab (int index);

   protected:
      void              closeEvent (QCloseEvent *event) override;

   private:
      Ui::Preview*      m_ui;
      QLabel*           m_filename;
      QMap<QString, artiLib::DocPtr> m_docs;


};
