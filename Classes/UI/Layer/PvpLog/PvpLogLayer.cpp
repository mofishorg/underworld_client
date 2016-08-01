//
//  PvpLogLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "PvpLogUI.h"
#include "PvpManager.h"
#include "XTableViewCell.h"

using namespace std;
using namespace ui;

static const Size subNodeSize(992, 513);
static const float tableEdgeY((subNodeSize.width - PvpLogNode::Width) / 2);
static const float tableNodeGapY(14);
static const float tableCellWidth(subNodeSize.width);
static const Size tableMaxSize(tableCellWidth, subNodeSize.height - tableEdgeY * 2);
static const Point tableLeftTopPosition(0, subNodeSize.height - tableEdgeY);

PvpLogLayer* PvpLogLayer::create()
{
    auto ret = new (nothrow) PvpLogLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PvpLogLayer::PvpLogLayer()
:_observer(nullptr)
,_background(nullptr)
,_table(nullptr)
,_expandedIdx(0) {}

PvpLogLayer::~PvpLogLayer()
{
    removeAllChildren();
}

#pragma mark - public
void PvpLogLayer::registerObserver(PvpLogLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool PvpLogLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        
        auto bg = Sprite::create(PvpLogUI::getResourcePath("ui_background_5.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        _background = bg;
        
        auto subNode = CocosUtils::createBackground("GameImages/public/ui_background_1.png", subNodeSize);
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const auto& subBgSize(subNode->getContentSize());
        const float edge((size.width - subBgSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subBgSize.height / 2 + edge));
        
        CocosUtils::createRedExitButton(bg, [this]() {
            removeFromParent();
        });
        
        auto title = CocosUtils::createLabel("Share", BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subBgSize.height + edge) / 2));
        bg->addChild(title);
        
        createTable(subNode);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(PvpLogLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(PvpLogLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool PvpLogLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void PvpLogLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDataSource
Size PvpLogLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    float h(getNodeHeight(idx) + tableNodeGapY);
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        h += tableNodeGapY / 2;
    }
    
    return Size(tableCellWidth, h);
}

TableViewCell* PvpLogLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const auto& allData(getData());
    auto cnt = getCellsCount();
    static const float nodeIdx(0);
    auto node = dynamic_cast<PvpLogNode*>(cell->getNode(nodeIdx));
    if (idx < cnt) {
        const auto& data(allData.at(idx));
        const bool expanded(_expandedIdx == idx);
        if (!node) {
            node = PvpLogNode::create(data, expanded);
            node->registerObserver(this);
            cell->addChild(node);
            cell->setNode(node, nodeIdx);
        } else {
            node->update(data, expanded);
        }
        
        // we must update the position when the table was reloaded
        const Point point(tableCellWidth / 2, getNodeHeight(idx) * 0.5f + tableNodeGapY / 2);
        node->setPosition(point + Point(0, (idx == cnt - 1) ? tableNodeGapY / 2: 0));
    } else if (node) {
        node->removeFromParent();
        cell->resetNode(nodeIdx);
    }
    
    return cell;
}

ssize_t PvpLogLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - table
void PvpLogLayer::createTable(Node* parent)
{
    auto table = TableView::create(this, tableMaxSize);
    table->setDirection(extension::ScrollView::Direction::VERTICAL);
    table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    table->setBounceable(false);
    parent->addChild(table);
    
    _table = table;
    refreshTable(false);
    table->setContentOffset(Point::ZERO);
}

void PvpLogLayer::refreshTable(bool reload)
{
    if (_table) {
        auto cnt(getCellsCount());
        auto height = (getNodeHeight(CC_INVALID_INDEX) + tableNodeGapY) * cnt + tableNodeGapY;
        if (_expandedIdx != CC_INVALID_INDEX) {
            height += PvpLogNode::ExpandedHeight - PvpLogNode::FoldedHeight;
        }
        auto size = Size(tableMaxSize.width, MIN(height, tableMaxSize.height));
        _table->setViewSize(size);
        _table->setPosition(tableLeftTopPosition - Point(0, size.height));
        
        if (reload) {
            const auto& offset = _table->getContentOffset();
            _table->reloadData();
            _table->setContentOffset(offset);
        }
    }
}

ssize_t PvpLogLayer::getCellsCount() const
{
    return getData().size();
}

float PvpLogLayer::getNodeHeight(ssize_t idx) const
{
    return (_expandedIdx == idx) ? PvpLogNode::ExpandedHeight : PvpLogNode::FoldedHeight;
}

const vector<PvpLogData*>& PvpLogLayer::getData() const
{
    return PvpManager::getInstance()->getPvpLogData();
}