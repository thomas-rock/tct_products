#pragma once

#include "iwave_importer.h"
#include "waveform_model.h"

#include <QHash>
#include <QStringList>

class VcdImporter : public IWaveImporter
{
   public:
      std::unique_ptr<WaveformDocument>   importFile(const QString& path, QString* errorMessage = nullptr) override;

   private:

      struct ParsedVar
      {
         QString idCode;
         QString name;
         QString scopePath;
         int width = 1;
      };

      struct ParsedChange
      {
         qreal time = 0.0;
         QString value;
      };


      bool                                   parseFile(const QString& path, QString* errorMessage);

      void                                   reset();
      void                                   processDirectiveLine(const QString& line);
      void                                   processValueChangeLine(const QString& line);
      void                                   finalizeDocument();

      QString                                currentScopePath() const;
      WaveSignalKind                         signalKindForWidth(int width) const;
      QString                                normalizeScalarValue(QChar c) const;
      QString                                normalizeVectorValue(const QString& v) const;
      QString                                defaultUnknownValue(const ParsedVar& var) const;

   private:
      std::unique_ptr<WaveformDocument>      m_document;
      QStringList                            m_scopeStack;
      qreal                                  m_currentTime= 0.0;
      qreal                                  m_lastTime= 0.0;
      QHash<QString, ParsedVar>              m_varsById;
      QHash<QString, QVector<ParsedChange>>  m_changesById;
      QHash<QString, DesignNode*>            m_designNodeByPath;

      qreal                                  m_timescaleFactor = 1.0; // converts VCD time ? nanoseconds
};
