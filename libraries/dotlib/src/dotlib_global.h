#ifndef DOTLIB_GLOBAL_H
#define DOTLIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (DOTLIB_LIBRARY)
   #define DOTLIB_EXPORT Q_DECL_EXPORT
#else
   #define DOTLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // DOTLIB_GLOBAL_H
