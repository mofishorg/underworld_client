//
//  SkillData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillData.h"
#include "DataManager.h"
#include "SkillProperty.h"

static rapidjson::Document createFakeData(int skillId, int level)
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("id", skillId, allocator);
    document.AddMember("level", level, allocator);
    return document;
}

SkillData::SkillData(int skillId, int level)
:AbstractData(createFakeData(skillId, level)) {}

SkillData::~SkillData() {}

void SkillData::update(int level)
{
    _level = level;
}
