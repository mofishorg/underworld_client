//
//  GearUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearUpgradeData_h
#define GearUpgradeData_h

#include "AbstractUpgradeData.h"

class RewardData;

class GearUpgradeData : public AbstractUpgradeData
{
public:
    GearUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearUpgradeData();
    
    const RewardData* getSoldReward(int type) const;
    
private:
    std::map<int, RewardData*> _soldRewards;
};

#endif /* GearUpgradeData_h */
