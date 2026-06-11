#ifndef JSONLIB_GLOBAL_H
#define JSONLIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (JSONLIB_LIBRARY)
   #define JSONLIB_EXPORT Q_DECL_EXPORT
#else
   #define JSONLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // JSONLIB_GLOBAL_H
