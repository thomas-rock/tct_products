#ifndef CHARTLIB_GLOBAL_H
#define CHARTLIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (CHARTLIB_LIBRARY)
   #define CHARTLIB_EXPORT Q_DECL_EXPORT
#else
   #define CHARTLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // CHARTLIB_GLOBAL_H
