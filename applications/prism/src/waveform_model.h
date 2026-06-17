#pragma once

#include "wave_value_utils.h"

#include <QString>
#include <QVector>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>
#include <algorithm>

enum class WaveSignalKind
{
   Bit,
   Bus
};

struct WaveStyle
{
   QColor normalColor { QColor(0, 255, 0) };   // green
   QColor xColor      { QColor(255, 80, 80) }; // red
   QColor zColor      { QColor(0, 255, 255) }; // cyan
   QColor uColor      { QColor(80, 80, 80) };  // gray
   QColor textColor   { QColor(0, 255, 0) };
};

struct WaveSegment
{
   qreal   startTime = 0.0;
   qreal   endTime   = 0.0;
   QString value;

   WaveSegment() = default;

   WaveSegment(qreal start, qreal end, const QString& v)
      : startTime(start), endTime(end), value(v)
   {
   }

   void dump (int indent) const
   {
      qDebug().noquote() << QString("%1start: %2, end: %3, value: %4").arg(QString(indent, ' ')).arg(startTime).arg(endTime).arg(value);
   }
};

class WaveSignal
{
   public:
      QString name;       // leaf name, e.g. "clk"
      QString scopePath;  // e.g. "top.cpu"
      QString fullPath;   // e.g. "top.cpu.clk"
      int     width = 1;

      WaveSignalKind kind = WaveSignalKind::Bus;
      QVector<WaveSegment> segments;
      WaveStyle style;

      WaveSignal() = default;

      WaveSignal(const QString& n,
                 WaveSignalKind k,
                 int w = 1,
                 const QString& scope = {})
         : name(n),
           scopePath(scope),
           width(w),
           kind(k)
      {
         fullPath = (scopePath.isEmpty()) ? name : scopePath + "." + name;
      }

      QString valueAtTime(qreal time) const
      {
         const WaveSegment* seg = segmentAtTime(time);
         return seg ? seg->value : QString{};
      }

      qreal nearestEdgeTime(qreal time, qreal maxDistance = -1.0) const
      {
         const WaveSegment* seg = segmentAtTime(time);
         if (!seg)
            return time;

         const qreal startDist = std::abs(time - seg->startTime);
         const qreal endDist   = std::abs(seg->endTime - time);

         qreal bestTime = seg->endTime;
         qreal bestDist = endDist;

         if (startDist < endDist)
         {
            bestTime = seg->startTime;
            bestDist = startDist;
         }

         if (maxDistance >= 0.0 && bestDist > maxDistance)
            return time;

         return bestTime;
      }

      // do a binary search to find a segment at the given time
      const WaveSegment* segmentAtTime(qreal time) const
      {
         int lo = 0;
         int hi = segments.size() - 1;

         while (lo <= hi)
         {
            const int mid           = lo + (hi - lo) / 2;
            const WaveSegment& seg  = segments[mid];

            if (time < seg.startTime)
               hi = mid - 1;
            else if (time >= seg.endTime)
               lo = mid + 1;
            else
               return &seg;
         }
         return nullptr;
      }

      qreal endTime() const
      {
         return segments.last().endTime;
      }

      void dump (int indent = 0) const
      {
         qDebug().noquote() << QString("%1name: %2, fullPath: %3, width: %4").arg(QString(indent, ' '), name, fullPath).arg(width);
         for (const auto& segment : segments)
            segment.dump(indent);
      }
};

enum class DisplayNodeKind
{
   Group,
   Signal,
   Field,
   Bit
};

class DisplayNode
{
   public:
      QString name;
      DisplayNodeKind kind = DisplayNodeKind::Group;

      WaveSignal* signal = nullptr;

      // For decomposed views
      int msb = -1;
      int lsb = -1;

      std::vector<std::unique_ptr<DisplayNode>> children;

      DisplayNode() = default;

      explicit DisplayNode(const QString& n, DisplayNodeKind k = DisplayNodeKind::Group)
         : name(n), kind(k)
      {
      }

      DisplayNode* addChild(std::unique_ptr<DisplayNode> child)
      {
         DisplayNode* raw = child.get();
         children.push_back(std::move(child));
         return raw;
      }

      bool isLeafSignalLike() const
      {
         return kind == DisplayNodeKind::Signal || kind == DisplayNodeKind::Bit;
      }

      QString valueAtTime(qreal time) const
      {
         if (!signal)
            return {};

         const QString base = signal->valueAtTime(time);
         if (base.isEmpty())
            return {};

         switch (kind)
         {
         case DisplayNodeKind::Signal:
            return base;

         case DisplayNodeKind::Bit:
         {
            if (msb < 0 || lsb < 0 || msb != lsb)
               return {};

            if (signal->kind == WaveSignalKind::Bit)
               return WaveValueUtils::normalizeBitValue(base);

            return WaveValueUtils::extractBusBit(base, msb);
         }

         case DisplayNodeKind::Field:
         {
            if (msb < 0 || lsb < 0 || msb < lsb)
               return {};

            QString result;
            result.reserve(msb - lsb + 1);

            for (int bit = msb; bit >= lsb; --bit)
               result += WaveValueUtils::extractBusBit(base, bit);

            return result;
         }

         case DisplayNodeKind::Group:
         default:
            return {};
         }
      }

      QJsonObject toJson() const
      {
         QJsonObject obj;

         switch (kind)
         {
         case DisplayNodeKind::Group:
            obj["kind"] = "group";
            obj["name"] = name;

            {
               QJsonArray childrenArray;

               for (const auto& child : children)
               {
                  if (child)
                     childrenArray.append(child->toJson());
               }

               obj["children"] = childrenArray;
            }
            break;

         case DisplayNodeKind::Signal:
            obj["kind"] = "signal";
            obj["name"] = name;

            if (signal)
               obj["path"] = signal->fullPath;
            break;

         case DisplayNodeKind::Bit:
            obj["kind"] = "bit";
            obj["name"] = name;

            if (signal)
               obj["path"] = signal->fullPath;

            obj["bit"] = msb;
            break;

         case DisplayNodeKind::Field:
            obj["kind"] = "field";
            obj["name"] = name;

            if (signal)
               obj["path"] = signal->fullPath;

            obj["msb"] = msb;
            obj["lsb"] = lsb;

            {
               QJsonArray childrenArray;

               for (const auto& child : children)
               {
                  if (child)
                     childrenArray.append(child->toJson());
               }

               obj["children"] = childrenArray;
            }
            break;
         }

         return obj;
      }

      static std::unique_ptr<DisplayNode>
      fromJson(
         const QJsonObject& obj,
         const std::function<WaveSignal*(const QString&)>& findSignal)
      {
         const QString kindString = obj.value("kind").toString();

         if (kindString == "group")
         {
            auto node = std::make_unique<DisplayNode>(
               obj.value("name").toString("Group"),
               DisplayNodeKind::Group);

            const QJsonArray childrenArray =
               obj.value("children").toArray();

            for (const QJsonValue& value : childrenArray)
            {
               if (!value.isObject())
                  continue;

               auto child =
                  DisplayNode::fromJson(value.toObject(), findSignal);

               if (child)
                  node->addChild(std::move(child));
            }

            return node;
         }

         const QString signalPath = obj.value("path").toString();

         WaveSignal* sig = nullptr;

         if (!signalPath.isEmpty())
            sig = findSignal(signalPath);

         if (!sig)
            return nullptr;

         if (kindString == "signal")
         {
            auto node = std::make_unique<DisplayNode>(
               obj.value("name").toString(sig->name),
               DisplayNodeKind::Signal);

            node->signal = sig;

            // Rebuild bit decomposition automatically for buses.
            if (sig->width > 1)
            {
               for (int bit = sig->width - 1; bit >= 0; --bit)
               {
                  auto bitNode =
                     std::make_unique<DisplayNode>(
                        QString("[%1]").arg(bit),
                        DisplayNodeKind::Bit);

                  bitNode->signal = sig;
                  bitNode->msb = bit;
                  bitNode->lsb = bit;

                  node->addChild(std::move(bitNode));
               }
            }

            return node;
         }

         if (kindString == "bit")
         {
            const int bit = obj.value("bit").toInt(-1);

            if (bit < 0)
               return nullptr;

            QString nodeName = obj.value("name").toString();

            if (nodeName.isEmpty())
               nodeName = QString("%1[%2]").arg(sig->name).arg(bit);

            auto node = std::make_unique<DisplayNode>(
               nodeName,
               DisplayNodeKind::Bit);

            node->signal = sig;
            node->msb = bit;
            node->lsb = bit;

            return node;
         }

         if (kindString == "field")
         {
            const int fieldMsb = obj.value("msb").toInt(-1);
            const int fieldLsb = obj.value("lsb").toInt(-1);

            if (fieldMsb < 0 || fieldLsb < 0 || fieldMsb < fieldLsb)
               return nullptr;

            auto node = std::make_unique<DisplayNode>(
               obj.value("name").toString("field"),
               DisplayNodeKind::Field);

            node->signal = sig;
            node->msb = fieldMsb;
            node->lsb = fieldLsb;

            const QJsonArray childrenArray =
               obj.value("children").toArray();

            for (const QJsonValue& value : childrenArray)
            {
               if (!value.isObject())
                  continue;

               auto child =
                  DisplayNode::fromJson(value.toObject(), findSignal);

               if (child)
                  node->addChild(std::move(child));
            }

            return node;
         }

         return nullptr;
      }

};

class DesignNode
{
   public:
      QString name;
      QString fullPath;

      std::vector<DesignNode*> children;
      std::vector<WaveSignal*> signalList;

      explicit                            DesignNode(const QString& n = {}, const QString& p = {}) : name(n), fullPath(p) {}

      void                                dump (int indent = 0)
      {
         qDebug().noquote() << QString("%1name: %2, fullPath: %3").arg(QString(indent, ' '), name, fullPath);
         qDebug().noquote() << QString("%1Children:").arg(QString(indent, ' '));
         for (auto& dn : children)
            dn->dump(indent+3);
      }
};

class WaveformDocument
{
   public:
      std::vector<std::unique_ptr<WaveSignal>> signalList;
      std::vector<std::unique_ptr<DesignNode>> designNodes;
      std::vector<std::unique_ptr<DisplayNode>> displayRoots;
      std::vector<DesignNode*> hierarchyRoots;

      WaveSignal* createSignal(const QString& name,
                               WaveSignalKind kind,
                               int width = 1,
                               const QString& scopePath = {})
      {
         auto sig = std::make_unique<WaveSignal>(name, kind, width, scopePath);
         WaveSignal* raw = sig.get();
         signalList.push_back(std::move(sig));
         return raw;
      }

      DesignNode* createDesignNode(const QString& name, const QString& fullPath)
      {
         auto node = std::make_unique<DesignNode>(name, fullPath);
         DesignNode* raw = node.get();
         designNodes.push_back(std::move(node));
         return raw;
      }

      DisplayNode* createDisplayRoot(const QString& name)
      {
         auto node = std::make_unique<DisplayNode>(name, DisplayNodeKind::Group);
         DisplayNode* raw = node.get();
         displayRoots.push_back(std::move(node));
         return raw;
      }

      void addHierarchyRoot(DesignNode* root)
      {
         hierarchyRoots.push_back(root);
      }

      qreal nearestTransitionTime(qreal time, qreal maxDistance = -1.0) const
      {
         bool found = false;
         qreal bestTime = time;
         qreal bestDist = 0.0;

         for (const auto& sig : signalList)
         {
            if (!sig)
               continue;

            const qreal t = sig->nearestEdgeTime(time);
            const qreal dist = std::abs(t - time);

            if (!found || dist < bestDist)
            {
               found = true;
               bestDist = dist;
               bestTime = t;
            }
         }

         if (!found)
            return time;

         if (maxDistance >= 0.0 && bestDist > maxDistance)
            return time;

         return bestTime;
      }

      qreal endTime() const
      {
         qreal t = 0.0;
         for (const auto& sig : signalList)
            t = std::max(t, sig->endTime());
         return t;
      }

      void dump () const
      {
         qDebug().noquote() << "Design Nodes:";
         for (const auto& dn: hierarchyRoots)
            dn->dump(3);

         qDebug().noquote() << "Signals";
         for (const auto& sig : signalList)
            sig->dump(3);
      }
};