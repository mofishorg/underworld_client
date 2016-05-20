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
#include "UserDefaultsDataManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif
#include "GameData.h"

BattleScene* BattleScene::create(int mapId)
{
    BattleScene *ret = new (nothrow) BattleScene();
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
,_client(nullptr)
,_sch(nullptr)
, _proxy(nullptr)
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
    
    // 2. add map ui layer
    _render = new (nothrow) GameRender(this, "Vampire");
    _render->registerObserver(this);
    
    _sch = new (nothrow) GameScheduler();
    _proxy = new (nothrow) ClientTCPNetworkProxy("192.168.31.100", 9999, "demoplayer", GameData::getInstance()->getUUID());
//    _proxy = new (nothrow) ClientTCPNetworkProxy("123.57.221.242", 8888, "demoplayer", GameData::getInstance()->getUUID());

    // 3. game setting
    UnderWorld::Core::GameContentSetting contentSetting;
    
    contentSetting.setFactionTypeKey("狼人族");
    
    UnderWorld::Core::UnitSetting core;
    core.setUnitTypeName("狼人基地");
    contentSetting.setCore(core);
    
    UnderWorld::Core::UnitSetting tower;
    tower.setUnitTypeName("狼人箭塔");
    contentSetting.setTower(tower);
    
    set<string> cardSet;
    std::vector<string> cards;
    UserDefaultsDataManager::getInstance()->getSelectedCards(cardSet);
    for (auto iter = cardSet.begin(); iter != cardSet.end(); ++iter) {
        cards.push_back(*iter);
    }
    
    _client = new (nothrow) UnderworldClient("mofish", _proxy, _sch, _render);
    _client->launchPve(_mapId, contentSetting, cards);
//    _client->launchPvp(contentSetting);
}

void BattleScene::clear()
{
    if (_client) {
        _client->quit();
    }
    CC_SAFE_DELETE(_client);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    CC_SAFE_DELETE(_proxy);
    removeAllChildren();
}
