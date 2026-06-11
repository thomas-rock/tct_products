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

/******************************************************************************
                        Language Definition
                        -------------------
//-----------------------------------------------------------------------------
// Parser rules
//-----------------------------------------------------------------------------
start
   : (element | complex_tag)+
   ;

element
   : TEXT
   | tag
   | eval
   ;

complex_tag
   : CTAG_START stmts+ CTAG_END
   ;

stmts
   : for_stmt
   | if_stmt
   | include_stmt
   | message_stmt
   | print_stmt
   | set_stmt
   | while_stmt
   ;

for_stmt
   : FOR ID IN expr (DOT_DOT expr)? stmts+ ENDFOR
   ;

if_stmt
   : IF expr stmts+ (ELSIF expr stmts+)* (ELSE stmts+) ENDIF
   ;

include_stmt
   : INCLUDE expr (WITH assign_list)?
   ;

message_stmt
   : message DSTRING (, expr)*
   | message SSTRING
   ;

print_stmt
   : PRINT DSTRING (, expr)*
   | PRINT SSTRING

set_stmt
   : SET assign_list
   ;

while_stmt
   : WHILE expr stmts+ ENDWHILE
   ;

tag
   : align_tag
   | for_tag
   | if_tag
   | include_tag
   | message_tag
   | print_tag
   | set_tag
   | template_tag
   | while_tag
   ;

eval
   : EVAL_START expr EVAL_END
   ;

// tags
align_tag
   : TAG_START ALIGN expr TAG_END

for_tag
   : TAG_START FOR ID IN expr (DOT_DOT expr)? TAG_END element+ TAG_START ENDFOR TAG_END
   ;

if_tag
   :  TAG_START IF expr TAG_END element+ (TAG_START ELSIF expr TAG_END element+)* (TAG_START ELSE TAG_END element+)? TAG_START ENDIF TAG_END
   ;
      
include_tag
   : TAG_START INCLUDE expr (WITH assign_list)? TAG_END
   ;
      
message_tag
   : TAG_START message DSTRING (, expr)* TAG_END
   | TAG_START message SSTRING TAG_END
   ;

print_tag
   : TAG_START PRINT DSTRING (, expr)* TAG_END
   | TAG_START PRINT SSTRING TAG_END
   ;

set_tag
   : TAG_START SET assign_list TAG_END
   ;

template_tag
   : TAG_START TEMPLATE (DSTRING | SSTRING) TAG_END
   ;
   
while_tag
   : TAG_START WHILE expr TAG_END element+ TAG_START ENDWHILE TAG_END
   ;

//-----------------------------------------------------------------------------
assign_list
   : ID '=' struct (',' ID '=' struct)*
   ;

struct
   : expr
   | array
   | map
   ;

array
   : '[' (struct (',' struct)*)? ']'

map
   : '{' ( expr ':' struct (',' expr ':' struct_elem)*)? '}'
   ;
   
expr
   : primary_expr (binop primary_expr)*
   ;

primary_expr
   : id
   | sysid
   | number
   | string
   | REGEX
   | boolean
   | NULL
   | func
   | '(' expr ')'
   | unop primary_expr
   ;

message
   : INFO
   | DEBUG
   | STATUS
   | WARNING
   | ERROR
   | FATAL
   ;

string
  : SSTRING
  | DSTRING
  ;

id
   : ID ('.' field)*
   ;

field
   : ID
   | INTEGER
   | '(' expr ')'
   ;

number
   : integer
   | REAL
   ;

integer
   : HEX
   | DEC
   | OCT
   | BIN
   ;

boolean
   : TRUE
   | FALSE
   ;

func
   : ID '(' (struct (',' struct)*)? ')'

// -------- for info, only ------------
func_name
   : APPEND       // array, string
   | CAMEL
   | CAP
   | CONTAINS     // array (returns index), map, string (returns index)
   | DATE
   | DQUOTE       // string
   | FIND         // find(context, key)
   | FIRST        // array, map
   | HTML         // string
   | INSERT       // map
   | ISARRAY
   | ISINT
   | ISMAP
   | ISREAL
   | ISSTRING
   | JOIN         // array
   | KEYS         // map
   | LAST         // array, map
   | LEFT         // string
   | LENGTH
   | LINE
   | LOWER
   | MID          // string
   | PREPEND      // array, string
   | RANDOM
   | REMOVE       // array, map
   | RIGHT        // string
   | SELECT
   | SIZE         // array, map, string
   | SPLIT        // string
   | TIME
   | SQUOTE       // string
   | UNQUOTE      // string
   | UPPER
   | WORDS        // string

   // math functions for integer and real
   : ABS
   | ACOS
   | ASIN
   | ATAN
   | COS
   | EXP
   | LN
   | MAX
   | MIN
   | ROUND
   | SIN
   | SQRT
   | TAN
   ;
//----------------

binop : '*' | '/' | '+' | '-' | '&' | '|' | '^' | '<<' | '>>' | '==' | '<=' | '>=' | '<' | '>' | '=~' ;   // standard operator precedence
unop  : '-' | '~' ;

//-----------------------------------------------------------------------------
// Lexer rules
//-----------------------------------------------------------------------------
TAG_START      : '{%' ;
TAG_END        : '%}' ;
EVAL_START     : '{{' ;
EVAL_END       : '}}' ;
CTAG_START     : '<%' ;
CTAG_END       : '%>' ;

// tag keywords
ALIGN          : 'align' ;
ELSE           : 'else' ;
ELSIF          : 'elsif' ;
ENDFOR         : 'endfor' ;
ENDIF          : 'endif' ;
ENDWHILE       : 'endwhile' ;
FOR            : 'for' ;
IF             : 'if' ;
IN             : 'in' ;
INCLUDE        : 'include' ;
LOAD           : 'load' ;
MESSAGE        : 'message' ;
OPTION         : 'option' ;
SET            : 'set' ;
TEMPLATE       : 'template' ;
WHILE          : 'while' ;
WITH           : 'with' ;

// function keywords
APPEND         : 'append' ;
CAMEL          : 'camel' ;
CAP            : 'cap' ;
CONTAINS       : 'contains' ;
DATE           : 'date' ;
DQUOTE         : 'dquote' ;
FIND           : 'find' ;
FIRST          : 'first' ;
HTML           : 'html' ;
INSERT         : 'insert' ;
ISARRAY        : 'isarray' ;
ISBOOL         : 'isbool' ;
ISINT          : 'isint' ;
ISMAP          : 'ismap' ;
ISNULL         : 'ismap' ;
ISREAL         : 'isreal' ;
ISSTRING       : 'isstring' ;
JOIN           : 'join' ;
KEYS           : 'keys' ;
LAST           : 'last' ;
LINE           : 'line' ;
LOWER          : 'lower' ;
PREPEND        : 'prepend' ;
RANDOM         : 'random' ;
SELECT         : 'select' ;
SIZE           : 'size' ;
SPLIT          : 'split' ;
SQUOTE         : 'squote' ;
TIME           : 'time' ;
UNQUOTE        : 'unquote' ;
UPPER          : 'upper' ;

// math functions for integer and real
ABS            : 'abs' ;
ACOS           : 'acos' ;
ASIN           : 'asin' ;
ATAN           : 'atan' ;
COS            : 'cos' ;
EXP            : 'exp' ;
LN             : 'ln' ;
MAX            : 'max' ;
MIN            : 'min' ;
ROUND          : 'round' ;
SIN            : 'sin' ;
SQRT           : 'sqrt' ;
TAN            : 'tan' ;

// helper rules
DOT_DOT        : '..' ;
               
ID             : [A-Za-z_][A-Za-z0-9_]* ;

STRING         : '"' (ESC|.)*? '"'
               | '\'' (ESC|.)*? '\''
               ;

REGEX          : '/' .*? '/'
               
fragment ESC   : '\\"' | | '\\\'' | '\\\\' ;

// numbers
BIN   : '0b' [01_]+ ;
OCT   : '0o' [0-7_]+ ;
DEC   : [0-9]+ ;
HEX   : '0x' [0-9a-fA-F_]+ ;
REAL  :  [0-9]* '.'? [0-9]+ ([eE] [-+]? [0-9]+)? ;

TEXT           : ~[{}]+ ;

MLCOMMENT   : '{#' .*? '#}'

******************************************************************************/
#pragma once

#include <QObject>
#include <QTextStream>
#include <QStack>
#include "artilib_global.h"
#include "arti_model.h"
#include "message.h"

namespace artiLib
{
   class ARTILIB_EXPORT Parser : public QObject
   {
      Q_OBJECT

      protected:
         typedef struct {
            quint32     token;
            QString     value;
            quint32     line;
            quint32     col;
         } Lexeme;

      public:
                                 Parser ();

         ArtiDataPtr             parse (const QString& filename);
         bool                    status () const;

      Q_SIGNALS:
         void                    message (MessageType type, const QString& msg, const QString& file = {}, int line = -1, int col = -1);

      protected:
         // parser functions
         ArtiNodePtr             parse_element ();

         QList<ArtiNodePtr>      parse_ctag ();
         ArtiNodePtr             parse_stmts ();
         ArtiNodePtr             parse_for_stmt ();
         ArtiNodePtr             parse_if_stmt ();
         ArtiNodePtr             parse_include_stmt ();
         ArtiNodePtr             parse_info_stmt ();
         ArtiNodePtr             parse_debug_stmt ();
         ArtiNodePtr             parse_status_stmt ();
         ArtiNodePtr             parse_warning_stmt ();
         ArtiNodePtr             parse_error_stmt ();
         ArtiNodePtr             parse_fatal_stmt ();
         ArtiNodePtr             parse_print_stmt ();
         ArtiNodePtr             parse_set_stmt ();
         ArtiNodePtr             parse_while_stmt ();

         ArtiNodePtr             parse_text ();
         ArtiNodePtr             parse_tag ();
         ArtiNodePtr             parse_eval ();

         ArtiNodePtr             parse_align ();
         ArtiNodePtr             parse_for ();
         ArtiNodePtr             parse_if ();
         ArtiNodePtr             parse_include ();
         ArtiNodePtr             parse_load ();
         ArtiNodePtr             parse_info ();
         ArtiNodePtr             parse_debug ();
         ArtiNodePtr             parse_status ();
         ArtiNodePtr             parse_warning ();
         ArtiNodePtr             parse_error ();
         ArtiNodePtr             parse_fatal ();
         ArtiNodePtr             parse_print ();
         ArtiNodePtr             parse_set ();
         ArtiNodePtr             parse_template ();
         ArtiNodePtr             parse_while ();

         QList<ArtiNodePtr>      parse_assign_list ();
         ArtiNodePtr             parse_struct ();
         ArtiNodePtr             parse_array ();
         ArtiNodePtr             parse_map ();

         QList<ArtiNodePtr>      parse_expr_list ();
         ArtiNodePtr             parse_expr ();
         ArtiNodePtr             parse_id ();
         ArtiNodePtr             parse_sysid ();
         ArtiNodePtr             parse_number ();
         ArtiNodePtr             parse_integer ();
         ArtiNodePtr             parse_func ();
         QList<ArtiNodePtr>      parse_string ();

         void                    do_expr ();
         void                    do_primary ();
         void                    do_op ();
         void                    push_operator (Lexeme op);
         ArtiNodePtr             mk_op_node (Lexeme op);

         Lexeme                  next_lex ();
         virtual void            consume_lex ();
         Lexeme                  look_ahead ();

         // lexer functions
         void                    tokenize ();
         void                    next_char ();
         void                    skip_ws ();
         QString                 trim_ws (QString text);

         // utility functio   ns
         QString                 token_name (quint32 token) const;
         void                    syntax_error (QStringList = {});

         void                    dump_lex ();

      private:
         // global static variables
         static const QMap<quint32, quint32> m_prec;

      // global variables
         ArtiDataPtr                   m_artiData;
         ArtiNodePtr                   m_templateNode;
         bool                          m_status = true;
         QList<Lexeme>                 m_lexemes;

         // parse variables
         QStack<Lexeme>                m_opStack;         // operator lexeme stack
         QStack<ArtiNodePtr>           m_nodeStack;       // expression node stack

         // tokenize variables
         QTextStream                   m_stream;
         QChar                         m_char;
         QChar                         m_nextc;
         QChar                         m_prevc;
         quint32                       m_line;
         quint32                       m_col;
   };
}
