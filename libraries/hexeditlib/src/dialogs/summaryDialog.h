#pragma once

#include <QDialog>

namespace hexeditLib
{
   namespace Ui {class summaryDialog;}

   class summaryDialog : public QDialog
   {
      Q_OBJECT

      private:
         Ui::summaryDialog*   _ui;

      public:
                                 summaryDialog (QString filename, QWidget* parent = nullptr);
                                 ~summaryDialog () override;

      private:
         void                    setup ();
   };
}

