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
#include "options_parser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

using namespace commonLib;

OptionsParser::OptionsParser() : QObject(), m_transcript{false} {}
//-----------------------------------------------------------------------------
OptionsParser::Status OptionsParser::parse (const QString& text)
{
   CommandLineParser clp;
   connect(&clp, SIGNAL(message(int, QString)), this, SIGNAL(message(int,QString)));
   m_lexemes = clp.tokenizeOpts(text);
   m_status = Ok;
   m_fileStack.push(QString());
   m_quiet.clear();
   m_preview = false;
   m_transcript = false;
   m_rtf = false;
   m_plaintext = true;         // default
   m_markdown = false;
   parse_command_line();
   return m_status;
}
QStringList OptionsParser::incpaths () const {return m_incpaths;}
QMap<QString, QString> OptionsParser::context () const {return m_context;}
QString OptionsParser::outfile () const {return m_outfile;}
bool OptionsParser::preview () const {return m_preview;}
bool OptionsParser::rtf () const {return m_rtf;}
bool OptionsParser::plaintext () const {return m_plaintext;}
bool OptionsParser::markdown () const {return m_markdown;}
bool OptionsParser::transcript () const {return m_transcript;}
QString OptionsParser::quiet () const {return m_quiet;}
QString OptionsParser::initial () const {return m_initial;}

//-----------------------------------------------------------------------------
void OptionsParser::parse_command_line ()
{
   CommandLineParser clp;

   while (m_status == Ok && next_lex().token != CommandLineParser::tEnd && next_lex().token != CommandLineParser::tError) {
      if      (next_lex().token == CommandLineParser::tOpt) parse_option();
      else if (next_lex().token == CommandLineParser::tId || next_lex().token == CommandLineParser::tPath || next_lex().token == CommandLineParser::tString) {
         m_initial = next_lex().value;
         consume_lex();
         if (next_lex().token != CommandLineParser::tEnd || !next_lex().value.isEmpty()) syntax_error({"<end>"});
      }
      else if (next_lex().token == CommandLineParser::tEnd && !next_lex().value.isEmpty()) {
         m_fileStack.pop();
         consume_lex();
      }
      else syntax_error({"<option>", "<initial template>"});
   }
}
void OptionsParser::parse_option ()
{
   QString opt = next_lex().value;
   consume_lex();
   if      (opt == "c" || opt == "context")     parse_context();
   else if (opt == "f" || opt == "file")        parse_file();
   else if (opt == "?" || opt == "help")        parse_help();
   else if (opt == "h" || opt == "hdlpath")     parse_hdlpath();
   else if (opt == "i" || opt == "incpath")     parse_incpath();
   else if (opt == "m" || opt == "markdown")    parse_markdown();
   else if (opt == "o" || opt == "outfile")     parse_outfile();
   else if (opt == "p" || opt == "preview")     parse_preview();
   else if (opt == "r" || opt == "rtf")         parse_rtf();
   else if (opt == "t" || opt == "text")        parse_plaintext();
   else if (opt == "T" || opt == "transcript")  parse_transcript();
   else if (opt == "v" || opt == "quiet")       parse_quiet();
   else if (opt == "V" || opt == "version")     parse_version();
   else syntax_error({"-c", "-f", "-h", "-i", "-o", "-q", "-s", "-t", "-v", "-V"});
}
void OptionsParser::parse_context ()
{
   if (next_lex().token == CommandLineParser::tId) {
      QString name = next_lex().value;
      consume_lex();
      if (next_lex().token == '=') {
         consume_lex();
         QString value = parse_value();
         m_context.insert(name, value);
      }
      else syntax_error({"'='"});
   }
   else syntax_error({"<id>"});
}
void OptionsParser::parse_help ()
{
   help();
   m_status = Help;
}
void OptionsParser::parse_file ()
{
   CommandLineParser clp;
   connect(&clp, SIGNAL(message(int, QString)), this, SIGNAL(message(int,QString)));
   QString file = parse_value();
   if (m_status) {
      m_lexemes = clp.tokenizeOpts(file) + m_lexemes;
      m_fileStack.push(file);
   }
}
void OptionsParser::parse_hdlpath ()    {m_hdlpaths.append(parse_value());}
void OptionsParser::parse_incpath ()    {m_incpaths.append(parse_value());}
void OptionsParser::parse_outfile ()    {m_outfile = parse_value();}
void OptionsParser::parse_preview ()    {m_preview = true;}
void OptionsParser::parse_rtf ()        {m_rtf = true;  m_plaintext = false; m_markdown = false;}
void OptionsParser::parse_plaintext ()  {m_rtf = false; m_plaintext = true;  m_markdown = false;}
void OptionsParser::parse_markdown ()   {m_rtf = false; m_plaintext = false; m_markdown = true;}
void OptionsParser::parse_transcript () {m_transcript = true;}
void OptionsParser::parse_version ()    {version(); m_status = Help;}
void OptionsParser::parse_quiet ()
{
   static QSet<QString> opts = {"i", "d", "s", "w", "e", "f"};
   m_quiet = parse_value();
   if (m_quiet == "all") {
      m_quiet = "idswef";
       return;
   }
   QStringList chars = m_quiet.split("");
   chars.pop_back();
   chars.pop_front();
   foreach (QString c, chars) {
      if (!opts.contains(c)) {
         syntax_error({"Combinations of 'idswef'", "all"});
         break;
      }
   }
}
QString OptionsParser::parse_value ()
{
   if (next_lex().token == CommandLineParser::tId ||
       next_lex().token == CommandLineParser::tString ||
       next_lex().token == CommandLineParser::tNumber) {
         QString value = next_lex().value;
         consume_lex();
         return value;
   }
   syntax_error({"<id>", "<string>", "<number>"});
   return QString();
}

//-----------------------------------------------------------------------------
CommandLineParser::lexeme OptionsParser::next_lex () {return m_lexemes[0];}
void OptionsParser::consume_lex () {m_lexemes.pop_front();}
CommandLineParser::lexeme OptionsParser::look_ahead() {return m_lexemes[1];}

//-----------------------------------------------------------------------------
void OptionsParser::version () {MESSAGE(SysInfo, QString("Arti version %1").arg("0.0.0"));}
void OptionsParser::help ()
{
   static const QString msg =
      "arti [options] <initial template>\n"
      "\n"
      "Options:\n"
      "   -c, --context <name[=value]> Specify a name/value pair to be added to the context. May be specified multiple times.\n"
      "    ?, --help                   Displays help on commandline options.\n"
      "   -f. --file <name>            Include options specified in file <name>. May be specified multiple times.\n"
      "   -h, --hdlpath <path>         Path to search for included hdl files. May be specified multiple times.\n"
      "   -i, --incpath <path>         Path to search for included template files. May be specified multiple times.\n"
      "   -m, --markdown               Document is a markdown document. Default is plain text document.\n"
      "   -o, --outfile <filename>     Output file name. If omitted, output will sent to stdout.\n"
      "   -q, --quiet                  Suppress informational messages.\n"
      "   -p, --preview                Display generated artifact in preview window.\n"
      "   -r, --rtf                    Document is an rtf (html) document. Default is plain text document.\n"
      "   -t, --text                   Document is a plaintext document.\n"
      "   -T, --transcript             Output generated artifact text to transcript.\n"
      "   -V, --version                Displays current version and exits.\n"
      "\n"
      "initial template                Name of initial arti template.\n"
      ;

      MESSAGE(SysInfo, msg);
}
void OptionsParser::syntax_error (QStringList exp)
{
   if (next_lex().token != CommandLineParser::tError) {  // failsafe
      QString expected;
      if (exp.size())
         expected = QString("Expected: %1").arg(exp.join(", "));

      MESSAGE(SysError, QString("Unexpected token %1 (%2), %3").arg(next_lex().value).arg(CommandLineParser::token_name(next_lex().token)).arg(expected), m_fileStack.last(), next_lex().line, next_lex().col);
   }
   m_status = Error;
}
