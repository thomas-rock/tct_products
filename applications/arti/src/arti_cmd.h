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

#include <QtCore>

class ArtiCmd
{
   public:
                     ArtiCmd ();

      void           run (const QStringList& args);

   protected:
      void           sendMessage (int type, QString msg);

   private:
      QStringList    m_args;
      QString        m_quiet;
      bool           m_ok;

};

