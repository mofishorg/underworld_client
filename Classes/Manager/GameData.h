//
//  GameData.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GameData_h
#define GameData_h

#include "network/HttpClient.h"
#include "User.h"
#include "CocosGlobal.h"

class IapObject;

class GameData
{
public:
    static GameData * getInstance();
    static void purge();
    
    void init() const;
    void parseFiles() const;
    
    User* currentUser() const;
    
    void setDeviceToken(const std::string& token);
    unsigned int getUUID() const;
    const std::string& getVersionId() const;
    
    // auto login (if a token is exist in local)
    void autoLogin(const httpRequestCallback& success, const httpErrorCallback& failed);
    
    // guest account
    void login(const httpRequestCallback& success, const httpErrorCallback& failed);
    bool isLoggedIn() const;
    
    // iap
    bool isTransacting() const;
    void beginTransaction();
    void finishTransaction();
    void fetchIAPInfo(const httpRequestCallback& success, const httpRequestCallback& failed, bool showLoadingLayer);
    const std::vector<IapObject *>& getIapObjects() const;
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    void loadAnonymousUser(rapidjson::Document& document);
    void saveAnonymousUser(const User* user);
    
#endif  // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    
    // heartbeat
    void startHeartBeatSynchronously();
    void scheduleHeartBeat();
    void heartBeat(float dt);
    void heartBeatRequest(bool showLoadingView);
    void stopHeartBeat();
    
    // callbacks
    void onLogout();
    void onUpdateVersionID(const std::string& version);
    void onNetworkResponseFailed(long code, const std::string& jsonPath) const;
    void onVersionError();
    void onUserIsOnline();
    void onUserIsOffline();
    
private:
    GameData();
    virtual ~GameData();
    
    void generateUUID();
    // login
    void requestLogin(const httpRequestCallback& success, const httpErrorCallback& failed);
    void handleLoginResponse(cocos2d::network::HttpResponse* response, const httpRequestCallback& success, const httpErrorCallback& failed);
    void reloadGame();
    
private:
    User *_user;
    unsigned int _uuid;
    std::string _versionId;
    std::string _deviceToken;
    bool _isTransacting;
    std::vector<IapObject *> _iapObjects;
};

#endif /* GameData_h */
