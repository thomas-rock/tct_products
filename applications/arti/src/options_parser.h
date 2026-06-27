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

#include "command_line_parser.h"
#include "message.h"
#include <QString>
#include <QStringList>
#include <QMap>
#include <QStack>

/******************************************************************************

                        Language Definition
                        -------------------
//-----------------------------------------------------------------------------
// Parser rules
//-----------------------------------------------------------------------------
command_line
   : option* ID
   ;

option
   : context
   | define
   | file
   | help
   | incpath
   | outfile
   | preview
   | quiet
   | transcript
   | version
   ;

context
   : (-c | --context) ID=value
   ;

help
   : (-h | --help)
   ;

file
   : (-f | --file) value
   ;

incpath
   : (-i | --incpath) value
   ;

outfile
   : (-o | --outfile) value
   ;

preview
   : (-p | --preview)

quiet
   : (-q | --quiet) value
   ;

transcript
   : (-t | --transcript)
   ;

version
   : (-v | --version)
   ;

value
   : ID
   | STRING
   | NUMBER
   ;

ID : [A-Za-z][0-9A-Za-z_]* ;
STRING : ".*?" | '.*?' ;
SUMBER : [0-9]+(.[0-9]+)?(e(+|-)?[0-9]+)? ;

******************************************************************************/

class OptionsParser  : public QObject
{
   Q_OBJECT

   public:
      typedef enum {
         Ok = 0,
         Help,
         Version,
         Error
      } Status;

   public:
                              OptionsParser ();

      Status                  parse (const QString& text);
      QStringList             incpaths () const;
      QMap<QString, QString>  context () const;
      QString                 outfile () const;
      bool                    preview () const;
      bool                    transcript () const;
      QString                 quiet () const;
      QString                 initial () const;

   Q_SIGNALS:
         void                 message (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

   protected:
      void                    parse_command_line ();
      void                    parse_option ();
      void                    parse_context ();
      void                    parse_help ();
      void                    parse_file ();
      void                    parse_hdlpath ();
      void                    parse_incpath ();
      void                    parse_outfile ();
      void                    parse_preview ();
      void                    parse_transcript ();
      void                    parse_quiet ();
      void                    parse_version ();
      QString                 parse_value ();

      commonLib::CommandLineParser::lexeme next_lex ();
      void                    consume_lex ();
      commonLib::CommandLineParser::lexeme look_ahead ();

      void                    version ();
      void                    help ();
      void                    syntax_error (QStringList exp);

   private:
      Status                  m_status = Ok;
      QStack<QString>         m_fileStack;
      QString                 m_filename;
      QList<commonLib::CommandLineParser::lexeme> m_lexemes;
      QStringList             m_hdlpaths;
      QStringList             m_incpaths;
      QMap<QString, QString>  m_context;
      QString                 m_outfile;
      bool                    m_preview;
      bool                    m_transcript;
      QString                 m_quiet;
      QString                 m_initial;

};

