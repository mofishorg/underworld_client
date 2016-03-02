//
//  MapUICardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUICardDeck.h"
#include "Camp.h"
#include "CocosUtils.h"

using namespace std;
using namespace UnderWorld::Core;

static const unsigned int resourceMaxCount(10);
static const int topZOrder(1);

MapUICardDeck* MapUICardDeck::create(const vector<const Camp*>& camps)
{
    MapUICardDeck *ret = new (nothrow) MapUICardDeck();
    if (ret && ret->init(camps))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUICardDeck::MapUICardDeck()
:_observer(nullptr)
,_candidateSprite(nullptr)
,_nextLabel(nullptr)
,_countLabel(nullptr)
{
    
}

MapUICardDeck::~MapUICardDeck()
{
    removeAllChildren();
}

bool MapUICardDeck::init(const vector<const Camp*>& camps)
{
    if (Node::init())
    {
        _camps = camps;
        
        static const float x1(10.0f);
        static const float x2(20.0f);
        static const float x3(8.0f);
        
        static const float y1(5.0f);
        static const float y2(50.0f);
        
        const Size& nodeSize = MapUIUnitNode::create(nullptr)->getContentSize();
        const Size size(x1 * 2 + x2 + (CARD_DECKS_COUNT + 1) * nodeSize.width + (CARD_DECKS_COUNT - 1) * x3, y1 + y2 + nodeSize.height);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        // background
        Rect rect(0, 0, 91, 157);
        static const float capInsets(18.0f);
        Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
        Scale9Sprite* background = Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
        background->setContentSize(size);
        background->setPosition(Point(size.width / 2, size.height / 2));
        addChild(background);
        
        // children
        const float y = y2 + nodeSize.height / 2;
        for (int i = CARD_DECKS_COUNT - 1; i >= 0; --i) {
            const float x = x1 + x2 + nodeSize.width * (i + 1.5f) + x3 * i;
            _unitPositions.push_back(Point(x, y));
        }
        
        _candidateSprite = Sprite::create("GameImages/test/ui_kapaibeiian.png");
        _candidateSprite->setPosition(x1 + nodeSize.width / 2, y);
        background->addChild(_candidateSprite);
        
        _nextLabel = CocosUtils::createLabel("Next:0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _nextLabel->setPosition(_candidateSprite->getPosition().x, y2 / 2);
        background->addChild(_nextLabel, topZOrder);
        
        _countLabel = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(_countLabel, topZOrder);
        
        Label* maxCountLabel = CocosUtils::createLabel(StringUtils::format("Max:%d", resourceMaxCount), BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(maxCountLabel, topZOrder);
        
        Size progressSize(Size::ZERO);
        for (int i = 0; i < resourceMaxCount; ++i) {
            Sprite* s = Sprite::create("GameImages/test/ui_blood_8.png");
            ProgressTimer* pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::BAR);
            pt->setBarChangeRate(Vec2(1.0f, 0.0f));
            pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
            pt->setPercentage(100);
            background->addChild(pt);
            _resources.push_back(pt);
            
            if (progressSize.width == 0) {
                progressSize = pt->getContentSize();
            }
        }
        
        static const float offsetX(1.0f);
        const float midX = x1 + x2 + nodeSize.width + (CARD_DECKS_COUNT * (nodeSize.width + x3) - x3) / 2;
        const float startX = midX - ((progressSize.width + offsetX) * resourceMaxCount - offsetX)  / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            ProgressTimer* pt = _resources.at(i);
            const Point pos(startX + progressSize.width / 2 + (progressSize.width + offsetX) * i, y2 / 2);
            pt->setPosition(pos);
        }
        
        _countLabel->setPosition(startX, y2 / 2);
        maxCountLabel->setPosition(midX, (y2 - progressSize.height) / 2);
        
        return true;
    }
    
    return false;
}

void MapUICardDeck::registerObserver(MapUICardDeckObserver *observer)
{
    _observer = observer;
}

void MapUICardDeck::select(const Camp* selectedCamp)
{
    for (int i = 0; i < _unitNodes.size(); ++i) {
        MapUIUnitNode* node = _unitNodes.at(i);
        if (node) {
            node->setSelected(selectedCamp == node->getCamp());
        }
    }
}

void MapUICardDeck::updateTimer(float time)
{
    if (_nextLabel) {
        _nextLabel->setString(StringUtils::format("Next:%d", static_cast<int>(time)));
    }
}

void MapUICardDeck::updateResource(float count)
{
    const size_t cnt(_resources.size());
    count = MIN(MAX(0, count), cnt);
    
    for (int i = 0; i < cnt; ++i) {
        ProgressTimer* pt = _resources.at(i);
        if (pt) {
            if (i <= count - 1) {
                pt->setPercentage(100.0f);
            } else if (i < count) {
                pt->setPercentage(100.0f * (count - i));
            } else {
                pt->setPercentage(0);
            }
        }
    }
    
    if (_countLabel) {
        _countLabel->setString(StringUtils::format("%d", static_cast<int>(count)));
    }
}

void MapUICardDeck::initial(const vector<const Camp*>& camps)
{
    for (int i = 0; i < _unitNodes.size(); ++i) {
        _unitNodes.at(i)->removeFromParent();
    }
    _unitNodes.clear();
    
    const size_t cnt(_unitPositions.size());
    for (int i = 0; i < camps.size(); ++i) {
        if (i < cnt) {
            createUnitNode(camps.at(i), i);
        }
    }
}

void MapUICardDeck::insert(const Camp* camp)
{
    createUnitNode(camp, _unitNodes.size());
}

void MapUICardDeck::remove(const Camp* camp)
{
    if (camp) {
        bool update(false);
        for (auto iter = begin(_unitNodes); iter != end(_unitNodes); ++iter) {
            if ((*iter)->getCamp() == camp) {
                _unitNodes.erase(iter);
                update = true;
                break;
            }
        }
        
        if (update) {
            reload();
        }
    }
}

#pragma mark - MapUIUnitNodeObserver
void MapUICardDeck::onMapUIUnitNodeTouchedBegan(const Camp* camp)
{
    if (_observer) {
        _observer->onMapUICardDeckUnitTouchedBegan(camp);
    }
}

void MapUICardDeck::onMapUIUnitNodeTouchedEnded(const Camp* camp, bool isValid)
{
    if (isValid && _observer) {
        _observer->onMapUICardDeckUnitTouchedEnded(camp);
    }
}

void MapUICardDeck::createUnitNode(const Camp* camp, size_t idx)
{
    const size_t cnt(_unitPositions.size());
    if (idx < cnt && _unitNodes.size() < cnt) {
        MapUIUnitNode* node = MapUIUnitNode::create(camp);
        node->registerObserver(this);
        node->setPosition(_unitPositions.at(idx));
        addChild(node);
        _unitNodes.push_back(node);
    }
}

void MapUICardDeck::reload()
{
    const size_t cnt(_unitPositions.size());
    for (int i = 0; i < _unitNodes.size(); ++i) {
        if (i < cnt) {
            MapUIUnitNode* node = _unitNodes.at(i);
            const Point& position = _unitPositions.at(i);
            node->setPosition(position);
        }
    }
}
