//
//  UAConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UAConfigData_h
#define UAConfigData_h

#include "CocosGlobal.h"
#include "cocos2d.h"
#include <map>
#include <vector>

USING_NS_CC;

namespace tinyxml2 { class XMLElement; }

// =====================================================
// Animation Parameters
// =====================================================

class AnimationParameters
{
public:
    AnimationParameters()
    :scale(1.0f)
    ,speed(1.0f)
    ,effect_pos(0.0f, 0.0f)
    ,effect_scale(1.0f) {}
    
public:
    float scale;
    float speed;
    Point effect_pos;
    float effect_scale;
};

// =====================================================
// Unit Animation Config Data
// =====================================================

class UAConfigData
{
public:
    UAConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~UAConfigData();
    
    AnimationParameters getAnimationParameters(UnitDirection direction);
    
protected:
    void parse(AnimationParameters& params, const std::vector<std::string>& directions, const std::vector<std::string>& scales, const std::vector<std::string>& speeds, int index);
    
private:
    std::map<UnitDirection, AnimationParameters> _data;
};

#endif /* UAConfigData_h */
