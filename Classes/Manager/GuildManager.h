//
//  GuildManager.h
//  Underworld_Client
//
//  Created by Andy on 15/11/20.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GuildManager_h
#define GuildManager_h

#include "cocos2d.h"
#include "json/document.h"

class GuildManager
{
public:
    GuildManager();
    virtual ~GuildManager();
    
    void initGuild(const rapidjson::Value& jsonDict);
    
protected:
    CC_DISALLOW_COPY_AND_ASSIGN(GuildManager);
};

#endif /* GuildManager_h */
