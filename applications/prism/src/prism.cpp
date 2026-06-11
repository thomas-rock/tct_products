#include "prism.h"
#include "ui_prism.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QShortcut>

Prism::Prism(QWidget* parent) : QMainWindow(parent), ui(new Ui::prism)
{
   ui->setupUi(this);

   auto* waveTab = new WaveformViewWidget(this);

   ui->m_waveformTab->addTab(waveTab, "Waveform 1");

   connect(ui->m_save_format, &QAction::triggered,
           this, [this]()
           {
              saveWaveLayout("wave_layout.json");
           });

   connect(ui->m_load_format, &QAction::triggered,
           this, [this]()
           {
              loadWaveLayout("wave_layout.json");
           });

}

Prism::~Prism()
{
   delete ui;
}

void Prism::saveWaveLayout(const QString& filename)
{
   WaveformViewWidget* view = qobject_cast<WaveformViewWidget*>(ui->m_waveformTab->currentWidget());
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
   WaveformViewWidget* view = qobject_cast<WaveformViewWidget*>(ui->m_waveformTab->currentWidget());
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

