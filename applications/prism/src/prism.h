#pragma once

#include <QMainWindow>
#include "waveform_view_widget.h"

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
      void           saveWaveLayout(const QString& filename);
      void           loadWaveLayout(const QString& filename);

   private:
      Ui::prism*     ui;
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
