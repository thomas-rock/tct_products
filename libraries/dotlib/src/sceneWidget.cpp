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
// $Id: xDotScene.cpp 1365 2017-01-18 16:59:18Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "sceneWidget.h"

#define CY(Y) _bb.height()-(Y)

using namespace dotLib;

//------------------------------
// static storage
//------------------------------
const char* const sceneWidget::_graphCmds[] = {"_draw_", "_ldraw_"};
const char* const sceneWidget::_nodeCmds[]  = {"_draw_", "_ldraw_"};
const char* const sceneWidget::_edgeCmds[]  = {"_draw_", "_ldraw_", "_hdraw_", "_tdraw_"};
//------------------------------


sceneWidget::sceneWidget (QObject* parent) : QGraphicsScene(parent) {}
sceneWidget::~sceneWidget () {}
//-----------------------------------------------------------------------------
void sceneWidget::clear () {QGraphicsScene::clear();}
bool sceneWidget::render (const QString dot)
{
   clear();

   _index = 0;
   _pen.setWidth(1);
   _brush.setStyle(Qt::SolidPattern);
   _superscript = false;
   _subscript = false;

   // read, layout and render graph
   GVC_t* context  = gvContext();
   Agraph_t* graph = agmemread(const_cast<char*>(qUtf8Printable(dot)));

   bool status = graph != nullptr;
   if (status) {
      gvLayout(context, graph, "dot");
      gvRender(context, graph, "xdot", nullptr);

//      Agsym_t* attr = 0;
//      while (attr = agnxtattr(graph, AGRAPH, attr))
//         qDebug() << "attibute: " << attr->name;

      // Read graphs, nodes and edges

      const char* bb = "bb";
      QStringList values = QString(agget(graph, (char*) bb)).split(',');
      _bb = QRectF(QPointF(values[0].toDouble(), values[1].toDouble()), QPointF(values[2].toDouble(), values[3].toDouble()));

      drawGraph(graph);

      gvFreeLayout(context, graph);
      agclose(graph);
   }
   gvFreeContext(context);

   return status;
}
void sceneWidget::render (QPainter* painter) {QGraphicsScene::render(painter);}
//-----------------------------------------------------------------------------
void sceneWidget::drawGraph (Agraph_t* graph)
{
   for (int i = 0; i < 2; i++) {
      _text = agget(graph, (char*)_graphCmds[i]);
      parseCmd();
   }

   for (Agraph_t* sub = agfstsubg(graph); sub != nullptr; sub = agnxtsubg(sub))
      drawSubGraph(sub);

   for (Agnode_t* node = agfstnode(graph); node != nullptr; node = agnxtnode(graph, node)) {
      drawNode(node);
      for (Agedge_t* edge = agfstout(graph, node); edge != nullptr; edge = agnxtout(graph, edge))
         drawEdge(edge);
   }
}
void sceneWidget::drawSubGraph (Agraph_t* graph)
{
   for (int i = 0; i < 2; i++) {
      _text = agget(graph, (char*)_graphCmds[i]);
      parseCmd();
   }

   for (Agraph_t* sub = agfstsubg(graph); sub != nullptr; sub = agnxtsubg(sub))
      drawSubGraph(sub);
}
void sceneWidget::drawNode (Agnode_t* node)
{
   for (int i = 0; i < 2; i++) {
      _text = agget(node, (char*)_nodeCmds[i]);
      parseCmd();
   }
}
void sceneWidget::drawEdge (Agedge_t* edge)
{
   for (int i = 0; i < 4; i++) {
      _text = agget(edge, (char*)_edgeCmds[i]);
      parseCmd();
   }
}
void sceneWidget::parseCmd ()
{
//qDebug() << "cmd: " << _text;
   _index = 0;
   while (_index < _text.length()) {
      QChar cmd = _text.at(_index++);
      if (cmd == 'E') drawEllipse(true);
      else if (cmd == 'e') drawEllipse(false);
      else if (cmd == 'P') drawPolygon(true);
      else if (cmd == 'p') drawPolygon(false);
      else if (cmd == 'L') drawPolyline();
      else if (cmd == 'B') drawBspline(false);
      else if (cmd == 'b') drawBspline(true);
      else if (cmd == 'T') drawText();
      else if (cmd == 'C') setBrush();
      else if (cmd == 'c') setPen();
      else if (cmd == 'F') setFont();
      else if (cmd == 't') setFontStyle();
      else if (cmd == 'S') setStyle();
   }
}
QGraphicsEllipseItem* sceneWidget::drawEllipse (bool filled)
{
   qreal x0 = parseNumber();
   qreal y0 = parseNumber();
   qreal w  = parseNumber();
   qreal h  = parseNumber();

   QRectF rect = scale(x0, y0, w, h);
   QGraphicsEllipseItem* item = addEllipse(rect, _pen, (filled) ? _brush : QBrush());
//   item->setZValue(10);
   return item;
}
QGraphicsPolygonItem* sceneWidget::drawPolygon (bool filled)
{
   QVector<QPointF> points;
   int n = int(parseNumber());
   for (int i = 0; i < n; i++) {
      qreal x = parseNumber();
      qreal y = parseNumber();
      points.append(QPointF(x, CY(y)));
   }
   return addPolygon(points, _pen, (filled) ? _brush : QBrush());
}
QGraphicsPolygonItem* sceneWidget::drawPolyline ()
{
   QVector<QPointF> points;
   int n = int(parseNumber());
   for (int i = 0; i < n; i++) {
      qreal x = parseNumber();
      qreal y = parseNumber();
      points.append(QPointF(x, CY(y)));
   }
   return addPolygon(points, _pen);
}
QGraphicsPathItem* sceneWidget::drawBspline (bool filled)
{
   Q_UNUSED(filled)

   QVector<QPointF> points;
   int n = int(parseNumber());
   for (int i = 0; i < n; i++) {
      qreal x = parseNumber();
      qreal y = parseNumber();
      points.append(QPointF(x, CY(y)));
   }
   QPainterPath path;
   path.moveTo(points[0]);
   for (int i = 1; i < points.size() - 1; i += 3)
      path.cubicTo(points[i], points[i+1], points[i+2]);
   return addPath(path);
}
QGraphicsTextItem* sceneWidget::drawText ()
{
   qreal x = parseNumber();
   qreal y = parseNumber();
   int j   = int(parseNumber());
   int w   = int(parseNumber());
   QString text = parseString();
   if (text == "\\]") text = "]";      // hack

   QFontMetricsF fm(_font);
   QRectF tbr = fm.boundingRect(text);

   QGraphicsTextItem* item = addText(text, _font);
   item->setDefaultTextColor(_pen.color());

   // this is difficult to get right
   // left-align
   if (j == -1)
      item->setPos(x + w/2 - 3 - tbr.width()/2, CY(y)-tbr.height());

   // center align
   else if (j == 0)
      item->setPos(x - 3 - tbr.width()/2, CY(y)-tbr.height());

   item->setZValue(10);
   item->setData(0, true);

   return item;
}
void sceneWidget::setFont ()
{
   _font.setPointSizeF(parseNumber()-6);
   _font.setFamily(parseString());
}
//    style value: OR of BOLD=1, ITALIC=2, UNDERLINE=4, SUPERSCRIPT=8, SUBSCRIPT=16, and STRIKE-THROUGH=32.
void sceneWidget::setFontStyle ()
{
   int style = int(parseNumber());
   _font.setBold(style & 0x1);
   _font.setItalic(style & 0x2);
   _font.setUnderline(style & 0x4);
   _superscript = style & 0x8;
   _subscript = style & 0x10;
   _font.setStrikeOut(style & 0x20);
}
void sceneWidget::setStyle ()
{
   // defaults
   _pen = QPen();

   QString style = parseString();
   if (style == "dotted")
      _pen.setStyle(Qt::DotLine);
   else if (style == "dashed")
      _pen.setStyle(Qt::DashLine);
   else if (style == "solid")
      _pen.setStyle(Qt::SolidLine);
   else if (style == "bold")
      _pen.setWidth(3);
   else if (style == "invis")
      _pen = QPen(Qt::NoPen);

   else if (style == "filled") {
      _brush.setStyle(Qt::SolidPattern);
      _pen.setBrush(_brush);
   }
   else if (style == "striped") {
      _brush.setStyle(Qt::VerPattern);
      _pen.setBrush(_brush);
   }
   else if (style == "wedged") {
      _brush.setStyle(Qt::SolidPattern);
      _pen.setBrush(_brush);
   }
   else if (style == "diagonals") {
      _brush.setStyle(Qt::BDiagPattern);
      _pen.setBrush(_brush);
   }
   else if (style == "rounded") {
      _brush.setStyle(Qt::SolidPattern);
      _pen.setBrush(_brush);
   }
}
void sceneWidget::setPen () {_pen.setColor(parseColor());}
void sceneWidget::setBrush () {_brush.setColor(parseColor());}
QColor sceneWidget::parseColor () {return QColor(parseString());}
qreal sceneWidget::parseNumber ()
{
   parseWhitespace();

   QString num;
   QChar c = _text.at(_index);
   while (c.isNumber() || c == '.' || c == '-') {
      num.append(c);
      c = _text.at(++_index);
   }
   return num.toDouble();
}
QString sceneWidget::parseString ()
{
   QString str;
   int chars = int(parseNumber());
   parseWhitespace();
   if (_text.at(_index) == '-') {
      for (int i = 0; i < chars; i++)
         str.append(_text.at(++_index));
      ++_index;    // read past last character of the string
   }
   return str;
}
void sceneWidget::parseWhitespace ()
{
   while (_index < _text.length()-1) {
      QChar c = _text.at(_index);
      if (!c.isSpace()) return;
      c = _text.at(++_index);
   }
}
QRectF sceneWidget::scale (qreal x0, qreal y0, qreal w, qreal h) {return QRectF(QPointF(x0-w, CY(y0 + h)), QSize(w*2, h*2));}
