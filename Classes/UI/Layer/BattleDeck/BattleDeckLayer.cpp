//
//  BattleDeckLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleDeckLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "BattleDeckUI.h"
#include "DeckData.h"
#include "CardSimpleData.h"
#include "UniversalBoard.h"
#include "TabButton.h"
#include "UniversalButton.h"

using namespace std;

static const int zorder_top(1);
static const Vec2 secondaryEdge(5, 5);

#if DECKLAYER_ENABLE_TYPE_FILTER
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};
#endif

#pragma mark - BattleDeckLayer
BattleDeckLayer* BattleDeckLayer::create()
{
    auto ret = new (nothrow) BattleDeckLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckLayer::BattleDeckLayer()
:_observer(nullptr)
,_background(nullptr)
#if DECKLAYER_ENABLE_TYPE_FILTER
,_thisCardType(DeckTabType::None)
#endif
,_averageElixirLabel(nullptr)
,_cardPreview(nullptr)
,_deckEditMask(nullptr)
,_usedCard(nullptr)
,_usedCardPoint(Point::ZERO)
,_cardOriginalPoint(Point::ZERO)
,_featureType(DeckManager::FeatureType::Deck)
,_isEditing(false) {}

BattleDeckLayer::~BattleDeckLayer()
{
    CC_SAFE_DELETE(_cardPreview);
    removeAllChildren();
}

void BattleDeckLayer::registerObserver(BattleDeckLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool BattleDeckLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = UniversalBoard::create(2);
        board->setTitle(LocalHelper::getString("ui_deck_battleDeck"));
        board->setExitCallback([this]() {
            DeckManager::getInstance()->saveThisDeckData();
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        _background = board;
        
        createLeftNode(board->getSubNode(0));
        createRightNode(board->getSubNode(1));
        
        // deck
        const int deckId(DeckManager::getInstance()->getThisDeckId());
        loadDeck(deckId);
#if DECKLAYER_ENABLE_TYPE_FILTER
        setCardType(DeckTabType::All);
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BattleDeckLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BattleDeckLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BattleDeckLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BattleDeckLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_isEditing) {
        const auto& point(touch->getLocation());
        bool touched(false);
        for (auto card : _deckCards) {
            if (card && getWorldBoundingBox(card).containsPoint(point)) {
                touched = true;
                break;
            }
        }
        
        if (!touched) {
            useCardCancelled();
        }
    }
}

#pragma mark - DeckCardObserver
void BattleDeckLayer::onDeckCardTouched(DeckCard* touchedCard, ui::Widget::TouchEventType type)
{
    if (touchedCard && _isEditing) {
        if (ui::Widget::TouchEventType::BEGAN == type) {
            _cardOriginalPoint = touchedCard->getPosition();
            touchedCard->setLocalZOrder(zorder_top);
        } else if (ui::Widget::TouchEventType::MOVED == type) {
            const auto& point(touchedCard->getParent()->convertToNodeSpace(touchedCard->getTouchMovePosition()));
            touchedCard->setPosition(point);
        } else {
            auto replacedCard(getIntersectedCard(touchedCard));
            if (touchedCard == _usedCard) {
                if (replacedCard) {
                    useCard(replacedCard, false);
                } else {
                    useCardCancelled();
                }
            } else {
                if (replacedCard) {
                    if (replacedCard == _usedCard) {
                        useCard(touchedCard, true);
                    } else {
                        exchangeCard(touchedCard, replacedCard);
                    }
                } else {
                    exchangeCardCancelled(touchedCard);
                }
            }
        }
    }
}

void BattleDeckLayer::onDeckCardClicked(DeckCard* pSender)
{
    if (_isEditing) {
        if (_usedCard && pSender != _usedCard) {
            useCard(pSender, false);
        }
    } else if (pSender) {
        const int cardId(pSender->getCardId());
        if (!DeckManager::getInstance()->isFound(cardId)) {
            showInfo(cardId);
        } else if (_cardPreview) {
            const bool isCandidate(getFoundCard(cardId));
            vector<DeckCardOpType> types = {DeckCardOpType::Info};
            if (isCandidate) {
                types.push_back(DeckCardOpType::Use);
            }
            _cardPreview->showOpNode(pSender, types);
        }
    }
}

#pragma mark - DeckEditMaskObserver
void BattleDeckLayer::onDeckEditMaskTouched(const Point& point)
{
    if (!_usedCard || !getWorldBoundingBox(_usedCard).containsPoint(point)) {
        useCardCancelled();
    }
}

#pragma mark - CardPreviewObserver
AbstractCard* BattleDeckLayer::onCardPreviewCreateCard(int cardId)
{
    return createCard(cardId);
}

void BattleDeckLayer::onCardPreviewClickedOpButton(DeckCardOpType type, int cardId)
{
    if (DeckCardOpType::Use == type) {
        beginEdit(cardId);
    } else if (DeckCardOpType::Info == type) {
        showInfo(cardId);
    }
}

#pragma mark - CardInfoLayerObserver
void BattleDeckLayer::onCardInfoLayerReturn(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void BattleDeckLayer::onCardInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
    
    removeFromParent();
}

#pragma mark - UI
void BattleDeckLayer::createLeftNode(Node* node)
{
    if (_background && node) {
        node->setLocalZOrder(zorder_top);
        
        const auto& subSize(node->getContentSize());
        static const Size topBarSize(subSize.width - secondaryEdge.x * 2, 60);
        
        // top
        {
            auto bar = CocosUtils::createPureBar(topBarSize);
            bar->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + topBarSize.height / 2));
            node->addChild(bar);
            
            static const float edgeX(5);
            auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_battleDeck"), DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(edgeX, topBarSize.height / 2);
            bar->addChild(label);
            
            float x(edgeX);
            for (int i = 0; i < DeckManager::DecksMaxCount; ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_2.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_3.png"));
                const auto idx(DeckManager::DecksMaxCount - i - 1);
                const string title = StringUtils::format("%d", idx + 1);
                auto button = TabButton::create(title, normal, selected, [this, idx](Ref*) {
                    if (!_isEditing) {
                        DeckManager::getInstance()->saveThisDeckData();
                        loadDeck(idx);
                    }
                });
                bar->addChild(button);
                _deckTabButtons.insert(make_pair(idx, button));
                
                const auto& size = button->getContentSize();
                button->setPosition(topBarSize.width - (x + size.width / 2), topBarSize.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
        }
        
        // heroes
        {
            static const float spaceBarLine(13);
            auto line = createLine(true);
            line->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + topBarSize.height + spaceBarLine));
            node->addChild(line);
            
            static const float spaceLineCard(13);
            static const int column(DeckData::HeroCount);
            for (int i = 0; i < column; ++i) {
                static const float cardSpaceX(20);
                static const float basePosX((subSize.width - (column * DeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                auto card = createCard(0);
                const float x = basePosX + (i + 0.5) * DeckCard::Width + i * cardSpaceX;
                const float y = line->getPositionY() - (line->getContentSize().height / 2 + spaceLineCard + DeckCard::Height / 2);
                card->setPosition(Point(x, y));
                node->addChild(card);
                _deckCards.push_back(card);
            }
        }
        
        static const Size bottomBarSize(topBarSize.width, 36);
        
        //
        {
            auto bar = CocosUtils::createPureBar(bottomBarSize);
            bar->setPosition(subSize.width / 2, bottomBarSize.height / 2 + secondaryEdge.y);
            node->addChild(bar, zorder_top);
            
            auto label = CocosUtils::createLabel("4.0", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            label->setPosition(bottomBarSize.width / 2, bottomBarSize.height / 2);
            bar->addChild(label);
            _averageElixirLabel = label;
            
            auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Purple, LocalHelper::getString("ui_deck_move"));
            button->setCallback([this](Ref*) {
                if (!_isEditing) {
                    beginEdit(0);
                }
            });
            button->setPosition(bottomBarSize.width - button->getContentSize().width / 2, bottomBarSize.height / 2);
            bar->addChild(button);
        }
        
        // soldiers
        {
            static const float spaceLineCard(13);
            static const float spaceBarCard(5);
            static const int row(2);
            static const int column((DeckData::SoldierCount - 1) / row + 1);
            static const float cardSpaceY(13);
            
            static float posY(secondaryEdge.y + bottomBarSize.height + row * DeckCard::Height + (row - 1) * cardSpaceY + spaceLineCard + spaceBarCard);
            auto line = createLine(false);
            line->setPosition(subSize.width / 2, posY);
            node->addChild(line);
            
            for (int i = 0; i < row; ++i) {
                static const float basePosY(posY - spaceLineCard);
                for (int j = 0; j < column; ++j) {
                    static const float cardSpaceX(5);
                    static const float basePosX((subSize.width - (column * DeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                    auto card = createCard(0);
                    const float x = basePosX + (j + 0.5) * DeckCard::Width + j * cardSpaceX;
                    const float y = basePosY - (i + 0.5) * DeckCard::Height - i * cardSpaceY;
                    card->setPosition(Point(x, y));
                    node->addChild(card);
                    _deckCards.push_back(card);
                }
            }
        }
    }
}

void BattleDeckLayer::createRightNode(Node* node)
{
    if (_background && node && !_cardPreview) {
        _cardPreview = new (nothrow) CardPreview(_featureType, node, this);
    }
}

DeckCard* BattleDeckLayer::createCard(int card)
{
    auto node = DeckCard::create(card);
    node->registerObserver(this);
    return node;
}

Node* BattleDeckLayer::createLine(bool isHero) const
{
    string file;
    string text;
    if (isHero) {
        file = "ui_line_1.png";
        text = LocalHelper::getString("ui_deck_tab_hero");
    } else {
        file = "ui_line_2.png";
        text = LocalHelper::getString("ui_deck_tab_soldier");
    }
    
    auto line = Sprite::create(CocosUtils::getResourcePath(file));
    auto label = CocosUtils::createLabel(text, DEFAULT_FONT_SIZE);
    label->setTextColor(Color4B::BLACK);
    label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    label->setPosition(line->getContentSize().width / 2, line->getContentSize().height / 2);
    line->addChild(label);
    return line;
}

void BattleDeckLayer::updateAverageElixir()
{
    if (_averageElixirLabel) {
        float total(0);
        for (auto card : _deckCards) {
            if (card) {
                auto data = DeckManager::getInstance()->getCardData(card->getCardId());
                if (data) {
                    total += data->getCost();
                }
            }
        }
        
        _averageElixirLabel->setString(LocalHelper::getString("ui_deck_elixirCost") + StringUtils::format("%.1f", total / DeckData::SoldierCount));
    }
}

#pragma mark - Info
void BattleDeckLayer::showInfo(int cardId)
{
    auto layer = CardInfoLayer::create(cardId);
    layer->registerObserver(this);
    addChild(layer);
}

#pragma mark - Move cards
void BattleDeckLayer::beginEdit(int cardId)
{
    CC_ASSERT(!_isEditing);
    _isEditing = true;
    
    // create mask
    if (_background && !_deckEditMask) {
        auto node(_background->getSubNode(1));
        if (node) {
            const auto& size(node->getContentSize());
            _deckEditMask = DeckEditMask::create(size);
            _deckEditMask->registerObserver(this);
            _deckEditMask->setPosition(node->getPosition() - Point(size.width / 2, size.height / 2));
            _background->addChild(_deckEditMask);
        }
    }
    
    // create card
    _usedCard = getFoundCard(cardId);
    
    if (_usedCard) {
        auto parent = _deckCards.front()->getParent();
        BattleDeckUI::readdChild(parent, _usedCard);
        _usedCardPoint = _usedCard->getPosition();
        
        auto dm(DeckManager::getInstance());
        auto ud(dm->getCardData(_usedCard->getCardId()));
        if (ud) {
            vector<DeckCard*> temp;
            if (ud->isHero()) {
                for (int i = 0; i < DeckData::HeroCount; ++i) {
                    temp.push_back(_deckCards.at(i));
                }
            } else {
                for (int i = DeckData::HeroCount; i < _deckCards.size(); ++i) {
                    temp.push_back(_deckCards.at(i));
                }
            }
            temp.push_back(_usedCard);
            shake(temp);
        } else { CC_ASSERT(false); }
    } else {
        shake({_deckCards});
    }
}

void BattleDeckLayer::endEdit()
{
    _isEditing = false;
    
    if (_deckEditMask) {
        _deckEditMask->removeFromParent();
        _deckEditMask = nullptr;
    }
    
    stopShake();
}

DeckCard* BattleDeckLayer::getFoundCard(int cardId) const
{
    if (_cardPreview) {
        return dynamic_cast<DeckCard*>(_cardPreview->getFoundCard(cardId));
    }
    
    return nullptr;
}

void BattleDeckLayer::exchangeCard(DeckCard* from, DeckCard* to)
{
    if (from && to) {
        auto dm(DeckManager::getInstance());
        auto fd(dm->getCardData(from->getCardId()));
        auto td(dm->getCardData(to->getCardId()));
        if (fd && td) {
            if (fd->isHero() == td->isHero()) {
                BattleDeckUI::move(from, to->getPosition(), moveDuration, nullptr);
                BattleDeckUI::move(to, _cardOriginalPoint, moveDuration, nullptr);
                
                DeckManager::getInstance()->exchangeCard(from->getCardId(), to->getCardId(), [=](int idxFrom, int idxTo) {
                    const ssize_t cnt(_deckCards.size());
                    if (cnt > idxFrom && cnt > idxTo) {
                        _deckCards.at(idxFrom) = to;
                        _deckCards.at(idxTo) = from;
                    }
                });
            } else {
                exchangeCardCancelled(from);
                MessageBox("英雄与士兵卡牌无法互换", nullptr);
            }
        } else { CC_ASSERT(false); }
    }
}

void BattleDeckLayer::exchangeCardCancelled(DeckCard* card)
{
    if (card) {
        BattleDeckUI::move(card, _cardOriginalPoint, moveDuration, nullptr);
    }
}

void BattleDeckLayer::useCard(DeckCard* replaced, bool fromDeck)
{
    CC_ASSERT(_usedCard);
    if (_usedCard && replaced && replaced->getParent()) {
        const auto uid(_usedCard->getCardId());
        const auto rid(replaced->getCardId());
        auto dm(DeckManager::getInstance());
        auto ud(dm->getCardData(uid));
        auto rd(dm->getCardData(rid));
        if (ud && rd) {
            if (ud->isHero() == rd->isHero()) {
                BattleDeckUI::readdChild(replaced->getParent(), _usedCard);
                
                Point point(Point::ZERO);
                if (fromDeck) {
                    point = _cardOriginalPoint;
                } else {
                    point = replaced->getPosition();
                }
                BattleDeckUI::move(_usedCard, point, moveDuration, nullptr);
                
                if (_cardPreview) {
                    _cardPreview->readdToScrollView(replaced);
                }
                
                endEdit();
                
                updateAverageElixir();
                
                if (_cardPreview) {
                    _cardPreview->removeFoundCard(uid, false);
                    _cardPreview->insertFoundCard(rid, replaced);
                }
                
                DeckManager::getInstance()->useCard(uid, rid, [this](int idx) {
                    _deckCards.at(idx) = _usedCard;
                    if (_cardPreview) {
                        _cardPreview->sortAndRealign();
                    }
                });
            } else {
                if (fromDeck) {
                    exchangeCardCancelled(replaced);
                } else {
                    useCardCancelled();
                }
                MessageBox("英雄与士兵卡牌无法互换", nullptr);
            }
        }
    }
}

void BattleDeckLayer::useCardCancelled()
{
    const bool isMoving(_usedCard && _usedCardPoint != _usedCard->getPosition());
    if (_usedCard) {
        BattleDeckUI::move(_usedCard, _usedCardPoint, moveDuration, [=]() {
            if (_cardPreview) {
                _cardPreview->readdToScrollView(_usedCard);
            }
            
            if (isMoving) {
                resetParams();
            }
        });
    }
    
    endEdit();
    
    if (!isMoving) {
        resetParams();
    }
}

#pragma mark - Universal Methods
void BattleDeckLayer::shake(const vector<DeckCard*>& nodes) const
{
    for (auto node : nodes) {
        if (node) {
            node->shake();
        }
    }
}

void BattleDeckLayer::stopShake()
{
    vector<vector<DeckCard*>> nodes = {
        _deckCards, {_usedCard}
    };
    
    for (int i = 0; i < nodes.size(); ++i) {
        auto& set(nodes.at(i));
        for (auto iter = begin(set); iter != end(set); ++iter) {
            auto node(*iter);
            if (node) {
                node->stopShake();
            }
        }
    }
}

DeckCard* BattleDeckLayer::getIntersectedCard(const DeckCard* touchedCard) const
{
    if (touchedCard) {
        auto card = getIntersectedCard(touchedCard, _deckCards);
        if (card) {
            return card;
        } else if (_usedCard) {
            return getIntersectedCard(touchedCard, {_usedCard});
        }
    }
    
    return nullptr;
}

DeckCard* BattleDeckLayer::getIntersectedCard(const DeckCard* touchedCard, const vector<DeckCard*>& cards) const
{
    if (nullptr == touchedCard || 0 == cards.size()) {
        return nullptr;
    }
    
    DeckCard* ret(nullptr);
    float intersectedArea(INT_MAX);
    const auto& rect(getWorldBoundingBox(touchedCard));
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& bd(getWorldBoundingBox(*iter));
        if (touchedCard != (*iter) && bd.intersectsRect(rect)) {
            if (cards.size() == 1) {
                ret = *iter;
                break;
            } else {
                const auto& size = bd.unionWithRect(rect).size;
                float area = size.width * size.height;
                if (intersectedArea > area) {
                    intersectedArea = area;
                    ret = *iter;
                }
            }
        }
    }
    
    return ret;
}

Rect BattleDeckLayer::getWorldBoundingBox(const Node* node) const
{
    if (node && node->getParent()) {
        Rect rect(node->getBoundingBox());
        rect.origin = node->getParent()->convertToWorldSpace(rect.origin);
        return rect;
    }
    
    return Rect::ZERO;
}

#pragma mark - Functions
void BattleDeckLayer::loadDeck(int idx)
{
    if (true) {
        // tab buttons
        for (auto iter = begin(_deckTabButtons); iter != end(_deckTabButtons); ++iter) {
            iter->second->setEnabled(idx != iter->first);
        }
        
        // load data
        auto dm(DeckManager::getInstance());
        dm->loadDeck(idx);
        if (_cardPreview) {
            _cardPreview->sortAndRealign();
        }
        
        // update cards
        bool find(false);
        const auto& cards(dm->getThisDeckData()->getCards());
        for (int i = 0; i < cards.size(); ++i) {
            if (i < _deckCards.size()) {
                const auto cardId(cards.at(i));
                _deckCards.at(i)->update(cardId);
                if (_cardPreview) {
                    _cardPreview->removeFoundCard(cardId, true);
                }
                
                if (!find && _cardPreview) {
                    find = _cardPreview->setOpNodePosition(_deckCards.at(i));
                }
            } else { CC_ASSERT(false); }
        }
        
        if (!find && _cardPreview) {
            _cardPreview->hideOpNode();
        }
        
        for (int i = 0; i < _deckCards.size(); ++i) {
            auto card(_deckCards.at(i));
            static const int actionTag(1100);
            if (card->getActionByTag(actionTag)) {
                card->stopActionByTag(actionTag);
            }
            
            card->setVisible(false);
            auto action = Sequence::create(DelayTime::create(0.03 * (i + 1)), Show::create(), nullptr);
            action->setTag(actionTag);
            card->runAction(action);
        }
        
        updateAverageElixir();
    }
}

void BattleDeckLayer::resetParams()
{
    _cardOriginalPoint = Point::ZERO;
    _usedCard = nullptr;
    _usedCardPoint = Point::ZERO;
}

#if DECKLAYER_ENABLE_TYPE_FILTER
void BattleDeckLayer::setCardType(DeckTabType type)
{
    if (_thisCardType != type) {
        _thisCardType = type;
        
        const auto info(_candidates.at(type));
        loadCards(info);
        
        // tab buttons
        for (int i = 0; i < _cardTabButtons.size(); ++i) {
            _cardTabButtons.at(i)->setEnabled(type != cardTabs[i]);
        }
    }
}

string BattleDeckLayer::getCardTabName(DeckTabType type) const
{
    switch (type) {
        case DeckTabType::All:
            return LocalHelper::getString("ui_deck_tab_all");
        case DeckTabType::Heroes:
            return LocalHelper::getString("ui_deck_tab_hero");
        case DeckTabType::Soldiers:
            return LocalHelper::getString("ui_deck_tab_soldier");
        case DeckTabType::Spells:
            return LocalHelper::getString("ui_deck_tab_spell");
        default:
            return "";
    }
}
#endif
