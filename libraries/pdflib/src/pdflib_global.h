#ifndef PDFLIB_GLOBAL_H
#define PDFLIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (PDFLIB_LIBRARY)
   #define PDFLIB_EXPORT Q_DECL_EXPORT
#else
   #define PDFLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // PDFLIB_GLOBAL_H
