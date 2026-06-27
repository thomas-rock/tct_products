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
#include "main_page.h"
#include "message.h"
#include "renderer.h"
#include "preview.h"
#include "settings_p.h"
#include "ui_main_page.h"
#include "options_parser.h"
#include "preferences_dialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

MainPage::MainPage(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainPage)
{
   m_ui->setupUi(this);
   m_ui->context->setTitle(tr("Context"));
   m_preview = new Preview(this);
   m_preview->hide();

   // add status
   m_root_template = new QLabel;
   QLabel* title = new QLabel("Root Template:  ");
   m_ui->statusBar->addWidget(title);
   m_ui->statusBar->addWidget(m_root_template);

   // setup controls
   connect(m_ui->preferencesAct,    &QAction::triggered,
           this,                    &MainPage::preferences);

   // elide left the template list and expand new paths
   m_ui->templates->setTextElideMode(Qt::ElideLeft);
   m_ui->templates->setExpandNew(true);

   m_ui->outfileLabel->setEnabled(false);
   m_ui->outfileEntry->setEnabled(false);
   m_ui->outfileBrowse->setEnabled(false);

   connect(m_ui->outfileCheck,   &QCheckBox::checkStateChanged,
           this,                 &MainPage::outfileClicked);

   connect(m_ui->outfileBrowse,  &QToolButton::clicked,
           this,                 &MainPage::browseOutfile);

   connect(m_ui->loadAct,        &QAction::triggered,
           this,                 &MainPage::loadConfig);

   connect(m_ui->saveAct,        &QAction::triggered,
           this,                 &MainPage::saveConfig);

   connect(m_ui->clearAct,       &QAction::triggered,
           this,                 &MainPage::clearConfig);

   connect(m_ui->incpaths,       &commonLib::IncludePath::pathAdded,
           m_ui->templates,      &TemplateList::addPath);

   connect(m_ui->incpaths,       &commonLib::IncludePath::pathRemoved,
           m_ui->templates,      &TemplateList::removePath);

   connect(m_ui->templates,      &TemplateList::editRequest,
           this,                 &MainPage::editRequest);

   connect(m_ui->templates,      &TemplateList::rootChanged,
           this,                 &MainPage::setRoot);
}

MainPage::~MainPage() {delete m_ui;}
//-----------------------------------------------------------------------------
void MainPage::run ()
{
   QSettings settings;

   const QString header = QString(31, '-') + " Template Output " + QString(32, '-');

   bool trans = transcript();
   bool prev  = preview();
   bool ofile = m_ui->outfileCheck->isChecked();
   QString outfile = (m_ui->outfileCheck->isChecked()) ? m_ui->outfileEntry->text() : QString();

   if (!trans && !prev && !ofile)
      if (QMessageBox::question(this, tr("Output Warning"), tr("No output option selected. Continue anyway?")) == QMessageBox::No) return;

   if (ofile && outfile.isEmpty())
      if (QMessageBox::question(this, tr("Output Warning"), tr("File output selected by no output file specified. Continue anyway?")) == QMessageBox::No) return;

   if (settings.value("clearOnRun", false).toBool()) m_ui->transcript->clear();

   QString root = m_ui->templates->rootPath();

   artiLib::Renderer arti;
   connect(&arti, SIGNAL(message(int, QString, QString, int, int)), this, SLOT(addMessage(int, QString, QString, int, int)));
   arti.setMaxLoops(settings.value("maxLoops", 100).toInt());
   if (arti.render(root, context(), incpaths())) {
      QString artifact = arti.artifact();

      if (trans) {
         // bypass checks for quiet performed in addMessage
         m_ui->transcript->addMessage(SysInfo, header);
         m_ui->transcript->addMessage(SysInfo, artifact);
         m_ui->transcript->addMessage(SysInfo, QString(80, '-'));
      }
      if (prev) {
         m_preview->addArtifact(root, arti.artifact());
         m_preview->show();
      }
      if (!outfile.isEmpty()) {
         QFile ofile(outfile);
         if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QString msg = QString("Cannot open output file '%1' for writing").arg(outfile);
            addMessage(SysError, msg);
            QMessageBox::critical(this, tr("File Error"), msg);
         }
         else {
            QTextStream ostream(&ofile);
            ostream << artifact;
            ofile.close();
            addMessage(SysStatus, QString("Artifact written to file '%1'").arg(outfile));
         }
      }
   }
}
void MainPage::check ()
{
   QSettings settings;

   QString currentPath = m_ui->templates->currentPath();

   if (settings.value("clearOnCheck", false).toBool()) m_ui->transcript->clear();
   artiLib::Renderer arti;
   connect(&arti, SIGNAL(message(MessageType, QString, QString, int, int)), this, SLOT(addMessage(MessageType, QString, QString, int, int)));
   connect(&arti, SIGNAL(message(MessageType, QString)), this, SLOT(addMessage(MessageType, QString)));
   arti.check(currentPath);
}

//-----------------------------------------------------------------------------
void MainPage::loadState (QSettings& settings)
{
   if (settings.value("loadLastConfig").toBool()) {
      QString config = settings.value("config").toString();
      QTextStream cstream(&config);
      readConfig(cstream);
   }
}
void MainPage::saveState (QSettings& settings)
{
   QString config;
   QTextStream cstream(&config);
   writeConfig(cstream);

   settings.setValue("config", config);
}
//-----------------------------------------------------------------------------
QStringList MainPage::incpaths () const {return m_ui->incpaths->paths();}
QMap<QString, QString> MainPage::context () const {return m_ui->context->defines();}
QString MainPage::outfile () const {return (m_ui->outfileCheck->isChecked()) ? m_ui->outfileEntry->text() : QString();}
bool MainPage::transcript () const {return m_ui->transcriptCheck->isChecked();}
bool MainPage::preview () const {return m_ui->previewCheck->isChecked();}
//-----------------------------------------------------------------------------
void MainPage::addMessage (int type, const QString& msg, const QString& file, int line, int col)  {
{
   if ((m_ui->infoCheck->isChecked()    && (type & (UserInfo    | SysInfo)))    ||
       (m_ui->debugCheck->isChecked()   && (type & UserDebug))                  ||
       (m_ui->statusCheck->isChecked()  && (type & (UserStatus  | SysStatus)))  ||
       (m_ui->warningCheck->isChecked() && (type & (UserWarning | SysWarning))) ||
       (m_ui->errorCheck->isChecked()   && (type & (UserError   | SysError)))   ||
       (m_ui->fatalCheck->isChecked()   && (type & (UserFatal   | SysFatal)))) return;

   m_ui->transcript->addMessage(type, msg, file, line, col);}
}
void MainPage::addMessage (int type, const QString& msg)  {
   {
      if ((m_ui->infoCheck->isChecked()    && (type & (UserInfo    | SysInfo)))    ||
          (m_ui->debugCheck->isChecked()   && (type & UserDebug))                  ||
          (m_ui->statusCheck->isChecked()  && (type & (UserStatus  | SysStatus)))  ||
          (m_ui->warningCheck->isChecked() && (type & (UserWarning | SysWarning))) ||
          (m_ui->errorCheck->isChecked()   && (type & (UserError   | SysError)))   ||
          (m_ui->fatalCheck->isChecked()   && (type & (UserFatal   | SysFatal)))) return;

      m_ui->transcript->addMessage(type, msg);}
}
void MainPage::outfileClicked (int state)
{
   m_ui->outfileLabel->setEnabled(state == Qt::Checked);
   m_ui->outfileEntry->setEnabled(state == Qt::Checked);
   m_ui->outfileBrowse->setEnabled(state == Qt::Checked);
}
void MainPage::browseOutfile ()
{
   QString ofile = QFileDialog::getSaveFileName(this, tr("Output File"), QDir::currentPath());
   if (!ofile.isEmpty()) {
      m_ui->outfileEntry->setText(ofile);
      QDir::setCurrent(ofile);
   }
}
//-----------------------------------------------------------------------------
void MainPage::preferences ()
{
   PreferencesDialog prefs(this);
   prefs.exec();
}
void MainPage::loadConfig ()
{
   QString file = QFileDialog::getOpenFileName(this, tr("Load Configuration File"), QDir::currentPath());
   if (!file.isEmpty()) {
      QFile cfile(file);
      if (!cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
         QString msg = QString("Cannot open configuration file '%1' for reading").arg(file);
         QMessageBox::critical(this, tr("Configure File Error"), msg);
         addMessage(SysError, msg);
      }
      else {
         clearConfig(true);

         QTextStream cstream(&cfile);
         readConfig(cstream);
      }
      QDir::setCurrent(QFileInfo(file).absolutePath());
   }
}
void MainPage::saveConfig ()
{
   QString file = QFileDialog::getSaveFileName(this, tr("Save Configuration File"), QDir::currentPath());
   if (!file.isEmpty()) {
      QFile cfile(file);
      if (!cfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
         QString msg = QString("Cannot open configuration file '%1' for writing").arg(file);
         QMessageBox::critical(this, tr("Configure File Error"), msg);
         addMessage(SysError, msg);
      }
      else {
         QTextStream cstream(&cfile);
         writeConfig(cstream);
      }
   }
}
void MainPage::clearConfig (bool force)
{
   if (force || QMessageBox::question(this, tr("Confirm Clear"), tr("Are you sure you want to clear all configuration information?")) == QMessageBox::Yes) {
      m_ui->incpaths->clear();
      m_ui->context->clear();
      m_ui->outfileEntry->clear();

      if (transcript()) m_ui->transcriptCheck->click();
      if (preview()) m_ui->previewCheck->click();
      if (m_ui->outfileCheck->isChecked())    m_ui->outfileCheck->click();
   }
}
//-----------------------------------------------------------------------------
void MainPage::setRoot (const QString path)
{
   m_root_template->setText(path);
   emit rootChanged(path);
}
void MainPage::openFile ()
{
   QString currentPath = m_ui->templates->currentPath();
   if (currentPath.isEmpty()) return;

   QSettings settings;
   QString editor = settings.value("externalEditor").toString();
   if (editor.isEmpty()) {
      emit editRequest(currentPath);
      return;
   }

   editor = editor.trimmed();
   editor.replace("%f", currentPath);

   m_process.setProgram(editor);
   m_process.setArguments({});
   m_process.startDetached();
}
//-----------------------------------------------------------------------------
void MainPage::readConfig (QTextStream& cstream)
{
   OptionsParser parser;
   connect(&parser, SIGNAL(message(MessageType,QString,QString,int,int)), this, SLOT(addMessage(MessageType,QString,QString,int,int)));
   connect(&parser, SIGNAL(message(MessageType,QString)), this, SLOT(addMessage(MessageType,QString)));
   if (parser.parse(cstream.readAll()) != OptionsParser::Ok) return;
   QMap<QString, QString> ctx = parser.context();
   foreach (QString key, ctx.keys())
      m_ui->context->addDefine(key, ctx.value(key));

   foreach (QString incpath, parser.incpaths())
      m_ui->incpaths->addPath(incpath);

   QString outfile = parser.outfile();
   if (!outfile.isEmpty()) {
      m_ui->outfileEntry->setText(outfile);
      m_ui->outfileCheck->click();
   }
   m_ui->transcriptCheck->setChecked(parser.transcript());
   m_ui->previewCheck->setChecked(parser.preview());

   QString quiet = parser.quiet();
   m_ui->infoCheck->setChecked(quiet.contains("i"));
   m_ui->debugCheck->setChecked(quiet.contains("d"));
   m_ui->statusCheck->setChecked(quiet.contains("s"));
   m_ui->warningCheck->setChecked(quiet.contains("w"));
   m_ui->errorCheck->setChecked(quiet.contains("e"));
   m_ui->fatalCheck->setChecked(quiet.contains("f"));

   QString root = parser.initial();
   m_ui->templates->setRootPath(root);
   setRoot(root);
}
void MainPage::writeConfig (QTextStream& cstream)
{
   foreach (QString path, incpaths())
      cstream << QString("--incpath \"%1\"\n").arg(path);

   QMap<QString, QString> ctx = context();
   foreach (QString key, ctx.keys())
      cstream << QString("--context %1=\"%2\"\n").arg(key, ctx.value(key));

   QString ofile = outfile();
   if (!ofile.isEmpty()) cstream << QString("--outfile \"%1\"\n").arg(ofile);

   if (transcript()) cstream << QString("--transcript\n");
   if (preview()) cstream << QString("--preview\n");

   QString quiet;
   if (m_ui->infoCheck->isChecked()) quiet.append("i");
   if (m_ui->debugCheck->isChecked()) quiet.append("d");
   if (m_ui->statusCheck->isChecked()) quiet.append("s");
   if (m_ui->warningCheck->isChecked()) quiet.append("w");
   if (m_ui->errorCheck->isChecked()) quiet.append("e");
   if (m_ui->fatalCheck->isChecked()) quiet.append("f");
   if (!quiet.isEmpty()) cstream << QString("--quiet %1\n").arg(quiet);

   QString root = m_ui->templates->rootPath();
   if (!root.isEmpty()) cstream << QString("\"%1\"\n").arg(root);
}
