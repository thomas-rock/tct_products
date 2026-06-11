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
#include "preview.h"
#include "ui_preview.h"

#include <QFileInfo>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

Preview::Preview(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::Preview)
{
   m_ui->setupUi(this);

   QLabel* title = new QLabel("Filename:  ");
   m_ui->statusbar->addWidget(title);
   m_filename = new QLabel();
   m_ui->statusbar->addWidget(m_filename);

   connect(m_ui->tabWidget, &QTabWidget::tabCloseRequested,
           this,            &Preview::closeTab);

   connect(m_ui->tabWidget, &QTabWidget::currentChanged,
           this,            &Preview::tabChanged);

   connect(m_ui->saveAct, &QAction::triggered,
           this,          &Preview::saveFile);

   connect(m_ui->exitAct, &QAction::triggered,
           this,          &Preview::close);

   setWindowTitle(QString("Artifact Preview"));

   // setup tabWidget tab styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      QString text = s.readAll();
      text.replace("<TAB_COLOR>", "00A080");
      setStyleSheet(text);
   }
}
Preview::~Preview() {delete m_ui;}
//-----------------------------------------------------------------------------
void Preview::addDocument (const QString& filename, artiLib::DocPtr doc)
{
   // check if doc exists
   if (m_docs.contains(filename)) {
      for (int index = 0; index < m_ui->tabWidget->count(); index++) {
         if (m_ui->tabWidget->tabToolTip(index) == filename) {
            closeTab(index);
            break;
         }
      }
   }
   m_docs.insert(filename, doc);
   QTextEdit* viewer = new QTextEdit;
   viewer->setDocument(doc.get());
   int index = m_ui->tabWidget->addTab(viewer, QFileInfo(filename).fileName());
   m_ui->tabWidget->setTabToolTip(index, filename);
   m_ui->tabWidget->setCurrentIndex(index);
   tabChanged(index);
}
//-----------------------------------------------------------------------------
void Preview::tabChanged (int index) {m_filename->setText(m_ui->tabWidget->tabToolTip(index));}
void Preview::saveFile ()
{
   QString filename = m_ui->tabWidget->tabToolTip(m_ui->tabWidget->currentIndex());
   artiLib::DocPtr doc = m_docs[filename];
   QString filter = (doc->isHtml()) ? tr("HTML (*.html)") : (doc->isMd()) ? tr("Markdown (*.md)") : tr("Plain Text (*.txt)");
   QString file = QFileDialog::getSaveFileName(this, tr("Artifact filename"), QDir::currentPath(), filter);
   if (file.isEmpty()) return;

   QFile fio(file);
   if (!fio.open(QIODevice::WriteOnly)) {
      QString msg = QString("Cannot open file '%1' for writing").arg(file);
      QMessageBox::critical(this, QString("File Error"), msg);
   }
   else {
      QTextStream stream(&fio);
      if      (doc->isHtml()) stream << doc->toHtml();
      else if (doc->isMd())   stream << doc->toMarkdown();
      else                    stream << doc->toString();
   }
   fio.close();
}
void Preview::closeTab (int index)
{
   QString filename = m_ui->tabWidget->tabToolTip(index);
   m_ui->tabWidget->removeTab(index);
   m_docs.remove(filename);
}
//-----------------------------------------------------------------------------
void Preview::closeEvent (QCloseEvent* event)
{
   m_ui->tabWidget->clear();
   m_docs.clear();
   QWidget::closeEvent(event);
}
