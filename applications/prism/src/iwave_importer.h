#pragma once

#include <QString>
#include <memory>

class WaveformDocument;

class IWaveImporter
{
   public:
      virtual           ~IWaveImporter() = default;

      virtual           std::unique_ptr<WaveformDocument> importFile(const QString& path,
                                                                     QString* errorMessage = nullptr) = 0;
};