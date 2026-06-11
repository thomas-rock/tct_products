#pragma once

#include "commonlib_global.h"
#include <QWidget>
#include <QSettings>

namespace commonLib
{
   namespace Ui { class MessagePreferences; }

   class COMMONLIB_EXPORT MessagePreferences : public QWidget
   {
      Q_OBJECT

      public:
                                 MessagePreferences (QWidget* parent = nullptr);
                                 ~MessagePreferences () override;

         void                    load (QSettings& settings);
         void                    save (QSettings& settings);

      public Q_SLOTS:
         void                    setDefaults ();

      private:
         Ui::MessagePreferences* m_ui;
   };
}
