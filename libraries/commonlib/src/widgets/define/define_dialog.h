#pragma once

#include <QDialog>

namespace commonLib
{
   namespace Ui {
      class DefineDialog;
   }

   class DefineDialog : public QDialog
   {
      Q_OBJECT

      public:
                                 DefineDialog(QWidget* parent = nullptr, const QString& title = "Define");
                                 ~DefineDialog() override;

         QPair<QString, QString> define () const;

      protected Q_SLOTS:
         void                    browse ();
         void                    validate ();

      private:
         Ui::DefineDialog*       m_ui;
         QString                 m_title;
   };
}
