//
//  CardSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleData_h
#define CardSimpleData_h

#include <string>
#include "json/document.h"
#include "GameModeHMM.h"

namespace UnderWorld {
    namespace Core {
        class HMMCardType;
    }
}

class CardSimpleData
{
public:
    CardSimpleData(const rapidjson::Value& jsonDict);
    virtual ~CardSimpleData();
    
    bool operator==(const CardSimpleData& instance) const;
    
    void update(const rapidjson::Value& jsonDict);
    int getDbId() const;
    int getCardId() const;
    UnderWorld::Core::HMMCardClass getCardClass() const;
    bool isHero() const;
    int getLevel() const;
    int getAmount() const;
    int getCost() const;
    int getQuality() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getUnlockInfo() const;
    
private:
    int _dbId;
    int _cardId;
    int _level;
    int _amount;
    const UnderWorld::Core::HMMCardType* _cardType;
};

#endif /* CardSimpleData_h */
