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
//#include "viewer.h"
#include "revision.h"
#include "arti_cmd.h"
#include "arti_gui.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QDebug>
#include <QStyleFactory>

int main(int argc, char** argv)
{
   QApplication::setOrganizationName(COMPANY_NAME);
   QApplication::setOrganizationDomain(DOMAIN_NAME);
   QApplication::setApplicationName(APP);
   QApplication::setApplicationVersion(VERSION);
   QApplication::setStyle(QStyleFactory::create("Fusion"));

   QApplication app(argc, argv);

   if (argc > 1) {

      QStringList args;
      for (int i = 1; i < argc; i++)
         args.append(argv[i]);

      ArtiCmd cmd;
      cmd.run(args);
      return 1;
   }

   // gui mode
   ArtiGui gui;
   gui.show();
   return app.exec();
}

