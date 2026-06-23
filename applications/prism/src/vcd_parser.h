// vcd_recursive_descent_parser_qt.hpp
// Qt-oriented recursive-descent VCD parser.
//
// C++17 + Qt 6.
//
// Design goals:
// - Uses Qt types: QString, QVector, QChar, QTextStream.
// - Reads from QTextStream incrementally instead of slurping the full VCD file.
// - Uses a single-character pushback/lookahead buffer.
// - Parses text blocks such as $comment ... $end by scanning until the next
//   standalone "$end" keyword.
// - Parses common compact VCD value changes:
//      1!
//      xabc
//      b101010 !
//      r3.14159 %
//
// Typical use:
//
//    QFile file(path);
//    file.open(QIODevice::ReadOnly | QIODevice::Text);
//
//    QTextStream in(&file);
//    vcd::Parser parser(in);
//    vcd::VcdFile ast = parser.parse();
//

/******************************************************************************
                        Language Definition
                        -------------------
//-----------------------------------------------------------------------------
// Parser rules
//-----------------------------------------------------------------------------

value_change_dump_definitions
   : declaration_command+ simulation_command+
   ;

declaration_command
   : vcd_declaration_comment
   | vcd_declaration_date
   | vcd_declaration_scope
   | vcd_declaration_timescale
   | vcd_declaration_upscope
   | vcd_declaration_vars
   | vcd_declaration_version
   | vcd_declaration_enddefinitions
   ;

vcd_declaration_comment
   : '$comment' comment_text '$end'
   ;

vcd_declaration_date
   : '$date' date_text '$end'
   ;

vcd_declaration_enddefinitions
   : '$enddefinitions' '$end'
   ;

vcd_declaration_scope
   : '$scope' scope_type scope_identifier '$end'
   ;

scope_type
   : 'begin'
   | 'fork'
   | 'function'
   | 'module'
   | 'task'
   ;

vcd_declaration_timescale
   : '$timescale' time_number time_unit '$end'
   ;

time_number
   : 1 | 10 | 100
   ;

time_unit
   : 's' | 'ms' | 'us' | 'ns' | 'ps' | 'fs'
   ;

vcd_declaration_upscope
   : '$upscope' '$end'
   ;

vcd_declaration_vars
   : '$var' var_type size identifier_code reference '$end'
   ;

var_type
   : 'event' | 'integer' | 'parameter' | 'real'   | 'realtime' | 'reg'  | 'supply0' | 'supply1' | 'time'
   | 'tri'   | 'triand'  | 'trior'     | 'trireg' | 'tri0'     | 'tri1' | 'wand'    | 'wire'    | 'wor'
   ;

size
   : decimal_number
   ;

reference
   : identifier
   | identifier '[' bit_select_index ']'
   | identifier '[' msb_index ':' lsb_index ']'
   ;

index
   : decimal_number
   ;

vcd_declaration_version
   : '$version' text_string '$end'
   ;

simulation_command
   : vcd_simulation_dumpall
   | vcd_simulation_dumpoff
   | vcd_simulation_dumpon
   | vcd_simulation_dumpvars
   | '$comment' comment_text? '$end'
   | simulation_time
   | value_change
   ;

vcd_simulation_dumpall
   : '$dumpall' value_change+ '$end'
   ;

vcd_simulation_dumpoff
   : '$dumpoff' value_change+ '$end'
   ;

vcd_simulation_dumpon
   : '$dumpon' value_change+ '$end'
   ;

vcd_simulation_dumpvars
   : '$dumpvars' value_change+ '$end'
   ;

simulation_time
   : '#' decimal_number
   ;

value_change
   : scalar_value_change
   | vector_value_change
   ;

scalar_value_change
   : value identifier_code
   ;

value
   : '0' | '1' | 'x' | 'X' | 'z' | 'Z'
   ;

vector_value_change
   : 'b' binary_number identifier_code
   | 'B' binary_number identifier_code
   | 'r' real_number identifier_code
   | 'R' real_number identifier_code
   ;

identifier_code
   : ASCII_character+
   ;
*/

#pragma once

#include "waveform_model.h"
#include "message.h"

#include <QChar>
#include <QException>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QStack>

struct SignalBuildState
{
   bool                 hasValue = false;
   WaveSignal*          signal;
   QString              currentValue;
   qreal                currentStartTime;
};

// -----------------------------------------------------------------------------
// Parser
// -----------------------------------------------------------------------------

class VcdParser : public QObject
{
   Q_OBJECT

   public:
      explicit          VcdParser(QTextStream& in);

      std::unique_ptr<WaveformDocument>   parse();
      int               errors () {return m_errors;}
      int               warnings () {return m_warnings;}

   signals:
      void              message (MessageType type, const QString& msg, const QString& file, int line, int col);
      void              message (MessageType type, const QString& msg);

   private:
      void              parseDeclarationCommands ();
      void              parseSimulationCommands ();

      // declaration command parse functions
      void              parseComment ();
      void              parseDate ();
      void              parseScope ();
      void              parseTimescale ();
      void              parseUpscope ();
      void              parseVar ();
      void              parseVersion ();
      void              parseEndDefinition ();


      // simulation command parse functions
      void              parseDumpall();
      void              parseDumpoff();
      void              parseDumpon();
      void              parseDumpvars();
      void              parseSimulationTime();
      void              parseValueChange();
      void              parseValueChangeUntilEnd();


      // low-level parse functions
      QString           parseTextUntilEnd();
      void              expectKeyword(const QString& keyword);
      int               parseDecimalNumber();
      quint64           parseTimeValue();
      WaveSignalType    getType (const QString& var_type);


      // tokenizer functions
      void              skipWhitespace();
      QString           parseToken();
      bool              eof();
      QChar             peek();
      QChar             get();

      // utility functions
      void              debugMessage (const QString& msg, bool noloc = false);
      void              errorMessage (const QString& msg, bool noloc = false);
      void              warningMessage (const QString& msg, bool noloc = false);
      void              infoMessage (const QString& msg, bool noloc = false);

      // status variables
      int               m_errors;
      int               m_warnings;

      // parsing variables
      QTextStream&      m_in;
      QString           m_filename;
      bool              m_in_definitions;
      bool              m_hasLookahead    = false;
      QChar             m_lookahead;
      int               m_offset          = 0;
      int               m_line            = 1;
      int               m_column          = 1;

      // document processing variables
      std::unique_ptr<WaveformDocument>   m_document;
      QStack<DesignNode*>                 m_scopeStack;
      qreal                               m_currentTime     = 0.0;
      qreal                               m_timescaleFactor = 1.0;
      QHash<QString, int>                 m_idToIndex;
      QVector<SignalBuildState>           m_signals;
};
