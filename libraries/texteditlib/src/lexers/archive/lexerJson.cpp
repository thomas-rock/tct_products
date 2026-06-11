#include "lexerJson.h"
#include <QRegularExpression>
#include <Qsci/qsciscintilla.h>

#include <QDebug>

using namespace editorLib;

enum Styles
{
   Default     = 0,
   Keyword,
   Number,
   String
};

lexerJson::lexerJson(QObject* parent)
   : QsciLexerCustom(parent)
{
}

lexerJson::~lexerJson()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerJson::language() const
{
   return "Json";
}

// description
QString lexerJson::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
      case Keyword   : return "Keyword";
      case Number    : return "Number";
      case String    : return "String";
    }
    return QString();
}

//-----------------------------------------------------------------------------
// styleText
void lexerJson::styleText(int start, int end)
{
   QsciScintilla* editor = QsciLexer::editor();
   if(editor == nullptr)
      return;

   if (end == editor->length()) end--;     // crashes if try to get text at editor()->length() position.

   char* data = new char[end - start + 1];
   editor->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
   _source = QString(data);
   delete [] data;
   if(_source.isEmpty())
      return;

   // make everything default style
   startStyling(start);
   setStyling(end-start+1, Default);

   // style special things
   style(_keywords, start, Keyword);
   style(_numbers, start, Number);
   style(_strings, start, String);

   // folding
   // folding goes here....
}


//-----------------------------------------------------------------------------
// defaultColor
QColor lexerJson::defaultColor (int style) const
{
   switch(style) {
      case Keyword   : return Qt::blue;
      case Number    : return QColor("#ff8000");
      case String    : return QColor("#0080C0");
    }

    return Qt::black;
}

// defaultFont
QFont lexerJson::defaultFont (int style) const
{
   Q_UNUSED(style)

   QFont font("Courier New", 10);
   return font;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// style
void lexerJson::style(QList<QRegularExpression*> patterns, int start, int style)
{
   foreach (QRegularExpression* re, patterns)
   {
      QRegularExpressionMatchIterator i = re->globalMatch(_source);
      while (i.hasNext())
      {
         QRegularExpressionMatch match = i.next();
         startStyling(start + match.capturedStart());
         setStyling(match.capturedLength(), style);
      }
   }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QList<QRegularExpression*> lexerJson::_keywords
{
   new QRegularExpression("\\bvar\\b"),
   new QRegularExpression("\\bfalse\\b"),
   new QRegularExpression("\\bnull\\b"),
   new QRegularExpression("\\btrue\\b")
};

QList<QRegularExpression*> lexerJson::_numbers
{
   new QRegularExpression("\\b0b[01_xX]+\\b"),
   new QRegularExpression("\\b0o[0-7_xX]+\\b"),
   new QRegularExpression("\\b0x[0-9A-Fa-f_xX]+\\b"),
   new QRegularExpression("(?<!\\$)\\b[0-9]+\\b"),
   new QRegularExpression("\\b[0-9]*\\.[0-9]+\\b")
};

QList<QRegularExpression*> lexerJson::_strings
{
    new QRegularExpression("(?<!\')\".*\"", QRegularExpression::InvertedGreedinessOption),
    new QRegularExpression("(?<!\")\'.*\'", QRegularExpression::InvertedGreedinessOption)
};

