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
#include "arti_cmd.h"
#include "options_parser.h"
#include "renderer.h"
#include "message.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <QDir>
#include <QFile>
#include <QFileInfo>

using namespace artiLib;

ArtiCmd::ArtiCmd () {}

//-----------------------------------------------------------------------------
void ArtiCmd::run (const QStringList& args)
{
   OptionsParser parser;
   if (parser.parse(args.join(" ")) != OptionsParser::Ok) return;

   QString quiet = parser.quiet();
   QString initial = parser.initial();

   // run arti
   sendMessage(SysStatus, QString("Starting artifact generation using template '%1'\n").arg(initial));

   artiLib::Renderer a;
   a.setRtf(parser.rtf());
   if (a.render(initial, parser.context(), parser.incpaths())) {
      QString artifact = a.artifact();

      // output to file
      QString outfile = parser.outfile();
      if (!outfile.isEmpty()) {
         QFile fout(outfile);
         if (!fout.open(QIODevice::WriteOnly | QIODevice::Text))
            sendMessage(SysError, QString("Cannot open file '%1' for writing").arg(fout.fileName()));
         else {
            QTextStream sout(&fout);
            sout << artifact;
            fout.close();
            sendMessage(SysStatus, QString("Artifact written to file '%1'").arg(outfile));
         }
      }

      // write to stdout
      else sendMessage(SysInfo, artifact);
   }
   sendMessage(SysStatus, QString("\nArtifact generation complete"));
}
//-----------------------------------------------------------------------------
void ArtiCmd::sendMessage (int type, QString msg)
{
   if ((type == SysInfo    && m_quiet.contains('i')) ||
       (type == UserDebug  && m_quiet.contains('d')) ||
       (type == SysStatus  && m_quiet.contains('s')) ||
       (type == SysWarning && m_quiet.contains('w')) ||
       (type == SysError   && m_quiet.contains('e')) ||
       (type == SysFatal   && m_quiet.contains('f'))) return;

   QString severity = (type == SysError)   ? "Error: "
                    : (type == SysWarning) ? "Warning: "
                    : (type == SysFatal)   ? "Fatal: "
                    : (type == UserDebug)   ? "Debug: "
                    : (type == SysStatus)  ? "Status: "
                    : QString();
   qInfo() << qPrintable(QString("%1%2").arg(severity).arg(msg));
}
