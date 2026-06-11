#include <QApplication>
#include "prism.h"
#include "revision.h"

int main(int argc, char* argv[])
{
   QApplication::setOrganizationName(COMPANY_NAME);
   QApplication::setOrganizationDomain(DOMAIN_NAME);
   QApplication::setApplicationName(APP);
   QApplication::setApplicationVersion(VERSION);

   QApplication app(argc, argv);

   Prism w;
   w.show();

   return app.exec();
}