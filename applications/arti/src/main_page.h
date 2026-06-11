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
#pragma once

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QMap>
#include <QSettings>
#include <QProcess>
#include "preview.h"

QT_BEGIN_NAMESPACE
namespace Ui {class MainPage;}
QT_END_NAMESPACE

class MainPage : public QMainWindow
{
   Q_OBJECT

   public:
      explicit                MainPage (QWidget* parent = nullptr);
                              ~MainPage () override;


      void                    loadState (QSettings& settings);
      void                    saveState (QSettings& settings);

      QStringList             incpaths () const;
      QMap<QString, QString>  context () const;
      QString                 outfile () const;
      bool                    rtf () const;
      bool                    plaintext () const;
      bool                    markdown () const;
      bool                    transcript () const;
      bool                    preview () const;

   public Q_SLOTS:
      void                    run ();
      void                    check ();

   Q_SIGNALS:
      void                    initialChanged (const QString& path);
      void                    editRequest (QString);

   protected Q_SLOTS:
      void                    addMessage (int type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);
      void                    outfileClicked (int state);
      void                    browseOutfile ();

      void                    preferences ();
      void                    loadConfig ();
      void                    saveConfig ();
      void                    clearConfig (bool force = false);

      void                    setInitial (const QString);
      void                    openFile ();

   protected:
      void                    readConfig (QTextStream& cstream);
      void                    writeConfig (QTextStream& cstream);

   private:
      Ui::MainPage*           m_ui;
      QLabel*                 m_initial_template;
      Preview*                m_preview;
      QProcess                m_process;
};

