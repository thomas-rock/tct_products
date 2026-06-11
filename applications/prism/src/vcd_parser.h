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
   | vcd_declaration_enddefinitions
   | vcd_declaration_scope
   | vcd_declaration_timescale
   | vcd_declaration_upscope
   | vcd_declaration_vars
   | vcd_declaration_version
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

#include <QChar>
#include <QException>
#include <QString>
#include <QTextStream>
#include <QVector>

#include <stdexcept>

namespace vcd
{

   // -----------------------------------------------------------------------------
   // Model
   // -----------------------------------------------------------------------------

   enum class ScopeType
   {
      Begin,
      Fork,
      Function,
      Module,
      Task
   };

   enum class VarType
   {
      Event,
      Integer,
      Parameter,
      Real,
      Realtime,
      Reg,
      Supply0,
      Supply1,
      Time,
      Tri,
      TriAnd,
      TriOr,
      TriReg,
      Tri0,
      Tri1,
      WAnd,
      Wire,
      WOr
   };

   enum class TimeUnit
   {
      S,
      Ms,
      Us,
      Ns,
      Ps,
      Fs
   };

   struct SourceLocation
   {
      qsizetype offset = 0;
      qsizetype line   = 1;
      qsizetype column = 1;
   };

   class ParseError : public std::runtime_error
   {
      public:
         SourceLocation    loc;

                           ParseError(SourceLocation where, const QString& msg);

         static QString    format(SourceLocation where, const QString& msg);
   };

   struct Reference
   {
      QString name;
      bool    hasRange = false;
      int     msb      = 0;
      int     lsb      = 0;

      bool isBitSelect() const { return hasRange && msb == lsb; }
   };

   struct ScopeDecl
   {
      ScopeType type;
      QString   identifier;
   };

   struct VarDecl
   {
      VarType   type;
      int       size = 0;
      QString   idCode;
      Reference reference;
   };

   struct TimescaleDecl
   {
      int      number = 1;
      TimeUnit unit   = TimeUnit::Ns;
   };

   struct Declaration
   {
      enum class Kind
      {
         Comment,
         Date,
         EndDefinitions,
         Scope,
         Timescale,
         Upscope,
         Var,
         Version
      };

      Kind          kind = Kind::Comment;
      QString       text;
      ScopeDecl     scope;
      TimescaleDecl timescale;
      VarDecl       var;
   };

   struct ValueChange
   {
      QChar             radix;      // b/B/r/R
      QString           value;
      QString           idCode;
   };

   struct SimulationCommand
   {
      enum class Kind
      {
         DumpAll,
         DumpOff,
         DumpOn,
         DumpVars,
         Comment,
         Time,
         ValueChange
      };

      Kind                 kind = Kind::Comment;
      QString              comment;
      quint64              time = 0;
      ValueChange          valueChange;
      QVector<ValueChange> dumpChanges;
   };

   // -----------------------------------------------------------------------------
   // Parser
   // -----------------------------------------------------------------------------

   class VcdParser
   {
      public:
         explicit          VcdParser(QTextStream& in);

         std::unique_ptr<WaveformDocument>   parse();

      private:
         void              processDirectiveLine(const QString& line);
         void              processValueChangeLine(const QString& line);


      SourceLocation    loc() const;
         [[noreturn]] void fail(const QString& msg) const;

         static bool       isWhitespace(QChar c);
         static bool       isDigit(QChar c);
         bool              eof();
         QChar             peek();
         QChar             get();
         void              skipWhitespace();
         QString           parseToken();
         QString           readRawTokenFromCurrentPosition();
         bool              nextTokenIs(const QString& keyword);
         void              expectKeyword(const QString& keyword);
         void              expectChar(QChar expected);
         int               parseDecimalNumber();
         quint64           parseUnsigned64AfterHash();

         // Reads text until a standalone $end token. The $end token is consumed.
         QString           parseTextUntilEnd();
         static void       trimRight(QString& s);
         static void       trimLeft(QString& s);
         bool              isDeclarationCommandToken(const QString& token) const;
         bool              isSimulationCommandToken(const QString& token) const;
         bool              isDeclarationCommandStart();
         bool              isSimulationCommandStart();
         bool              startsWithAnyKeyword(const QVector<QString>& keywords);

         QString           peekToken();
         QString           parseTokenFromPeekBufferOrInput();
         QString           parseTokenRawNoPeekToken();

         static bool       isValueChangeStart(QChar c);
         Declaration       parseDeclarationCommand();
         Declaration       parseDeclarationCommentAfterKeyword();
         Declaration       parseDeclarationDateAfterKeyword();
         Declaration       parseDeclarationEndDefinitionsAfterKeyword();
         Declaration       parseDeclarationScopeAfterKeyword();
         ScopeType         parseScopeType();
         Declaration       parseDeclarationTimescaleAfterKeyword();
         int               parseTimeNumber();
         TimeUnit          parseTimeUnit();
         Declaration       parseDeclarationUpscopeAfterKeyword();
         Declaration       parseDeclarationVarAfterKeyword();
         VarType           parseVarType();
         Reference         parseReferenceUntilEnd();
         Declaration       parseDeclarationVersionAfterKeyword();
         SimulationCommand parseSimulationCommand();
         SimulationCommand parseDumpCommand(SimulationCommand::Kind kind);
         SimulationCommand parseSimulationCommentAfterKeyword();
         SimulationCommand parseSimulationTime();
         SimulationCommand parseSimulationValueChange();
         ValueChange       parseValueChange();
         ValueChange       parseScalarValueChange();
         ValueChange       parseVectorOrRealValueChange();

         QTextStream&      m_in;

         bool              m_hasLookahead    = false;
         QChar             m_lookahead;
         qsizetype         m_offset          = 0;
         qsizetype         m_line            = 1;
         qsizetype         m_column          = 1;

         QString           m_peekedToken;
         bool              m_hasPeekedToken  = false;

         std::unique_ptr<WaveformDocument>      m_document;
         QStringList                            m_scopeStack;
         qreal                                  m_currentTime= 0.0;
         qreal                                  m_lastTime= 0.0;
         QHash<QString, VarDecl>                m_varsById;
         QHash<QString, QVector<ParsedChange>>  m_changesById;
         QHash<QString, DesignNode*>            m_designNodeByPath;

         qreal                                  m_timescaleFactor = 1.0; // converts VCD time ? nanoseconds

   };

   // -----------------------------------------------------------------------------
   // Small helpers
   // -----------------------------------------------------------------------------

   inline QString toString(TimeUnit unit);
   inline QString toString(ScopeType type);
   inline QString toString(VarType type);

} // namespace vcd

