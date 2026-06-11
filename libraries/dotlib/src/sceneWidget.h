//=============================================================================
// X-Tek Corporation                                   http://www.x-tekcorp.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from X-Tek Corporation.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, X-Tek Corporation
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author:: thomas@x-tekcorp.com      $   $Revision: 1365 $
//
// $Date: 2017-01-18 11:59:18 -0500 (Wed, 18 Jan 2017) $
//=============================================================================
// $Id: xDotScene.h 1365 2017-01-18 16:59:18Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#pragma once

#include <QtWidgets>
#include <gvc.h>

#define DPI       72.0

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// An object containing a libgraph graph and its associated nodes and edges
namespace dotLib
{
   class sceneWidget : public QGraphicsScene
   {
      Q_OBJECT

      protected:
         static const char* const _graphCmds[];
         static const char* const _nodeCmds[];
         static const char* const _edgeCmds[];

         QRectF                           _bb;
         QString                          _text;
         int                              _index;
         QPen                             _pen;
         QBrush                           _brush;
         QFont                            _font;
         QString                          _style;
         bool                             _superscript;
         bool                             _subscript;

      public:
                                          sceneWidget (QObject* parent = nullptr);
         virtual                          ~sceneWidget () override;

         virtual void                     clear ();
         virtual bool                     render (const QString dot);
         void                             render (QPainter* painter);

      protected:
         virtual void                     drawGraph (Agraph_t* graph);
         virtual void                     drawSubGraph (Agraph_t* graph);
         virtual void                     drawNode (Agnode_t* node);
         virtual void                     drawEdge (Agedge_t* edge);
         virtual void                     parseCmd ();
         virtual QGraphicsEllipseItem*    drawEllipse (bool filled);
         virtual QGraphicsPolygonItem*    drawPolygon (bool filled);
         virtual QGraphicsPolygonItem*    drawPolyline ();
         virtual QGraphicsPathItem*       drawBspline (bool filled);
         virtual QGraphicsTextItem*       drawText ();
         virtual void                     setFont ();
         virtual void                     setFontStyle ();
         virtual void                     setStyle ();
         virtual void                     setPen ();
         virtual void                     setBrush ();
         virtual QColor                   parseColor ();
         virtual qreal                    parseNumber ();
         virtual QString                  parseString ();
         virtual void                     parseWhitespace ();
         virtual QRectF                   scale (qreal x0, qreal y0, qreal w, qreal h);
   };
}
