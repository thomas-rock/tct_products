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
#ifndef ARTILIB_GLOBAL_H
#define ARTILIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (ARTILIB_LIBRARY)
   #define ARTILIB_EXPORT Q_DECL_EXPORT
#else
   #define ARTILIB_EXPORT Q_DECL_IMPORT
#endif

#endif // ARTILIB_GLOBAL_H
