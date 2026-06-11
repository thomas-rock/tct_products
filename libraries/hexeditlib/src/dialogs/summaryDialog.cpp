#include "summaryDialog.h"
#include "ui_summaryDialog.h"

#include <QFileInfo>

using namespace hexeditLib;

summaryDialog::summaryDialog (QString filename, QWidget* parent)
   : QDialog(parent)
{
   setup();

   QFileInfo fi(filename);
   int permissions = fi.permissions();
   QString perm;
   if ((permissions & QFileDevice::ReadOwner) != 0) perm += "R";
   if ((permissions & QFileDevice::WriteOwner) != 0) perm += "W";
   if ((permissions & QFileDevice::ExeOwner) != 0) perm += "X";

   QString text = QString("Full File Name : %1\n"
                          "Created        : %2\n"
                          "Modified       : %3\n"
                          "File Size      : %4\n"
                          "Permissions    : %5\n").arg(filename)
                                                  .arg(fi.birthTime().toString())
                                                  .arg(fi.lastModified().toString())
                                                  .arg(QString::number(fi.size()))
                                                  .arg(perm);


   _ui->_textEntry->setPlainText(text);
}
summaryDialog::~summaryDialog () {delete _ui;}
//-----------------------------------------------------------------------------
void summaryDialog::setup ()
{
   _ui = new Ui::summaryDialog;
   _ui->setupUi(this);
}
