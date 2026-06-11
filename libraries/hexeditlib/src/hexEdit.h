#pragma once

#include <QStringList>
#include <QMainWindow>
#include <QDir>
#include <QLabel>

#include "editorlib_global.h"

namespace hexeditLib
{
   namespace Ui {class hexEdit;}

   class HEXEDITLIB_EXPORT hexEdit : public QMainWindow
   {
      Q_OBJECT

      private:
         Ui::hexEdit*            _ui;
         QLabel*                 _positionLabel;
         QLabel*                 _filenameLabel;
         QLabel*                 _languageLabel;
         QLabel*                 _selectedCountLabel;
         QMenu*                  _contextMenu;

      public:
                                 hexEdit (QWidget* parent = nullptr);
                                 ~hexEdit() override;

      public slots:
         void                    edit (const QString& filename, int line = 1, int col = 1);        // external access to opening a file

      protected Q_SLOTS:
         void                    updateFileMenu ();
         void                    recentSelected (QAction* file);
         void                    updateWindowMenu ();
         void                    editorChanged ();
         void                    dataChanged ();
         void                    currentAddressChanged(qint64 addr);
         void                    currentSizeChanged(qint64 size);
         void                    overwriteModeChanged(bool mode);
         void                    contextMenu (QPoint pos);

      private:
         void                    setup ();
   };
}
