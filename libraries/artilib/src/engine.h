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

#include "doc.h"
#include "arti_model.h"
#include <QStack>
#include <QJSEngine>

namespace artiLib
{
   class ARTILIB_EXPORT Engine   : public QObject
   {
      Q_OBJECT
         // for-loop variables
         struct LoopData
         {
            quint32  loop;
            quint32  rem;
            bool     last;
            bool     first;

            LoopData () : loop(0), rem(0), last(false), first(false) {}
            void set (quint32 l, quint32 r, bool la, bool f) {loop = l; rem = r; last = la; first = f;}
         };

         QStack<LoopData*>       m_loopStack;
         quint32                 m_loopCount = 0;
         quint32                 m_indent = 0;
         QStack<int>             m_indentStack;
         QJSEngine               m_jsengine;

      public:
                                 Engine (const QStringList& incpaths = {});
                                 ~Engine () override;

         void                    setMaxLoops (int max);

         bool                    check (const QString& filename);
         bool                    render (const QString& arti, QVariantMap contexts);
         QString                 artifact ();
         quint32                 errors () const {return m_errors;}
         quint32                 warnings () const {return m_warnings;}

      Q_SIGNALS:
         void                    message (MessageType type, const QString& msg, const QString& file, int line, int col);
         void                    message (MessageType type, const QString& msg);

      protected Q_SLOTS:
         void                    handleMessage (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

      protected:
         virtual ArtiNodePtr     parse(const QString& artiPath);
         virtual void            exec (ArtiNodePtr n);
         QVariant                eval (ArtiNodePtr n);

         QVariant                binop_string (quint32 op, const QString& l, const QString& r);
         QVariant                binop_real (quint32 op, const qreal& l, const qreal r);
         QVariant                binop_int (quint32 op, const int& l, const int& r);
         QVariant                binop_bool (quint32 op, const bool& l, const bool& r);

         int                     toInt (ArtiNodePtr n);
         QVariant                doFunc (const QString& name, QVariantList& args, const quint32 line, const quint32 pos);

         void                    writeText (const QString& text);
         bool                    isArray (const QVariant& value) const;
         bool                    isBool (const QVariant& value) const;
         bool                    isInt (const QVariant& value) const;
         bool                    isMap (const QVariant& value) const;
         bool                    isNull (const QVariant& value) const;
         bool                    isReal (const QVariant& value) const;
         bool                    isString (const QVariant& value) const;
         QString                 toString (const QVariant& value);
         QString                 findFile (QString filename) const;
         QJSValue                variant2js (QVariant var);

         QVariantList            searchArray (QVariant context, const QString& key);
         QVariantList            searchMap (QVariant context, const QString& key);

      protected:
         int                     m_maxLoops;
         QStringList             m_incpaths;
         Doc*                    m_doc;
         QMap<QString, ArtiDataPtr> m_parseData;
         ArtiDataPtr             m_currentParseData;

         QStack<QVariantMap*>    m_variableStack;
         QVariantMap*            m_variables = nullptr;
         QVariantMap             m_globals;
         QStack<QString>         m_filenames;
         quint32                 m_errors;
         quint32                 m_warnings;

   };
}
