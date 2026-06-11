#include "command_line_parser.h"
#include "message.h"

#include <QFile>
#include <QFileInfo>

using namespace commonLib;

CommandLineParser::CommandLineParser () : QObject() {}
CommandLineParser::~CommandLineParser () {}
//-----------------------------------------------------------------------------
QList<CommandLineParser::lexeme> CommandLineParser::tokenizeFile (const QString& filename)
{
   QFile f(filename);
   if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
      MESSAGE(SysError, QString("Cannot open file '%1' for reading").arg(filename));
      return {{tError, QString(), 0, 0}};
   }
   QTextStream stream(&f);

   QList<lexeme> lexemes = tokenizeOpts(stream.readAll());
   lexemes.last().value = filename;
   return lexemes;
}
QList<CommandLineParser::lexeme> CommandLineParser::tokenizeOpts (const QString& opts)
{
   // toeknize states
   typedef enum {
      Text,
      Opt,
      Id,
      Path,
      Num,
      Fraction,
      Exponent,
      Sstring,
      Dstring,
      Comment
   } states;

   // tokenize variables
   QString text;
   quint32 sline;
   quint32 scol;
   states  state = Text;

   // tokenize
   m_lexemes.clear();
   m_size = opts.size();
   m_index = 0;
   m_opts = opts;
   next_char();      // load first character
   sline = m_line;
   scol = m_col;
   while (!m_char.isNull()) {
      switch (state)
      {
         case Text      :
         {
            if (m_char == '-' && m_nextc.isLetter()) {     // short option
               sline = m_line;
               scol = m_col;
               next_char();
               m_lexemes.append({token::tOpt, QString(m_char), sline, scol});
            }
            else if (m_char == '-' && m_nextc == '-') {    // long option
               sline = m_line;
               scol  = m_col;
               next_char();      // consume first '-'
               state = Opt;
            }
            else if (m_char == '"') {
               sline = m_line;
               scol  = m_col;
               state = Dstring;
            }
            else if (m_char == '\'') {
               sline = m_line;
               scol  = m_col;
               state = Sstring;
            }
            else if (m_char == '#') {    // start of comment
               state = Comment;
            }
            else if (m_char.isDigit()) {
               sline = m_line;
               scol  = m_col;
               state = Num;
            }
            else if (m_char.isLetter()) {
               text.append(m_char);
               sline = m_line;
               scol  = m_col;
               state = Id;
            }
            else if (!m_char.isSpace())    // skip whitespace
               m_lexemes.append({static_cast<quint32>(m_char.toLatin1()), QString(m_char), m_line, m_col});

            next_char();
            break;
         }

         case Opt          :
         {
            if ((text.isEmpty() && m_char.isLetter()) || (!text.isEmpty() && (m_char.isLetterOrNumber() || m_char == '_'))) {   // start of option
               text.append(m_char);
               next_char();
            }
            else {        // end of opt
               m_lexemes.append({token::tOpt, text, sline, scol});
               text.clear();
               state = Text;
            }

            break;
         }

         case Id        :
         {
            if (m_char.isLetterOrNumber() || m_char == '_') {
               text.append(m_char);
               next_char();
            }
            else if (m_char == ':' || m_char == '/' || m_char == '.') {
               text.append(m_char);
               next_char();
               state = Path;
            }
            else {            // end of value
               m_lexemes.append({token::tId, text, sline, scol});
               text.clear();
               state = Text;
            }

            break;
         }

         case Path      :
         {
            if (m_char.isSpace()) {
               m_lexemes.append({token::tPath, text, sline, scol});
               text.clear();
               state = Text;
            }
            else {
               text.append(m_char);
               next_char();
            }
            break;
         }

         case Num       :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else if (m_char == '.') {
               text.append(m_char);
               next_char();
               state = Fraction;
            }
            else {         // end of dec number
               m_lexemes.append({token::tNumber, text, sline, scol});
               text.clear();
               state = Text;
            }
            break;
         }

         case Fraction  :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else if (m_char.toLower() == 'e') {
               text.append(m_char);
               next_char();
               if (m_char == '-' || m_char == '+') {
                  text.append(m_char);
                  next_char();
               }
               if (m_char.isDigit())
                  state = Exponent;
               else {            // error - return what we have
                  m_lexemes.append({token::tNumber, text, sline, scol});
                  sline = m_line;
                  scol  = m_col;
                  state = Text;
               }
            }
            else {            // end of fraction
               m_lexemes.append({token::tNumber, text, sline, scol});
               text.clear();
               state = Text;
            }
            break;
         }

         case Exponent  :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else {
               m_lexemes.append({token::tNumber, text, sline, scol});
               text.clear();
               state = Text;
            }
            break;
         }

         case Sstring   :
         {
            if (m_char == '\'') {
               m_lexemes.append({token::tString, text, sline, scol});
               text.clear();
               state = Text;
            }
            else text.append(m_char);

            next_char();
            break;
         }

         case Dstring   :
         {
            if (m_char == '"') {
               m_lexemes.append({token::tString, text, sline, scol});
               text.clear();
               state = Text;
            }
            else text.append(m_char);

            next_char();
            break;
         }

         case Comment   :
         {
            if (m_char == '\n')
               state = Text;

            next_char();
            break;
         }
      }
   }
   if (!text.isEmpty()) {
      switch (state)
      {
         case Opt       : m_lexemes.append({token::tOpt, text, sline, scol}); break;
         case Id        : m_lexemes.append({token::tId, text, sline, scol}); break;
         case Num       :
         case Fraction  :
         case Exponent  : m_lexemes.append({token::tNumber, text, sline, scol}); break;
         case Dstring   :
         case Sstring   : m_lexemes.append({token::tString, text, sline, scol}); break;
         default        : break;
      }
   }

   m_lexemes.append({token::tEnd, QString(), m_line, m_col});

   return m_lexemes;
}
QString CommandLineParser::token_name (int token)
{
   if (token < 128) return QString("'%1'").arg(QChar(token));
   else {
      switch (token)
      {
         case tOpt      : return QStringLiteral("<option>");
         case tId       : return QStringLiteral("<id>");
         case tString   : return QStringLiteral("<string>");
         case tNumber   : return QStringLiteral("<number>");
         case tError    : return QStringLiteral("<error>");
         case tEnd      : return QStringLiteral("<end>");
      }
      return "<unknown>";
   }
}

//-----------------------------------------------------------------------------
void CommandLineParser::next_char ()
{
   m_prevc = m_char;

   if (m_char.isNull()) {      // starting
      if (m_index < m_size) m_char = m_opts[m_index++];
      if (m_index < m_size) m_nextc = m_opts[m_index++];
   }
   else {
      m_char = m_nextc;
      if (m_index < m_size) m_nextc = m_opts[m_index++];
      else m_nextc = QChar();  // null char
   }

   if (m_char == '\n') {
      m_line++;
      m_col = 1;
   }
   else
      m_col++;
}

