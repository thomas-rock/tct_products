// wave_value_utils.h
#pragma once
#include <QString>
#include <algorithm>

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

   inline QString extractBusBit(const QString& busValue,
                                int signalMsb,
                                int signalLsb,
                                int bitIndex)
   {
      if (busValue.isEmpty())
         return "X";

      const int lo = std::min(signalMsb, signalLsb);
      const int hi = std::max(signalMsb, signalLsb);

      if (bitIndex < lo || bitIndex > hi)
         return "X";

      const int charIndex =
         (signalMsb >= signalLsb) ? (signalMsb - bitIndex)
                                  : (bitIndex - signalMsb);

      if (charIndex < 0 || charIndex >= busValue.size())
         return "X";

      return normalizeBitValue(QString(busValue[charIndex]));
   }
}
