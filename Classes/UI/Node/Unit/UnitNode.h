//
//  UnitNode.h
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UnitNode_h
#define UnitNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Unit;
    class Skill;
} }

class UnitNodeObserver
{
public:
    virtual ~UnitNodeObserver() {}
};

class UnitNode : public Node
{
public:
    static UnitNode* create(const UnderWorld::Core::Unit* unit);
    virtual ~UnitNode();
    void registerObserver(UnitNodeObserver *observer);
    void update();
    
protected:
    UnitNode();
    bool init(const UnderWorld::Core::Unit* unit);
    
private:
    UnitNodeObserver *_observer;
    const UnderWorld::Core::Unit* _unit;
    const UnderWorld::Core::Skill* _lastSkill;
};

#endif /* UnitNode_h */