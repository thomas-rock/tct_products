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
#ifndef TEXTEDITLIB_GLOBAL_H
#define TEXTEDITLIB_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined (TEXTEDITLIB_LIBRARY)
   #define TEXTEDITLIB_EXPORT Q_DECL_EXPORT
#else
   #define TEXTEDITLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // TEXTEDITLIB_GLOBAL_H
