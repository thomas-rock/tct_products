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
#include <QLabel>

#include "text_edit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ArtiGui; }
QT_END_NAMESPACE

class MainPage;
class HelpPage;

class ArtiGui : public QMainWindow
{
   Q_OBJECT

   protected:
      struct locs {
         QStringView filename;
         int         line;
         int         col;
      };

   public:
                              ArtiGui (QWidget* parent = nullptr);
                              ~ArtiGui () override;

   protected Q_SLOTS:
      void                    setMode (bool state);

      // low-level slots
      void                    rootChanged (const QString& path);
      void                    editFile (QString path);
      void                    closeEvent (QCloseEvent* event) override;

   private:
      Ui::ArtiGui*            m_ui;
      MainPage*               m_main_page;
      texteditLib::TextEdit*  m_edit_page;
      QTabBar*                m_mode_tab;
      QList<QAction*>         m_mode_acts;
      QAction*                m_current_mode = nullptr;

      QString                 m_filename;
};
