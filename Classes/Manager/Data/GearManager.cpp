//
//  GearManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "GearData.h"

using namespace std;
using namespace cocostudio;

GearManager::GearManager()
{
}

GearManager::~GearManager()
{
    Utils::clearVector(_gears);
}

void GearManager::initGear(const rapidjson::Value& jsonDict)
{
    
}

void GearManager::upgradeGear()
{
    
}

void GearManager::recastGear()
{
    
}

void GearManager::sellGear()
{
    
}