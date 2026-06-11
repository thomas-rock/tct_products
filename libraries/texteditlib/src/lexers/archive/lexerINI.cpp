#include "lexerINI.h"
#include <QRegularExpression>
#include <Qsci/qsciscintilla.h>

#include <QDebug>

using namespace editorLib;

enum Styles
{
   Default     = 0,
   Group,
   Key,
   Comment
};

lexerINI::lexerINI(QObject* parent)
   : QsciLexerCustom(parent)
{
}

lexerINI::~lexerINI()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerINI::language() const
{
   return "INI";
}

// description
QString lexerINI::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
      case Group     : return "Group";
      case Key       : return "Key";
      case Comment   : return "Comment";
    }
    return QString();
}

//-----------------------------------------------------------------------------
// styleText
void lexerINI::styleText(int start, int end)
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
   style(_groups, start, Group);
   style(_keys, start, Key);
   style(_comments, start, Comment);

   // folding
   // goes here....
}

//-----------------------------------------------------------------------------
// defaultColor
QColor lexerINI::defaultColor (int style) const
{
   switch(style) {
      case Group     : return Qt::blue;
      case Key       : return Qt::darkMagenta;
      case Comment   : return Qt::darkGreen;
    }

    return Qt::black;
}

// defaultFont
QFont lexerINI::defaultFont (int style) const
{
   QFont font("Courier New", 10);
   if (style == Group)
      font.setBold(true);
   else if (style == Comment)
      font.setItalic(true);
   return font;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// style
void lexerINI::style(QList<QRegularExpression*> patterns, int start, int style)
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
QList<QRegularExpression*> lexerINI::_groups
{
   new QRegularExpression("(^|\\n)\\s*\\[[\\w/]+\\]")
};

QList<QRegularExpression*> lexerINI::_keys
{
   new QRegularExpression("(^|\\n)\\s*\\w+\\s*(?=\\=)")
};

QList<QRegularExpression*> lexerINI::_comments
{
   new QRegularExpression("(^|\\n)\\s*;[^\\n]*")
};


