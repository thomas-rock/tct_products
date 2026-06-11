#pragma once

#include <QMainWindow>
#include <QSettings>

namespace Ui {class HelpPage;}

class HelpPage : public QMainWindow
{
   Q_OBJECT

   public:
      explicit          HelpPage(QWidget* parent = nullptr);
                        ~HelpPage() override;

      void              loadState (QSettings& settings);
      void              saveState (QSettings& settings);

   private:
      Ui::HelpPage*     m_ui;
};

