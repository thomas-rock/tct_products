//=============================================================================
// RockSolid Logic Corporation                         www.RockSolidLogic.com  
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, RockSolid Solutions
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author:: thomas@x-tekcorp.com      $   $Revision: 1387 $
//
// $Date: 2017-02-07 11:22:48 -0500 (Tue, 07 Feb 2017) $
//=============================================================================
// $Id: main.cpp 1387 2017-02-07 16:22:48Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#include <QApplication>
#include "slate.h"
#include "revision.h"

int main(int argc, char **argv)
{   
   QApplication::setOrganizationName(COMPANY_NAME);
   QApplication::setOrganizationDomain(DOMAIN_NAME);
   QApplication::setApplicationName(APP);
   QApplication::setApplicationVersion(VERSION);

   QApplication app(argc, argv);

   Slate ed;
   ed.setWindowTitle(QString("Slate Artifact Viewer/Editor - version %1").arg(VERSION));
   ed.show();
   return app.exec();
}

