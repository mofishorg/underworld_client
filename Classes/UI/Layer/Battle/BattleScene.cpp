//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "CocosUtils.h"
#include "GameSettings.h"
#include "DataManager.h"
#include "SoundManager.h"
#include "TechTree.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

BattleScene* BattleScene::create(int mapId)
{
    BattleScene *ret = new (std::nothrow) BattleScene();
    if (ret && ret->init(mapId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleScene::BattleScene()
:_mapId(0)
,_render(nullptr)
,_looper(nullptr)
,_sch(nullptr)
,_techTree(nullptr)
{
    
}

BattleScene::~BattleScene()
{
    clear();
    CocosUtils::cleanMemory();
}

// on "init" you need to initialize your instance
bool BattleScene::init(int mapId)
{
    if (Scene::init() )
    {
        _mapId = mapId;
        return true;
    }
    
    return false;
}

void BattleScene::onEnter()
{
    Scene::onEnter();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(true);
#endif
    start();
}

void BattleScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(false);
#endif
    Scene::onExit();
}

#pragma mark - GameRenderObserver
void BattleScene::onGameRenderRestart()
{
    start();
}

void BattleScene::start()
{
    if (_render) {
        clear();
    }
    
    // 1. add map layer
    string mapSettingXml = DataManager::getInstance()->getMapData(_mapId);
    
    // 2. add map ui layer
    _render = new (nothrow) GameRender(this, _mapId, mapSettingXml, "Vampire");
    _render->registerObserver(this);
    
    _sch = new (nothrow) GameScheduler();
    
    // 3. game setting
    UnderWorld::Core::GameSettings setting;
    
    // 3.1. set map setting
    setting.setMap(_render->getMapSetting());
    
    // 3.2. set techTree;
    std::string techTreeData = DataManager::getInstance()->getTechTreeData(1);
    setting.setTechTree(techTreeData);
    if (!_techTree) {
        _techTree = new (std::nothrow) UnderWorld::Core::TechTree();
        _techTree->init(techTreeData);
    }
    
    // 3.3. set faction data
    setting.setFactionCount(2);
    setting.setThisFactionIndex(0);
    
    if (true) {
        const bool isWerewolf = true;
        
        {
            static const string wereWolf("狼人族");
            static const string vampire("吸血鬼族");
            setting.setFactionTypeKey(0, isWerewolf ? wereWolf : vampire);
            setting.setFactionControlType(0, UnderWorld::Core::kFactionControlType_Human);
            setting.setTeam(0, 0);
            setting.setMapIndex(0, 0);
            
            setting.setFactionTypeKey(1, isWerewolf ? vampire : wereWolf);
            setting.setFactionControlType(1, UnderWorld::Core::kFactionControlType_Cpu);
            setting.setTeam(1, 1);
            setting.setMapIndex(1, 1);
        }
        
        {
            static const string wereWolfCore("狼人基地");
            static const string wereWolfTower("狼人箭塔");
            static const string vampireCore("吸血鬼基地");
            static const string vampireTower("吸血鬼箭塔");
            // 3.4. set core & tower
            UnderWorld::Core::UnitSetting core0;
            core0.setUnitTypeName(isWerewolf ? wereWolfCore : vampireCore);
            setting.setCore(0, core0);
            UnderWorld::Core::UnitSetting core1;
            core1.setUnitTypeName(isWerewolf ? vampireCore : wereWolfCore);
            setting.setCore(1, core1);
            UnderWorld::Core::UnitSetting tower0;
            tower0.setUnitTypeName(isWerewolf ? wereWolfTower : vampireTower);
            setting.setTower(0, tower0);
            UnderWorld::Core::UnitSetting tower1;
            tower1.setUnitTypeName(isWerewolf ? vampireTower : wereWolfTower);
            setting.setTower(1, tower1);
        }
        
        // TODO: set deck
        {
            
        }
    }
    
    _looper = new (nothrow) UnderWorld::Core::GameLooper(_render, _sch);
    _looper->init(setting);
    _looper->start();
}

void BattleScene::clear()
{
    if (_looper) {
        _looper->end();
    }
    CC_SAFE_DELETE(_looper);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    CC_SAFE_DELETE(_techTree);
    removeAllChildren();
}
