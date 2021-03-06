//
//  CardSet.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardSet.h"
#include "BaseCard.h"

CardSet::~CardSet()
{
    clear();
}

void CardSet::insertCard(int cardId, BaseCard* card)
{
    if (0 == _cards.count(cardId)) {
        _cards.insert(std::make_pair(cardId, card));
    } else { CC_ASSERT(false); }
}

void CardSet::removeCard(int cardId, bool cleanup)
{
    if (_cards.find(cardId) != end(_cards)) {
        auto card(_cards.at(cardId));
        if (cleanup) {
            card->removeFromParent();
        }
        
        _cards.erase(cardId);
    }
}

void CardSet::pushPosition(const Point& point)
{
    _positions.push_back(point);
}

void CardSet::clear()
{
    for (auto iter = begin(_cards); iter != end(_cards); ++iter) {
        iter->second->removeFromParent();
    }
    
    _cards.clear();
    _positions.clear();
}

size_t CardSet::getCardsCount() const
{
    return _cards.size();
}

BaseCard* CardSet::getCard(int cardId) const
{
    auto iter(_cards.find(cardId));
    if (iter != end(_cards)) {
        return iter->second;
    }
    
    return nullptr;
}

size_t CardSet::getPositionsCount() const
{
    return _positions.size();
}

const Point& CardSet::getPosition(size_t idx) const
{
    if (_positions.size() > idx) {
        return _positions.at(idx);
    }
    
    CC_ASSERT(false);
    return Point::ZERO;
}
