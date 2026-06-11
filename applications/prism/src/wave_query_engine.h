#pragma once

#include "wave_query_ast.h"
#include "waveform_model.h"

#include <QHash>
#include <QSet>
#include <QString>
#include <QVector>

#include <memory>

struct WaveQueryValue
{
   bool valid = false;
   bool unknown = false;
   qint64 integer = 0;
   QString raw;

   static WaveQueryValue invalid()
   {
      return {};
   }

   static WaveQueryValue unknownValue(const QString& raw = {})
   {
      WaveQueryValue v;
      v.valid = true;
      v.unknown = true;
      v.raw = raw;
      return v;
   }

   static WaveQueryValue intValue(qint64 value, const QString& raw = {})
   {
      WaveQueryValue v;
      v.valid = true;
      v.integer = value;
      v.raw = raw;
      return v;
   }

   bool toBool() const
   {
      return valid && !unknown && integer != 0;
   }
};

class WaveQueryEngine
{
   public:
      struct BindResult
      {
         bool ok = false;
         QString error;
      };

      struct WaveQueryMatch
      {
         qreal startTime = 0.0;
         qreal endTime = 0.0;
      };

      struct WaveQuerySearchResult
      {
         bool ok = false;
         QString error;
         QVector<WaveQueryMatch> matches;
      };

      explicit                WaveQueryEngine(const WaveformDocument* document = nullptr);

      void                    setDocument(const WaveformDocument* document);

      WaveQuerySearchResult   search(const WaveQueryNode* root);

      bool                    evaluateBool(const WaveQueryNode* root, qreal time, QString* error = nullptr) const;

   private:
      BindResult              bind(const WaveQueryNode* root);
      void                    rebuildSignalMaps();

      void                    collectBoundTransitionTimes(QVector<qreal>& times) const;
      void                    addUniqueSortedTime(QVector<qreal>& times, qreal t) const;
      void                    appendMatch(QVector<WaveQueryMatch>& matches, qreal startTime, qreal endTime) const;

      void                    collectIdentifiers(const WaveQueryNode* node, QSet<QString>& ids) const;
      WaveSignal*             resolveIdentifier(const QString& id, QString* error) const;

      WaveQueryValue          evaluateValue(const WaveQueryNode* node, qreal time, QString* error) const;
      WaveQueryValue          valueFromSignal(WaveSignal* signal, qreal time) const;
      WaveQueryValue          valueFromString(const QString& value) const;

   private:
      const WaveformDocument*                m_document = nullptr;

      QHash<QString, WaveSignal*>            m_signalByFullPath;
      QHash<QString, QVector<WaveSignal*>>   m_signalByShortName;

      QHash<QString, WaveSignal*>            m_boundSignals;
};