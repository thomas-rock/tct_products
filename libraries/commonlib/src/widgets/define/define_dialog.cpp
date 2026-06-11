#include "define_dialog.h"
#include "ui_define_dialog.h"
#include <QFileDialog>
#include <QPushButton>

using namespace commonLib;

DefineDialog::DefineDialog(QWidget *parent, const QString& title) : QDialog(parent), m_ui(new Ui::DefineDialog), m_title(title)
{
   m_ui->setupUi(this);
   m_ui->defineBox->setTitle(QString("%1 Specification").arg(m_title));

   setWindowTitle(QString("Add/Edit %1").arg(m_title));

   m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

   connect(m_ui->nameEntry, SIGNAL(textChanged(QString)), this, SLOT(validate()));
   connect(m_ui->valueEntry, SIGNAL(textChanged(QString)), this, SLOT(validate()));
   connect(m_ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));

}
DefineDialog::~DefineDialog() {delete m_ui;}

//-----------------------------------------------------------------------------
QPair<QString, QString> DefineDialog::define () const{return qMakePair(m_ui->nameEntry->text(), m_ui->valueEntry->text());}
//-----------------------------------------------------------------------------
void DefineDialog::browse ()
{
   QString file = QFileDialog::getSaveFileName(this, QString("%1 Value").arg(m_title), QDir::currentPath(), QString(), nullptr, QFileDialog::DontConfirmOverwrite);
   if (!file.isEmpty()) {
      m_ui->valueEntry->setText(file);
      QDir::setCurrent(file);
   }
}
void DefineDialog::validate ()
{
   m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_ui->nameEntry->text().isEmpty() && !m_ui->valueEntry->text().isEmpty());
}
