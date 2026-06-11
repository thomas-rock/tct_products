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

#include "artilib_global.h"

#include <QXmlStreamReader>
#include "parser.h"
#include "doc.h"
#include "engine.h"

namespace artiLib
{
   class ARTILIB_EXPORT Renderer : public QObject
   {
      Q_OBJECT

      protected:
         Engine*           m_engine;

      public:
                           Renderer ();
                           ~Renderer () override;

         void              setRtf (bool state);
         void              setPlaintext (bool state);
         void              setMarkdown (bool state);
         void              setMaxLoops (int max);

         bool              check (const QString filename);
         bool              render (const QString& arti_name, const QMap<QString, QString>& context, const QStringList& incpaths);
         QString           artifact ();
         DocPtr            doc ();

      Q_SIGNALS:
         void              message (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

      protected:
         QVariant          readJson (const QString& filename);
         QVariant          readXml (const QString& filename);
         QVariantMap       createTag (QXmlStreamReader& reader);
   };
}
