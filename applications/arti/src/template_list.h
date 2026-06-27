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

#include <QTreeWidget>
#include <QFileSystemWatcher>

class TemplateList : public QTreeWidget
{
   Q_OBJECT

   public:
      explicit             TemplateList (QWidget* parent = nullptr);

      QString              currentPath ();
      void                 setExpandNew (bool state);
      QString              rootPath ();

   public Q_SLOTS:
      void                 addPath (const QString& path);
      void                 removePath (const QString& path);
      void                 setCurrentRoot ();
      void                 setRootPath (const QString& path);

   Q_SIGNALS:
      void                 editRequest (const QString& path);
      void                 rootChanged (const QString &path);

   protected Q_SLOTS:
      void                 updatePath (const QString& path);
      void                 updatePath (QTreeWidgetItem* item);
      void                 editCurrent ();
      void                 directoryChanged (const QString& path);
      void                 itemDoubleClicked (QTreeWidgetItem* item);
      void                 contextMenu ();

   protected:
      void                 clearRoot ();
      QTreeWidgetItem*     findItem (const QString& path);
      bool                 isArtiFile (QString path);

   private:
      QFileSystemWatcher   m_watcher;
      QTreeWidgetItem*     m_root;
      bool                 m_incSubdir;
      bool                 m_expandNew;
      QMenu*               m_templateContext;
      QAction*             m_set_root;
      QAction*             m_open;

};
