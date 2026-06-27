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
#include "renderer.h"
#include "json5_parser.h"

#include <QJsonDocument>
#include <QFileInfo>
#include <QFile>

using namespace artiLib;

Renderer::Renderer () : QObject() {m_engine = new Engine();}
Renderer::~Renderer() {delete m_engine;}
//-----------------------------------------------------------------------------
void Renderer::setMaxLoops (int max) {m_engine->setMaxLoops(max);}
//-----------------------------------------------------------------------------
bool Renderer::check (const QString filename)
{
   MESSAGE(SysStatus, QString("Starting syntax check of template '%1'\n").arg(filename));
   connect(m_engine, SIGNAL(message(int,QString,QString,int,int)), this, SIGNAL(message(int, QString, QString, int, int)));
   bool status = m_engine->check(filename);
   MESSAGE(SysStatus, QString("Syntax check complete - Errors: %1. Warnings: %2").arg(m_engine->errors()).arg(m_engine->warnings()));
   return status;
}

bool Renderer::render (const QString& arti_name, const QMap<QString, QString>& context, const QStringList& incpaths)
{
   // get context(s)
   QVariantMap qcontext;
   foreach (QString key, context.keys()) {
      QString value = context.value(key);
      QString suffix = QFileInfo(value).suffix().toLower();

      // read json data files
      if (suffix == "json")
         qcontext.insert(key, readJson(value));

      // read xml data files
      else if (suffix == "xml")
         qcontext.insert(key, readXml(value));

      // basic define
      // add appropriate QVariant type
      else {
         bool valid;
         qreal rtest = value.toDouble(&valid);
         if (valid) qcontext.insert(key, rtest);
         else {
            int itest = value.toInt(&valid);
            if (valid) qcontext.insert(key, itest);
            else qcontext.insert(key, value);
         }
         qcontext.insert(key, value);
      }
   }

   MESSAGE(SysStatus, QString("Starting artifact generation using template '%1'\n").arg(arti_name));
   connect(m_engine, SIGNAL(message(int,QString,QString,int,int)), this, SIGNAL(message(int, QString, QString, int, int)));
   connect(m_engine, SIGNAL(message(int,QString)), this, SIGNAL(message(int, QString)));
   bool status = m_engine->render(arti_name, qcontext);
   MESSAGE(SysStatus, QString("Artifact generation complete - Errors: %1. Warnings: %2").arg(m_engine->errors()).arg(m_engine->warnings()));
   return status;
}

QString Renderer::artifact () {return m_engine->artifact();}
//-----------------------------------------------------------------------------
QVariant Renderer::readJson (const QString& filename)
{
   QFile jfile(filename);
   if (jfile.open(QIODevice::ReadOnly|QIODevice::Text)) {
      QTextStream jstream(&jfile);

      jsonLib::Json5Parser j5p(jstream, filename);

      // forward messages
      connect(&j5p, SIGNAL(message(MessageType, const QString&, const QString&, int, col)),
              this, SIGNAL(message(MessageType, const QString&, const QString&, int, col)));
      connect(&j5p, SIGNAL(message(MessageType, const QString&)),
              this, SIGNAL(message(MessageType, const QString&)));

      QVariant rtn = j5p.parse();

      return rtn;
   }
   return filename;
}
QVariant Renderer::readXml (const QString& filename)
{
   QFile file(filename);
   QVariantMap xml;
   QVariantList children;
   if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
      QXmlStreamReader reader(&file);
      while (!reader.atEnd()) {
         reader.readNext();
         if (reader.isStartElement()) {
            QVariantMap child;
            child.insert(reader.name().toString(), createTag(reader));
            children.append(child);
         }
      }
   }
   if (children.size()) xml.insert("children", children);
   return xml;
}
QVariantMap Renderer::createTag (QXmlStreamReader& reader)
{
   QVariantMap tag;
   QString name = reader.name().toString();
   QVariantMap attrs;
   foreach (QXmlStreamAttribute attr, reader.attributes()) {
      QString value = attr.value().toString();
      bool ok;
      qreal rvalue = value.toDouble(&ok);
      if (ok) attrs.insert(attr.name().toString(), rvalue);
      else {
         int ivalue = value.toInt(&ok);
         if (ok) attrs.insert(attr.name().toString(), ivalue);
         else attrs.insert(attr.name().toString(), attr.value().toString());
      }
   }
   if (attrs.size()) tag.insert("attributes", attrs);

   QVariantList children;
   while (!reader.atEnd())
   {
      reader.readNext();

      if (reader.isEndElement() && reader.name().toString() == name) {
         if (children.size()) tag.insert("children", children);
         return tag;
      }
      else if (reader.isCharacters() && !reader.isWhitespace())
         tag.insert("value", reader.text().toString());
      else if (reader.isStartElement()) {
         QVariantMap child;
         child.insert(reader.name().toString(), createTag(reader));
         children.append(child);
      }
   }
   if (children.size()) tag.insert("children", children);
   return tag;
}

