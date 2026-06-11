#include "waveform_view_widget.h"
#include "signal_tree_widget.h"
#include "wave_query_ast.h"
#include "wave_query_parser.h"
#include "wave_query_engine.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QHeaderView>
#include <QKeySequence>
#include <QScrollBar>
#include <QShortcut>
#include <QSizePolicy>
#include <QTreeWidgetItem>
#include <QInputDialog>

#include <algorithm>
#include <cmath>
#include <functional>

WaveformViewWidget::WaveformViewWidget(QWidget* parent)
   : QWidget(parent)
{
   buildUi();
   connectUi();

   m_cursors.push_back(WaveformCursor(Cursor1Id, 10.0, QColor(255, 255, 0), true, "C1"));
   m_cursors.push_back(WaveformCursor(Cursor2Id, 25.0, QColor(0, 255, 255), true, "C2"));

   populateDemoData();
}

void WaveformViewWidget::buildUi()
{
   auto* topLayout = new QVBoxLayout(this);
   topLayout->setContentsMargins(0, 0, 0, 0);
   topLayout->setSpacing(0);

   m_mainSplitter = new QSplitter(Qt::Horizontal, this);
   topLayout->addWidget(m_mainSplitter);

   m_leftPane = new QWidget(m_mainSplitter);
   m_leftLayout = new QVBoxLayout(m_leftPane);
   m_leftLayout->setContentsMargins(0, 0, 0, 0);
   m_leftLayout->setSpacing(0);

   m_leftSplitter = new QSplitter(Qt::Vertical, m_leftPane);

   m_hierarchyTree = new SignalTreeWidget(m_leftSplitter);
   m_hierarchyTree->setColumnCount(1);
   m_hierarchyTree->setHeaderLabels({"Hierarchy"});

   m_scopeSignalTree = new SignalTreeWidget(m_leftSplitter);
   m_scopeSignalTree->setColumnCount(1);
   m_scopeSignalTree->setHeaderLabels({"Signals"});
   m_scopeSignalTree->setDragEnabled(true);
   m_scopeSignalTree->setDragDropMode(QAbstractItemView::DragOnly);
   m_scopeSignalTree->setSelectionMode(QAbstractItemView::ExtendedSelection);

   m_leftSplitter->addWidget(m_hierarchyTree);
   m_leftSplitter->addWidget(m_scopeSignalTree);
   m_leftSplitter->setStretchFactor(0, 1);
   m_leftSplitter->setStretchFactor(1, 1);

   m_leftLayout->addWidget(m_leftSplitter);

   m_rightSplitter = new QSplitter(Qt::Horizontal, m_mainSplitter);

   m_waveNamePane = new QWidget(m_rightSplitter);
   m_waveNameLayout = new QVBoxLayout(m_waveNamePane);
   m_waveNameLayout->setContentsMargins(0, 0, 0, 0);
   m_waveNameLayout->setSpacing(0);

   m_waveNameTree = new SignalTreeWidget(m_waveNamePane);
   m_waveNameTree->setColumnCount(3);
   m_waveNameTree->setHeaderLabels({"Waves", "Value", "Radix"});

   m_waveNameBottomSpacer = new QWidget(m_waveNamePane);

   m_waveNameLayout->addWidget(m_waveNameTree);
   m_waveNameLayout->addWidget(m_waveNameBottomSpacer);

   m_rightPane = new QWidget(m_rightSplitter);
   m_rightLayout = new QVBoxLayout(m_rightPane);
   m_rightLayout->setContentsMargins(0, 0, 0, 0);
   m_rightLayout->setSpacing(0);

   m_waveDisplay = new WaveDisplayWidget(m_rightPane);
   m_waveDisplay->setWaveNameTree(m_waveNameTree);
   m_waveDisplay->setDisplayNodeMap(&m_waveItemToDisplayNode);

   m_cursorTrack = new CursorTrackWidget(m_rightPane);
   m_cursorTrack->setLaneHeight(CursorTrackLaneHeight);

   m_timeRuler = new TimeRulerWidget(m_rightPane);
   m_timeRuler->setPixelsPerUnit(m_pixelsPerUnit);
   m_timeRuler->setSecondsPerUnit(1e-9);
   m_timeRuler->setAutoDisplayUnitEnabled(true);

   m_rightLayout->addWidget(m_waveDisplay);
   m_rightLayout->addWidget(m_cursorTrack);
   m_rightLayout->addWidget(m_timeRuler);

   m_rightLayout->setStretch(0, 1);
   m_rightLayout->setStretch(1, 0);
   m_rightLayout->setStretch(2, 0);

   m_mainSplitter->addWidget(m_leftPane);
   m_mainSplitter->addWidget(m_rightSplitter);
   m_mainSplitter->setStretchFactor(0, 0);
   m_mainSplitter->setStretchFactor(1, 1);

   m_rightSplitter->addWidget(m_waveNamePane);
   m_rightSplitter->addWidget(m_rightPane);
   m_rightSplitter->setStretchFactor(0, 0);
   m_rightSplitter->setStretchFactor(1, 1);

   m_mainSplitter->setSizes({350, 950});
   m_rightSplitter->setSizes({260, 900});

   QFont treeFont = font();
   treeFont.setPointSize(std::max(8, treeFont.pointSize()));

   QFont waveFont = treeFont;
   waveFont.setPointSize(treeFont.pointSize() + 1);

   m_hierarchyTree->setFont(treeFont);
   m_scopeSignalTree->setFont(treeFont);
   m_waveNameTree->setFont(waveFont);

   configureWaveNameTreeForReorder();
}

void WaveformViewWidget::connectUi()
{
   connect(m_hierarchyTree, &QTreeWidget::itemSelectionChanged,
           this, &WaveformViewWidget::handleHierarchySelectionChanged);

   connect(m_waveNameTree, &QTreeWidget::itemSelectionChanged,
           this, &WaveformViewWidget::handleTreeSelectionChanged);

   connect(m_waveNameTree, &QTreeWidget::itemExpanded,
           this, &WaveformViewWidget::rebuildWaveRows);
   connect(m_waveNameTree, &QTreeWidget::itemCollapsed,
           this, &WaveformViewWidget::rebuildWaveRows);

   connect(m_scopeSignalTree, &QTreeWidget::itemDoubleClicked,
           this, &WaveformViewWidget::handleScopeSignalItemDoubleClicked);

   connect(m_waveNameTree, &SignalTreeWidget::deletePressed,
           this, &WaveformViewWidget::handleWaveDeletePressed);

   connect(m_waveNameTree, &SignalTreeWidget::layoutMayHaveChanged,
           this, [this]()
           {
              rebuildWaveDisplayRootsFromTree();
              rebuildWaveRows();
              updateTreeValuesForActiveCursor();
           });

   connect(m_waveNameTree, &SignalTreeWidget::externalMimeDropped,
           this, &WaveformViewWidget::handleSignalsDroppedOnWaves);

   connect(m_waveNameTree->verticalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncTreeToWaveScroll);

   connect(m_waveDisplay->verticalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncWaveToTreeScroll);

   connect(m_waveDisplay->horizontalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncWaveToRulerScroll);

   connect(m_timeRuler->horizontalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncRulerToWaveScroll);

   connect(m_waveDisplay->horizontalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncWaveToCursorTrackScroll);

   connect(m_cursorTrack->horizontalScrollBar(), &QScrollBar::valueChanged,
           this, &WaveformViewWidget::syncCursorTrackToWaveScroll);

   connect(m_waveDisplay, &WaveDisplayWidget::zoomStepRequested,
           this, &WaveformViewWidget::applyZoom);

   connect(m_waveDisplay, &WaveDisplayWidget::areaZoomRequested,
           this, &WaveformViewWidget::zoomToTimeRange);

   connect(m_waveDisplay, &WaveDisplayWidget::leftMousePressed,
           this, &WaveformViewWidget::handleWaveLeftMousePressed);
   connect(m_waveDisplay, &WaveDisplayWidget::leftMouseDragged,
           this, &WaveformViewWidget::handleWaveLeftMouseDragged);
   connect(m_waveDisplay, &WaveDisplayWidget::leftMouseReleased,
           this, &WaveformViewWidget::handleWaveLeftMouseReleased);

   auto* zoomFitShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
   connect(zoomFitShortcut, &QShortcut::activated,
           this, &WaveformViewWidget::zoomFit);

   auto* zoomCursorShortcut = new QShortcut(QKeySequence("Ctrl+B"), this);
   connect(zoomCursorShortcut, &QShortcut::activated,
           this, &WaveformViewWidget::zoomBetweenCursors);


   // temporary keyboard actions
   auto* groupShortcut = new QShortcut(QKeySequence("Ctrl+G"), this);
   connect(groupShortcut, &QShortcut::activated,
           this,
           static_cast<void (WaveformViewWidget::*)()>(
              &WaveformViewWidget::groupSelectedWaveNodes));

   auto* renameGroupShortcut = new QShortcut(QKeySequence(Qt::Key_F2), this);
   connect(renameGroupShortcut, &QShortcut::activated,
           this,
           static_cast<void (WaveformViewWidget::*)()>(
              &WaveformViewWidget::renameSelectedGroup));

   auto* ungroupShortcut = new QShortcut(QKeySequence("Ctrl+U"), this);
   connect(ungroupShortcut, &QShortcut::activated,
           this,
           static_cast<void (WaveformViewWidget::*)()>(
              &WaveformViewWidget::ungroupSelectedWaveGroup));

   auto* findShortcut = new QShortcut(QKeySequence(Qt::Key_F1), this);
   connect(findShortcut, &QShortcut::activated,
           this,
           &WaveformViewWidget::doFind);




}

void WaveformViewWidget::configureWaveNameTreeForReorder()
{
   m_waveNameTree->setDragEnabled(true);
   m_waveNameTree->setAcceptDrops(true);
   m_waveNameTree->setDropIndicatorShown(true);
   m_waveNameTree->setDragDropMode(QAbstractItemView::DragDrop);
   m_waveNameTree->setDefaultDropAction(Qt::MoveAction);
}

void WaveformViewWidget::populateDemoData()
{
   auto document = std::make_unique<WaveformDocument>();

   auto* sigClk = document->createSignal("clk", WaveSignalKind::Bit, 1, "top");
   sigClk->segments = {
      {0.0, 5.0, "0"},
      {5.0, 10.0, "1"},
      {10.0, 15.0, "0"},
      {15.0, 20.0, "1"},
      {20.0, 25.0, "0"},
      {25.0, 30.0, "1"}
   };

   auto* sigRst = document->createSignal("rst_n", WaveSignalKind::Bit, 1, "top");
   sigRst->segments = {
      {0.0, 8.0, "0"},
      {8.0, 16.0, "X"},
      {16.0, 24.0, "Z"},
      {24.0, 30.0, "1"}
   };

   auto* sigBus = document->createSignal("data_bus", WaveSignalKind::Bus, 8, "top");
   sigBus->segments = {
      {2.0, 10.0, "00000000"},
      {10.0, 22.0, "10100101"},
      {22.0, 28.0, "101X101Z"},
      {28.0, 42.0, "XZ001100"},
      {42.0, 62.0, "11110000"}
   };

   auto* sigTx = document->createSignal("tx_data", WaveSignalKind::Bus, 8, "u_uart");
   sigTx->segments = {
      {0.0, 12.0, "01010101"},
      {12.0, 24.0, "10100001"},
      {24.0, 36.0, "01011010"}
   };

   auto* sigRx = document->createSignal("rx_data", WaveSignalKind::Bus, 8, "u_uart");
   sigRx->segments = {
      {0.0, 14.0, "00110011"},
      {14.0, 28.0, "11001100"},
      {28.0, 42.0, "11110000"}
   };

   sigClk->style.normalColor = QColor(0, 255, 0);
   sigClk->style.textColor = QColor(0, 255, 0);

   sigRst->style.normalColor = QColor(255, 255, 0);
   sigRst->style.textColor = QColor(255, 255, 0);

   sigBus->style.normalColor = QColor(0, 255, 0);
   sigBus->style.textColor = QColor(0, 255, 0);

   sigTx->style.normalColor = QColor(255, 165, 0);
   sigTx->style.textColor = QColor(255, 165, 0);

   sigRx->style.normalColor = QColor(173, 216, 230);
   sigRx->style.textColor = QColor(173, 216, 230);

   auto* topNode = document->createDesignNode("top", "top");
   auto* uartNode = document->createDesignNode("u_uart", "u_uart");

   topNode->signalList.push_back(sigClk);
   topNode->signalList.push_back(sigRst);
   topNode->signalList.push_back(sigBus);

   uartNode->signalList.push_back(sigTx);
   uartNode->signalList.push_back(sigRx);

   document->addHierarchyRoot(topNode);
   document->addHierarchyRoot(uartNode);

   loadDocument(std::move(document));
}

void WaveformViewWidget::loadDocument(std::unique_ptr<WaveformDocument> document)
{
   m_document = std::move(document);
   m_scopeDisplayRoots.clear();
   m_waveDisplayRoots.clear();

   rebuildTreeFromDocument();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
   updateDeltaDisplay();
}

void WaveformViewWidget::rebuildTreeFromDocument()
{
   rebuildHierarchyTree();

   QTreeWidgetItem* item = m_hierarchyTree->currentItem();
   DesignNode* node = m_itemToDesignNode.value(item, nullptr);

   rebuildScopeSignalTree(node);

   if (m_waveDisplayRoots.empty() && node)
   {
      for (WaveSignal* sig : node->signalList)
         m_waveDisplayRoots.push_back(makeSignalDisplayNode(sig, false));
   }

   rebuildWaveNameTreeFromWaveRoots();
}

void WaveformViewWidget::rebuildHierarchyTree()
{
   m_itemToDesignNode.clear();
   m_hierarchyTree->clear();

   if (!m_document)
      return;

   std::function<void(QTreeWidgetItem*, DesignNode*)> addNode =
      [&](QTreeWidgetItem* parentItem, DesignNode* node)
   {
      QTreeWidgetItem* item = nullptr;

      if (parentItem)
         item = new QTreeWidgetItem(parentItem);
      else
         item = new QTreeWidgetItem(m_hierarchyTree);

      item->setText(0, node->name);
      m_itemToDesignNode.insert(item, node);

      for (DesignNode* child : node->children)
         addNode(item, child);

      item->setExpanded(true);
   };

   for (DesignNode* root : m_document->hierarchyRoots)
      addNode(nullptr, root);

   if (m_hierarchyTree->topLevelItemCount() > 0)
      m_hierarchyTree->setCurrentItem(m_hierarchyTree->topLevelItem(0));
}

void WaveformViewWidget::rebuildScopeSignalTree(DesignNode* scopeNode)
{
   m_scopeItemToDisplayNode.clear();
   m_scopeSignalTree->clear();
   m_scopeDisplayRoots.clear();

   if (!scopeNode)
      return;

   for (WaveSignal* sig : scopeNode->signalList)
   {
      auto sigNode = makeSignalDisplayNode(sig, false);

      DisplayNode* raw = sigNode.get();
      m_scopeDisplayRoots.push_back(std::move(sigNode));

      auto* item = new QTreeWidgetItem(m_scopeSignalTree);
      item->setText(0, raw->name);
      item->setData(0, Qt::UserRole, sig->fullPath);
      m_scopeItemToDisplayNode.insert(item, raw);

      for (const auto& child : raw->children)
      {
         auto* childItem = new QTreeWidgetItem(item);
         childItem->setText(0, child->name);
         childItem->setData(0, Qt::UserRole,
                            QString("%1[%2]").arg(sig->fullPath).arg(child->msb));
         m_scopeItemToDisplayNode.insert(childItem, child.get());
      }

      item->setExpanded(false);
   }
}

void WaveformViewWidget::rebuildWaveNameTreeFromWaveRoots()
{
   m_waveItemToDisplayNode.clear();
   m_waveNameTree->clear();

   std::function<void(QTreeWidgetItem*, DisplayNode*)> addNode =
      [&](QTreeWidgetItem* parentItem, DisplayNode* node)
   {
      QTreeWidgetItem* item = nullptr;

      if (parentItem)
         item = new QTreeWidgetItem(parentItem);
      else
         item = new QTreeWidgetItem(m_waveNameTree);

      item->setText(SignalTreeWidget::NameColumn, node->name);

      if (node->kind == DisplayNodeKind::Group)
      {
         item->setText(SignalTreeWidget::RadixColumn, "");
         item->setText(SignalTreeWidget::ValueColumn, "");
      }
      else if (node->kind == DisplayNodeKind::Signal && node->signal)
      {
         item->setText(SignalTreeWidget::RadixColumn,
                       node->signal->kind == WaveSignalKind::Bus ? "hex" : "bin");
      }
      else if (node->kind == DisplayNodeKind::Field)
      {
         item->setText(SignalTreeWidget::RadixColumn, "hex");
      }
      else if (node->kind == DisplayNodeKind::Bit)
      {
         item->setText(SignalTreeWidget::RadixColumn, "bin");
      }

      m_waveItemToDisplayNode.insert(item, node);

      for (const auto& child : node->children)
         addNode(item, child.get());

      item->setExpanded(false);
   };

   for (const auto& root : m_waveDisplayRoots)
      addNode(nullptr, root.get());
}

std::unique_ptr<DisplayNode> WaveformViewWidget::cloneDisplayNode(const DisplayNode* src) const
{
   if (!src)
      return nullptr;

   QString name = src->name;

   if (src->kind == DisplayNodeKind::Bit &&
       src->signal &&
       name.startsWith("["))
   {
      name = QString("%1%2").arg(src->signal->name, src->name);
   }

   auto node = std::make_unique<DisplayNode>(name, src->kind);
   node->signal = src->signal;
   node->msb = src->msb;
   node->lsb = src->lsb;

   for (const auto& child : src->children)
   {
      auto clonedChild = cloneDisplayNode(child.get());
      if (clonedChild)
         node->addChild(std::move(clonedChild));
   }

   return node;
}

void WaveformViewWidget::addDisplayNodeToWaveView(const DisplayNode* src)
{
   if (!src)
      return;

   if (!(src->kind == DisplayNodeKind::Signal ||
         src->kind == DisplayNodeKind::Field ||
         src->kind == DisplayNodeKind::Bit))
   {
      return;
   }

   m_waveDisplayRoots.push_back(cloneDisplayNode(src));
}

void WaveformViewWidget::handleScopeSignalItemDoubleClicked(QTreeWidgetItem* item, int column)
{
   Q_UNUSED(column)

   DisplayNode* node = m_scopeItemToDisplayNode.value(item, nullptr);
   addDisplayNodeToWaveView(node);

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::addSelectedScopeSignalsToWaveView()
{
   const auto items = m_scopeSignalTree->selectedItems();

   for (QTreeWidgetItem* item : items)
   {
      DisplayNode* node = m_scopeItemToDisplayNode.value(item, nullptr);
      addDisplayNodeToWaveView(node);
   }

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::handleWaveDeletePressed()
{
   removeSelectedWaveNodes();
}

bool WaveformViewWidget::isTopLevelWaveItem(QTreeWidgetItem* item) const
{
   return item && item->parent() == nullptr;
}

void WaveformViewWidget::handleSignalsDroppedOnWaves(
   const QMimeData* mimeData,
   QTreeWidgetItem* targetItem,
   SignalTreeWidget::TreeDropPosition position)
{
   if (!mimeData ||
       !mimeData->hasFormat("application/x-wave-signal-paths"))
   {
      return;
   }

   const QString payload =
      QString::fromUtf8(mimeData->data("application/x-wave-signal-paths"));

   const QStringList ids =
      payload.split('\n', Qt::SkipEmptyParts);

   for (const QString& id : ids)
   {
      auto node = makeDisplayNodeFromDropId(id.trimmed());
      insertWaveDisplayNode(std::move(node), targetItem, position);

      if (position == SignalTreeWidget::TreeDropPosition::AboveItem ||
          position == SignalTreeWidget::TreeDropPosition::BelowItem)
      {
         if (targetItem)
            position = SignalTreeWidget::TreeDropPosition::BelowItem;
      }
   }

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
}

WaveSignal* WaveformViewWidget::findSignalByPath(const QString& fullPath) const
{
   if (!m_document)
      return nullptr;

   for (const auto& sig : m_document->signalList)
   {
      if (sig && sig->fullPath == fullPath)
         return sig.get();
   }

   return nullptr;
}

std::unique_ptr<DisplayNode>
WaveformViewWidget::makeDisplayNodeFromDropId(const QString& id) const
{
   // Bit drop format: full.path[7]
   const int lb = id.lastIndexOf('[');
   const int rb = id.endsWith(']') ? id.size() - 1 : -1;

   if (lb > 0 && rb > lb)
   {
      const QString path = id.left(lb);
      const QString bitText = id.mid(lb + 1, rb - lb - 1);

      bool ok = false;
      const int bit = bitText.toInt(&ok);
      WaveSignal* sig = findSignalByPath(path);

      if (ok && sig)
      {
         auto node = std::make_unique<DisplayNode>(
            QString("%1[%2]").arg(sig->name).arg(bit),
            DisplayNodeKind::Bit);

         node->signal = sig;
         node->msb = bit;
         node->lsb = bit;
         return node;
      }
   }

   WaveSignal* sig = findSignalByPath(id);
   if (!sig)
      return nullptr;

   return makeSignalDisplayNode(sig, false);
}

int WaveformViewWidget::topLevelIndexOfWaveItem(QTreeWidgetItem* item) const
{
   if (!item)
      return -1;

   QTreeWidgetItem* root = item;
   while (root->parent())
      root = root->parent();

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      if (m_waveNameTree->topLevelItem(i) == root)
         return i;
   }

   return -1;
}

bool WaveformViewWidget::insertIntoGroup(DisplayNode* group,
                                         std::unique_ptr<DisplayNode> node)
{
   if (!group || group->kind != DisplayNodeKind::Group || !node)
      return false;

   group->addChild(std::move(node));
   return true;
}

void WaveformViewWidget::insertWaveDisplayNode(
   std::unique_ptr<DisplayNode> node,
   QTreeWidgetItem* targetItem,
   SignalTreeWidget::TreeDropPosition position)
{
   if (!node)
      return;

   DisplayNode* targetNode = m_waveItemToDisplayNode.value(targetItem, nullptr);

   if (position == SignalTreeWidget::TreeDropPosition::OnItem &&
       targetNode &&
       targetNode->kind == DisplayNodeKind::Group)
   {
      targetNode->addChild(std::move(node));
      return;
   }

   int index = topLevelIndexOfWaveItem(targetItem);

   if (index < 0)
   {
      m_waveDisplayRoots.push_back(std::move(node));
      return;
   }

   if (position == SignalTreeWidget::TreeDropPosition::BelowItem ||
       position == SignalTreeWidget::TreeDropPosition::OnItem)
   {
      ++index;
   }

   index = std::clamp(index, 0, int(m_waveDisplayRoots.size()));
   m_waveDisplayRoots.insert(m_waveDisplayRoots.begin() + index, std::move(node));
}

void WaveformViewWidget::groupSelectedWaveNodes()
{
   groupSelectedWaveNodes("Group");
}

void WaveformViewWidget::groupSelectedWaveNodes(const QString& groupName)
{
   const auto selected = m_waveNameTree->selectedItems();
   if (selected.isEmpty())
      return;

   QSet<QTreeWidgetItem*> selectedTopItems;

   for (QTreeWidgetItem* item : selected)
   {
      if (!item)
         continue;

      QTreeWidgetItem* root = item;
      while (root->parent())
         root = root->parent();

      selectedTopItems.insert(root);
   }

   if (selectedTopItems.isEmpty())
      return;

   auto groupNode = std::make_unique<DisplayNode>(groupName, DisplayNodeKind::Group);
   DisplayNode* rawGroup = groupNode.get();

   std::vector<std::unique_ptr<DisplayNode>> newRoots;
   newRoots.reserve(m_waveDisplayRoots.size());

   bool groupInserted = false;

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* treeItem = m_waveNameTree->topLevelItem(i);

      if (i < 0 || i >= static_cast<int>(m_waveDisplayRoots.size()))
         continue;

      if (selectedTopItems.contains(treeItem))
      {
         // Insert the group at the position of the first selected item,
         // but do not move groupNode yet. We still need to add more children.
         if (!groupInserted)
         {
            newRoots.push_back(nullptr);
            groupInserted = true;
         }

         groupNode->addChild(std::move(m_waveDisplayRoots[size_t(i)]));
      }
      else
      {
         newRoots.push_back(std::move(m_waveDisplayRoots[size_t(i)]));
      }
   }

   if (!groupInserted || groupNode->children.empty())
      return;

   for (auto& root : newRoots)
   {
      if (!root)
      {
         root = std::move(groupNode);
         break;
      }
   }

   m_waveDisplayRoots = std::move(newRoots);

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* item = m_waveNameTree->topLevelItem(i);
      DisplayNode* node = m_waveItemToDisplayNode.value(item, nullptr);

      if (node == rawGroup)
      {
         m_waveNameTree->setCurrentItem(item);
         item->setExpanded(true);
         break;
      }
   }
}

void WaveformViewWidget::renameSelectedGroup()
{
   QTreeWidgetItem* item = m_waveNameTree->currentItem();
   DisplayNode* node = m_waveItemToDisplayNode.value(item, nullptr);

   if (!node || node->kind != DisplayNodeKind::Group)
      return;

   QString newName = QInputDialog::getText(this, tr("New Group Name"), tr("Name"));
   if (newName.isEmpty()) return;

   node->name = newName;

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
}

void WaveformViewWidget::ungroupSelectedWaveGroup()
{
   QTreeWidgetItem* selectedItem = m_waveNameTree->currentItem();
   if (!selectedItem)
      return;

   QTreeWidgetItem* rootItem = selectedItem;
   while (rootItem->parent())
      rootItem = rootItem->parent();

   DisplayNode* node = m_waveItemToDisplayNode.value(rootItem, nullptr);
   if (!node || node->kind != DisplayNodeKind::Group)
      return;

   std::vector<std::unique_ptr<DisplayNode>> newRoots;

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* treeItem = m_waveNameTree->topLevelItem(i);

      if (i < 0 || i >= static_cast<int>(m_waveDisplayRoots.size()))
         continue;

      if (treeItem == rootItem)
      {
         auto group = std::move(m_waveDisplayRoots[size_t(i)]);

         for (auto& child : group->children)
         {
            if (child)
               newRoots.push_back(std::move(child));
         }

         group->children.clear();
      }
      else
      {
         newRoots.push_back(std::move(m_waveDisplayRoots[size_t(i)]));
      }
   }

   m_waveDisplayRoots = std::move(newRoots);

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::removeSelectedWaveNodes()
{
   const auto selected = m_waveNameTree->selectedItems();
   if (selected.isEmpty())
      return;

   QSet<QTreeWidgetItem*> topRoots;

   for (QTreeWidgetItem* item : selected)
   {
      QTreeWidgetItem* root = item;
      while (root->parent())
         root = root->parent();

      topRoots.insert(root);
   }

   std::vector<std::unique_ptr<DisplayNode>> newRoots;
   newRoots.reserve(m_waveDisplayRoots.size());

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* item = m_waveNameTree->topLevelItem(i);
      if (topRoots.contains(item))
         continue;

      if (i >= 0 && i < static_cast<int>(m_waveDisplayRoots.size()))
         newRoots.push_back(std::move(m_waveDisplayRoots[size_t(i)]));
   }

   m_waveDisplayRoots = std::move(newRoots);

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::rebuildWaveDisplayRootsFromTree()
{
   std::vector<std::unique_ptr<DisplayNode>> reordered;
   reordered.reserve(m_waveDisplayRoots.size());

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* item = m_waveNameTree->topLevelItem(i);
      DisplayNode* node = m_waveItemToDisplayNode.value(item, nullptr);
      if (!node)
         continue;

      for (auto& owned : m_waveDisplayRoots)
      {
         if (owned.get() == node)
         {
            reordered.push_back(std::move(owned));
            break;
         }
      }
   }

   for (auto& owned : m_waveDisplayRoots)
   {
      if (owned)
         reordered.push_back(std::move(owned));
   }

   m_waveDisplayRoots = std::move(reordered);
}

void WaveformViewWidget::handleHierarchySelectionChanged()
{
   QTreeWidgetItem* item = m_hierarchyTree->currentItem();
   DesignNode* node = m_itemToDesignNode.value(item, nullptr);

   rebuildScopeSignalTree(node);
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::handleTreeSelectionChanged()
{
   if (m_waveDisplay)
      m_waveDisplay->setSelectedRowsFromTree();
}

void WaveformViewWidget::rebuildWaveRows()
{
   rebuildDisplayLayout();
}

void WaveformViewWidget::rebuildDisplayLayout()
{
   const qreal logicalEnd =
      m_document ? std::max<qreal>(100.0, m_document->endTime()) : 100.0;

   const qreal sceneWidth = scaleX(logicalEnd);

   m_waveDisplay->setLogicalEndTime(logicalEnd);
   m_waveDisplay->setPixelsPerUnit(m_pixelsPerUnit);
   m_waveDisplay->rebuildRows();
   m_waveDisplay->updateCursorLines(m_cursors);

   m_timeRuler->setPixelsPerUnit(m_pixelsPerUnit);
   m_timeRuler->setSceneWidth(sceneWidth);
   m_timeRuler->setHeaderHeight(m_waveNameTree->header()->height());
   m_timeRuler->updateCursorOverlay(m_cursors, m_pixelsPerUnit);

   m_cursorTrack->setSceneWidth(sceneWidth);
   m_cursorTrack->updateCursors(
      m_cursors,
      m_activeCursorId,
      m_pixelsPerUnit,
      [this](qreal t) { return formatDisplayTime(t); },
      [this](qreal dt) { return formatDeltaTime(dt); });

   updateWaveNameBottomSpacer();
   syncVerticalScrollRanges();

   m_waveDisplay->viewport()->update();
   m_timeRuler->viewport()->update();
   m_cursorTrack->viewport()->update();
}

void WaveformViewWidget::updateScaleOnly()
{
   const qreal logicalEnd =
      m_document ? std::max<qreal>(100.0, m_document->endTime()) : 100.0;

   const qreal sceneWidth = scaleX(logicalEnd);

   m_waveDisplay->setLogicalEndTime(logicalEnd);
   m_waveDisplay->setPixelsPerUnit(m_pixelsPerUnit);

   m_timeRuler->setPixelsPerUnit(m_pixelsPerUnit);
   m_timeRuler->setSceneWidth(sceneWidth);
   m_timeRuler->updateCursorOverlay(m_cursors, m_pixelsPerUnit);

   m_cursorTrack->setSceneWidth(sceneWidth);

   updateCursorDisplays();

   m_waveDisplay->viewport()->update();
   m_timeRuler->viewport()->update();
   m_cursorTrack->viewport()->update();
}

void WaveformViewWidget::updateCursorDisplays()
{
   m_waveDisplay->updateCursorLines(m_cursors);
   m_timeRuler->updateCursorOverlay(m_cursors, m_pixelsPerUnit);

   m_cursorTrack->updateCursors(
      m_cursors,
      m_activeCursorId,
      m_pixelsPerUnit,
      [this](qreal t) { return formatDisplayTime(t); },
      [this](qreal dt) { return formatDeltaTime(dt); });
}

void WaveformViewWidget::syncTreeToWaveScroll(int value)
{
   if (m_syncingVerticalScroll)
      return;

   m_syncingVerticalScroll = true;
   m_waveDisplay->verticalScrollBar()->setValue(value);
   m_syncingVerticalScroll = false;
}

void WaveformViewWidget::syncWaveToTreeScroll(int value)
{
   if (m_syncingVerticalScroll)
      return;

   m_syncingVerticalScroll = true;
   m_waveNameTree->verticalScrollBar()->setValue(value);
   m_syncingVerticalScroll = false;
}

void WaveformViewWidget::syncWaveToRulerScroll(int value)
{
   if (m_syncingHorizontalScroll)
      return;

   m_syncingHorizontalScroll = true;
   m_timeRuler->horizontalScrollBar()->setValue(value);
   m_cursorTrack->horizontalScrollBar()->setValue(value);
   m_syncingHorizontalScroll = false;
}

void WaveformViewWidget::syncRulerToWaveScroll(int value)
{
   if (m_syncingHorizontalScroll)
      return;

   m_syncingHorizontalScroll = true;
   m_waveDisplay->horizontalScrollBar()->setValue(value);
   m_cursorTrack->horizontalScrollBar()->setValue(value);
   m_syncingHorizontalScroll = false;
}

void WaveformViewWidget::syncWaveToCursorTrackScroll(int value)
{
   if (m_syncingHorizontalScroll)
      return;

   m_syncingHorizontalScroll = true;
   m_cursorTrack->horizontalScrollBar()->setValue(value);
   m_timeRuler->horizontalScrollBar()->setValue(value);
   m_syncingHorizontalScroll = false;
}

void WaveformViewWidget::syncCursorTrackToWaveScroll(int value)
{
   if (m_syncingHorizontalScroll)
      return;

   m_syncingHorizontalScroll = true;
   m_waveDisplay->horizontalScrollBar()->setValue(value);
   m_timeRuler->horizontalScrollBar()->setValue(value);
   m_syncingHorizontalScroll = false;
}

void WaveformViewWidget::syncVerticalScrollRanges()
{
   QScrollBar* treeBar = m_waveNameTree->verticalScrollBar();
   QScrollBar* waveBar = m_waveDisplay->verticalScrollBar();

   const bool old = m_syncingVerticalScroll;
   m_syncingVerticalScroll = true;

   waveBar->setRange(treeBar->minimum(), treeBar->maximum());
   waveBar->setPageStep(treeBar->pageStep());
   waveBar->setSingleStep(treeBar->singleStep());
   waveBar->setValue(treeBar->value());

   m_syncingVerticalScroll = old;
}

void WaveformViewWidget::updateWaveNameBottomSpacer()
{
   const int h =
      m_cursorTrack->height()
      + m_timeRuler->height();

   m_waveNameBottomSpacer->setFixedHeight(h);
}

void WaveformViewWidget::handleWaveLeftMousePressed(const QPointF& scenePos,
                                                    Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)

   constexpr qreal PickDistancePx = 8.0;

   const int picked = findCursorNearSceneX(scenePos.x(), PickDistancePx);
   if (picked >= 0)
      setActiveCursor(picked);

   WaveformCursor* active = findCursor(m_activeCursorId);
   if (!active || active->locked)
   {
      m_draggingCursor = false;
      return;
   }

   m_draggingCursor = true;
   setCursorTime(m_activeCursorId, unscaleX(scenePos.x()));
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::handleWaveLeftMouseDragged(const QPointF& scenePos,
                                                    Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)

   if (!m_draggingCursor)
      return;

   WaveformCursor* active = findCursor(m_activeCursorId);
   if (!active || active->locked)
      return;

   setCursorTime(m_activeCursorId, unscaleX(scenePos.x()));
   updateTreeValuesForActiveCursor();
}

void WaveformViewWidget::handleWaveLeftMouseReleased(const QPointF& scenePos,
                                                     Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)

   if (!m_draggingCursor)
      return;

   m_draggingCursor = false;

   snapActiveCursorToNearbyEdge(scenePos);
   updateTreeValuesForActiveCursor();
}

int WaveformViewWidget::findCursorNearSceneX(qreal sceneX, qreal maxDistancePx) const
{
   int bestId = -1;
   qreal bestDist = 0.0;
   bool found = false;

   for (const auto& cursor : m_cursors)
   {
      if (!cursor.visible || cursor.locked)
         continue;

      const qreal x = scaleX(cursor.time);
      const qreal dist = std::abs(x - sceneX);

      if (dist <= maxDistancePx && (!found || dist < bestDist))
      {
         found = true;
         bestDist = dist;
         bestId = cursor.id;
      }
   }

   return bestId;
}

void WaveformViewWidget::setActiveCursor(int cursorId)
{
   if (findCursor(cursorId))
      m_activeCursorId = cursorId;

   updateCursorDisplays();
   updateTreeValuesForActiveCursor();
   updateDeltaDisplay();
}

void WaveformViewWidget::setCursorTime(int cursorId, qreal time)
{
   WaveformCursor* cursor = findCursor(cursorId);
   if (!cursor)
      return;

   cursor->time = time;

   updateCursorDisplays();
   updateDeltaDisplay();
}

WaveformCursor* WaveformViewWidget::findCursor(int cursorId)
{
   for (auto& cursor : m_cursors)
   {
      if (cursor.id == cursorId)
         return &cursor;
   }

   return nullptr;
}

const WaveformCursor* WaveformViewWidget::findCursor(int cursorId) const
{
   for (const auto& cursor : m_cursors)
   {
      if (cursor.id == cursorId)
         return &cursor;
   }

   return nullptr;
}

bool WaveformViewWidget::snapActiveCursorToNearbyEdge(const QPointF& scenePos)
{
   WaveformCursor* active = findCursor(m_activeCursorId);
   if (!active || active->locked)
      return false;

   QTreeWidgetItem* item = m_waveDisplay->itemAtScenePos(scenePos);
   if (!item)
      return false;

   DisplayNode* node = m_waveItemToDisplayNode.value(item, nullptr);
   if (!node || !node->signal)
      return false;

   const qreal currentTime = active->time;
   const qreal snappedTime =
      node->signal->nearestEdgeTime(currentTime, snapThresholdTime());

   if (std::abs(snappedTime - currentTime) <= 1e-9)
      return false;

   setCursorTime(m_activeCursorId, snappedTime);
   return true;
}

qreal WaveformViewWidget::snapThresholdTime() const
{
   constexpr qreal SnapPx = 8.0;
   return SnapPx / m_pixelsPerUnit;
}

void WaveformViewWidget::updateTreeValuesForActiveCursor()
{
   updateTreeValuesForCursor(m_activeCursorId);
}

void WaveformViewWidget::updateTreeValuesForCursor(int cursorId)
{
   const WaveformCursor* cursor = findCursor(cursorId);
   if (!cursor)
      return;

   for (auto it = m_waveItemToDisplayNode.begin(); it != m_waveItemToDisplayNode.end(); ++it)
   {
      QTreeWidgetItem* item = it.key();
      DisplayNode* node = it.value();

      if (!item || !node)
         continue;

      if (node->kind == DisplayNodeKind::Group)
      {
         item->setText(SignalTreeWidget::ValueColumn, "");
         continue;
      }

      item->setText(SignalTreeWidget::ValueColumn, node->valueAtTime(cursor->time));
   }
}

void WaveformViewWidget::updateDeltaDisplay()
{
   // Keep status-bar integration outside this widget for now.
}

QString WaveformViewWidget::makeDeltaText() const
{
   const WaveformCursor* c1 = findCursor(Cursor1Id);
   const WaveformCursor* c2 = findCursor(Cursor2Id);

   if (!c1 || !c2)
      return "dt: n/a";

   return QString("C1: %1    C2: %2    dt: %3")
      .arg(formatDisplayTime(c1->time))
      .arg(formatDisplayTime(c2->time))
      .arg(formatDeltaTime(c2->time - c1->time));
}

QString WaveformViewWidget::formatDisplayTime(qreal time) const
{
   const qreal secondsPerUnit = m_timeRuler->secondsPerUnit();
   const qreal displayUnitSeconds = m_timeRuler->currentDisplayUnitSeconds();
   const QString displayUnitName = m_timeRuler->currentDisplayUnitName();

   const qreal seconds = time * secondsPerUnit;
   const qreal displayValue = seconds / displayUnitSeconds;

   const qreal majorStepDisplay =
      (m_timeRuler->majorTickStep() * secondsPerUnit) / displayUnitSeconds;

   int decimals = 2;
   if (majorStepDisplay >= 100.0)
      decimals = 0;
   else if (majorStepDisplay >= 10.0)
      decimals = 1;
   else if (majorStepDisplay >= 1.0)
      decimals = 2;
   else
      decimals = std::clamp(
         int(std::ceil(-std::log10(std::max(majorStepDisplay, 1e-12)))) + 1,
         2,
         6);

   return QString("%1 %2")
      .arg(displayValue, 0, 'f', decimals)
      .arg(displayUnitName);
}

QString WaveformViewWidget::formatDeltaTime(qreal dt) const
{
   const qreal secondsPerUnit = m_timeRuler->secondsPerUnit();
   const qreal displayUnitSeconds = m_timeRuler->currentDisplayUnitSeconds();
   const QString displayUnitName = m_timeRuler->currentDisplayUnitName();

   const qreal seconds = dt * secondsPerUnit;
   const qreal displayValue = seconds / displayUnitSeconds;

   const qreal majorStepDisplay =
      (m_timeRuler->majorTickStep() * secondsPerUnit) / displayUnitSeconds;

   int decimals = 2;
   if (majorStepDisplay >= 100.0)
      decimals = 0;
   else if (majorStepDisplay >= 10.0)
      decimals = 1;
   else if (majorStepDisplay >= 1.0)
      decimals = 2;
   else
      decimals = std::clamp(
         int(std::ceil(-std::log10(std::max(majorStepDisplay, 1e-12)))) + 1,
         2,
         6);

   return QString("%1%2 %3")
      .arg(displayValue >= 0.0 ? "+" : "")
      .arg(displayValue, 0, 'f', decimals)
      .arg(displayUnitName);
}

qreal WaveformViewWidget::scaleX(qreal x) const
{
   return x * m_pixelsPerUnit;
}

qreal WaveformViewWidget::unscaleX(qreal x) const
{
   return x / m_pixelsPerUnit;
}

void WaveformViewWidget::applyZoom(qreal zoomStep, qreal anchorSceneX)
{
   const qreal anchorTime = unscaleX(anchorSceneX);

   m_pixelsPerUnit *= zoomStep;
   m_pixelsPerUnit =
      std::clamp(m_pixelsPerUnit, m_minPixelsPerUnit, m_maxPixelsPerUnit);

   const qreal newAnchorSceneX = scaleX(anchorTime);
   const qreal dx = newAnchorSceneX - anchorSceneX;

   updateScaleOnly();

   const int newScroll =
      int(std::round(m_waveDisplay->horizontalScrollBar()->value() + dx));

   m_waveDisplay->horizontalScrollBar()->setValue(newScroll);
   m_timeRuler->horizontalScrollBar()->setValue(newScroll);
   m_cursorTrack->horizontalScrollBar()->setValue(newScroll);
}

void WaveformViewWidget::zoomFit()
{
   if (!m_document)
      return;

   const qreal endTime = m_document->endTime();
   if (endTime <= 0.0)
      return;

   zoomToTimeRange(0.0, endTime);
}

void WaveformViewWidget::zoomBetweenCursors()
{
   const WaveformCursor* active = findCursor(m_activeCursorId);
   if (!active)
      return;

   const WaveformCursor* other = nearestOtherCursor(active->time);
   if (!other)
      return;

   zoomToTimeRange(active->time, other->time);
}

void WaveformViewWidget::zoomToTimeRange(qreal t0, qreal t1)
{
   if (t1 < t0)
      std::swap(t0, t1);

   const qreal range = t1 - t0;
   if (range <= 0.0)
      return;

   const qreal viewportWidth =
      std::max<qreal>(1.0, m_waveDisplay->viewport()->width());

   m_pixelsPerUnit = viewportWidth / range;
   m_pixelsPerUnit =
      std::clamp(m_pixelsPerUnit, m_minPixelsPerUnit, m_maxPixelsPerUnit);

   updateScaleOnly();

   const int scrollValue = int(std::round(scaleX(t0)));

   m_waveDisplay->horizontalScrollBar()->setValue(scrollValue);
   m_timeRuler->horizontalScrollBar()->setValue(scrollValue);
   m_cursorTrack->horizontalScrollBar()->setValue(scrollValue);
}

QJsonArray WaveformViewWidget::waveLayoutToJson() const
{
   QJsonArray waves;

   for (const auto& node : m_waveDisplayRoots)
   {
      if (node)
         waves.append(node->toJson());
   }

   return waves;
}

bool WaveformViewWidget::loadWaveLayoutFromJson(const QJsonArray& waves)
{
   std::vector<std::unique_ptr<DisplayNode>> newRoots;

   for (const QJsonValue& value : waves)
   {
      if (!value.isObject())
         continue;

      auto node = DisplayNode::fromJson(
         value.toObject(),
         [this](const QString& path)
         {
            return findSignalByPath(path);
         });

      if (node)
         newRoots.push_back(std::move(node));
   }

   m_waveDisplayRoots = std::move(newRoots);

   rebuildWaveNameTreeFromWaveRoots();
   rebuildDisplayLayout();
   updateTreeValuesForActiveCursor();

   return true;
}

const WaveformCursor* WaveformViewWidget::nearestOtherCursor(qreal time) const
{
   const WaveformCursor* best = nullptr;
   qreal bestDist = 0.0;

   for (const auto& cursor : m_cursors)
   {
      if (!cursor.visible || cursor.id == m_activeCursorId)
         continue;

      const qreal d = std::abs(cursor.time - time);
      if (!best || d < bestDist)
      {
         best = &cursor;
         bestDist = d;
      }
   }

   return best;
}

std::unique_ptr<DisplayNode>
WaveformViewWidget::makeSignalDisplayNode(WaveSignal* sig,
                                          bool standaloneBitNames) const
{
   if (!sig)
      return nullptr;

   auto sigNode = std::make_unique<DisplayNode>(sig->name, DisplayNodeKind::Signal);
   sigNode->signal = sig;

   if (sig->width > 1)
   {
      for (int bit = sig->width - 1; bit >= 0; --bit)
      {
         QString name = QString("[%1]").arg(bit);

         if (standaloneBitNames)
            name = QString("%1[%2]").arg(sig->name).arg(bit);

         auto bitNode = std::make_unique<DisplayNode>(name, DisplayNodeKind::Bit);
         bitNode->signal = sig;
         bitNode->msb = bit;
         bitNode->lsb = bit;

         sigNode->addChild(std::move(bitNode));
      }
   }

   return sigNode;
}

// TEMPORARY
void WaveformViewWidget::doFind()
{
   WaveQueryParser parser;
   auto parsed = parser.parse("rx_data == 0b11110000 && tx_data == 0b01011010");

   if (!parsed.ok)
      qDebug() << parsed.error;

   qDebug().noquote() << WaveQueryNode::toString(parsed.root.get(), 0);

   WaveQueryEngine engine(m_document.get());

   auto result = engine.search(parsed.root.get());

   if (!result.ok)
   {
      qDebug() << result.error;
      return;
   }

   for (const auto& m : result.matches)
   {
      qDebug() << "match:"
               << m.startTime
               << "to"
               << m.endTime;
   }

   m_waveDisplay->setQueryMatches(result.matches);
}