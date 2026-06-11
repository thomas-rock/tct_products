#pragma once

#include <QMainWindow>

namespace Ui { class Slate; }

class Slate : public QMainWindow
{
   Q_OBJECT

   public:
                           Slate (QWidget* parent = nullptr);
                           ~Slate () override;

   protected Q_SLOTS:
      void                 setMode (bool state);
/*
      void                 setRtfMode ();
      void                 setTextMode ();
      void                 setHexMode ();
      void                 setPdfMode ();
      void                 setGraphMode ();
      void                 setChartMode ();
      void                 setImageMode ();
      void                 setSvgMode ();
      void                 setHelpMode ();
*/

      // low-level slots
//      void                 addMessage (int type, const QString& msg);
//      void                 closeEvent (QCloseEvent* event) override;

   private:
      Ui::Slate*           m_ui;
      QTabBar*             m_mode_tab;
      QList<QAction*>      m_mode_acts;
      QAction*             m_current_mode = nullptr;
};
