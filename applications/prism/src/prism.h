#pragma once

#include <QMainWindow>
#include <QString>

#include "waveform_view_widget.h"
#include "message.h"

namespace Ui {
   class prism;
}

class Prism : public QMainWindow
{
   Q_OBJECT

   public:
      explicit       Prism(QWidget* parent = nullptr);
                     ~Prism();

   protected slots:
      void           loadWaveform ();
      void           saveWaveLayout(const QString& filename);
      void           loadWaveLayout(const QString& filename);

      void           printMessage(MessageType type, QString msg, QString filename, int line, int col) {qDebug().noquote() << QString("%1 (%2 %3 : %4)").arg(msg).arg(filename).arg(line).arg(col);}
      void           printMessage(MessageType type, QString msg) {qDebug().noquote() << QString("%1").arg(msg);}

   private:
      Ui::prism*     m_ui;
};

/*
 * TO-DO List
 *
 * - Expand search parser
 * - Implement correct VCD parser
 * - Implement other waveform data parser(s)
 * - Drag/Drop from hierarchy to waves
 * - Add spacer to waves
 * - Remove Radix column
 * - Separate Wave Name / Wave Value into separate tree widgets
 * - Copy/Paste waveform
 * - Waveform compare
 * - Waveform icons in name trees
 * - Waveform configuration
 * -
 */
