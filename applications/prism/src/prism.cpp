#include "prism.h"
#include "ui_prism.h"
#include "vcd_parser.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>

Prism::Prism(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::prism)
{
   m_ui->setupUi(this);

   connect(m_ui->m_open, &QAction::triggered,
           this,         &Prism::loadWaveform);

   connect(m_ui->m_save_format, &QAction::triggered,
           this, [this]()
           {
              saveWaveLayout("wave_layout.json");
           });

   connect(m_ui->m_load_format, &QAction::triggered,
           this, [this]()
           {
              loadWaveLayout("wave_layout.json");
           });
}

Prism::~Prism()
{
   delete m_ui;
}

void Prism::loadWaveform ()
{
   QString filename = QFileDialog::getOpenFileName(this, tr("Open Waveform File"), QDir::currentPath(), QString("VCD (*.vcd)"));
   if (filename.isEmpty()) return;

   QFile vcd(filename);
   if (!vcd.open(QFile::ReadOnly)) {
      QMessageBox::critical(this, tr("File Open Error"), QString("Cannot open file %1 for reading").arg(filename));
      return;
   }

   QTextStream vcd_stream(&vcd);
   VcdParser p(vcd_stream);
   connect(&p, SIGNAL(message(MessageType,QString,QString,int,int)), this, SLOT(printMessage(MessageType, QString, QString, int, int)));
   connect(&p, SIGNAL(message(MessageType,QString)), this, SLOT(printMessage(MessageType, QString)));
   std::unique_ptr<WaveformDocument> document = p.parse();

   document->dump();

   auto* waveTab = new WaveformViewWidget(this);
   m_ui->m_waveformTab->addTab(waveTab, QFileInfo(filename).fileName());
   waveTab->loadDocument(std::move(document));
}

void Prism::saveWaveLayout(const QString& filename)
{
   WaveformViewWidget* view = qobject_cast<WaveformViewWidget*>(m_ui->m_waveformTab->currentWidget());
   if (!view)
      return;

   QFile file(filename);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return;

   QJsonObject root;
   root["version"] = 1;
   root["waves"] = view->waveLayoutToJson();

   file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void Prism::loadWaveLayout(const QString& filename)
{
   WaveformViewWidget* view = qobject_cast<WaveformViewWidget*>(m_ui->m_waveformTab->currentWidget());
   if (!view)
      return;

   QFile file(filename);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;

   QJsonParseError error;
   const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

   if (error.error != QJsonParseError::NoError || !doc.isObject())
      return;

   const QJsonObject root = doc.object();

   if (root.value("version").toInt() != 1)
      return;

   view->loadWaveLayoutFromJson(root.value("waves").toArray());
}

