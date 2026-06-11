#pragma once

#include "QHexEdit.h"
#include <QFile>

namespace hexeditLib
{
   class editorWidget  : public QHexEdit
   {
      Q_OBJECT

      protected:
         QFile             _file;
         QString           _filename;
         bool              _pristine               = true;

      public:
                           editorWidget (QWidget* parent = nullptr);

         QString           filename ();
         void              setFilename (QString filename);
         bool              setData (QString filename, bool readonly);

         bool              saveData ();


         bool              isPristine ();
         bool              isRedoAvailable ();
         bool              isUndoAvailable ();

         qint64            currentAddress ();
         qint64            currentSize ();
         void              updatePreferences ();

         // find/replace
         qint64            find (QString text, bool hex, bool rev);
         void              replace (QString ftext, bool fhex, QString rtext, bool rhex, bool rev);
         int               replaceAll (QString ftext, bool fhex, QString rtext, bool rhex);

      protected:
         qint64            findNext (QByteArray& ba, bool rev);
         QByteArray        getContent (bool hex, const QString& input);
   };
}
