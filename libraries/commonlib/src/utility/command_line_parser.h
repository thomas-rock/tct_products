#pragma once

#include "commonlib_global.h"
#include <QString>
#include <QList>
#include <QObject>

namespace commonLib
{
   class COMMONLIB_EXPORT CommandLineParser : public QObject
   {
      Q_OBJECT

      public:
         typedef enum : quint32
         {
            tOpt = 256,
            tId,
            tPath,
            tString,
            tNumber,
            tError,
            tEnd
         } token;

         typedef struct {
            quint32     token;
            QString     value;
            quint32     line;
            quint32     col;
         } lexeme;


      public:
         explicit          CommandLineParser();
                           ~CommandLineParser ();

         QList<lexeme>     tokenizeFile (const QString& filename);
         QList<lexeme>     tokenizeOpts (const QString& opts);
         static QString    token_name (int token);

      Q_SIGNALS:
         void              message (int type, QString msg);

      protected:
         void              next_char ();

      private:
         // command-line options lexer variables
         QList<lexeme>     m_lexemes;
         QChar             m_char;
         QChar             m_nextc;
         QChar             m_prevc;
         quint32           m_line;
         quint32           m_col;
         QString           m_opts;
         quint32           m_index;
         quint32           m_size;
   };
}
