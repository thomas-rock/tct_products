// wave_value_utils.h
#pragma once
#include <QString>

namespace WaveValueUtils
{
   inline QString normalizeBitValue(const QString& value)
   {
      if (value.isEmpty())
            return "X";

      const QChar c = value[0].toUpper();

      if (c == '0') return "0";
      if (c == '1') return "1";
      if (c == 'Z') return "Z";
      if (c == 'X') return "X";

      return "X";
   }

   inline QString extractBusBit(const QString& busValue, int bitIndex)
   {
      if (busValue.isEmpty())
         return "X";

      const int width = busValue.size();
      const int charIndex = (width - 1) - bitIndex;

      if (charIndex < 0 || charIndex >= width)
         return "X";

      return normalizeBitValue(QString(busValue[charIndex]));
   }
}