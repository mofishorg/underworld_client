//
//  User.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef User_h
#define User_h

#include "json/document.h"
#include "CocosGlobal.h"

// from login response
#define kUID    ("uid")
#define kAuth   ("auth")
#define kNew    ("new_user")

class User
{
public:
    User(const rapidjson::Document& jsonDict);
    virtual ~User();
    
    // getters
    unsigned int getUserId() const;
    bool isNewUser() const;
    bool isGuest() const;
    const std::string& getToken() const;
    const std::string& getName() const;
    
    //------------------------------ NETWORKING BEGIN ------------------------------//
    void parseGemInfo(const rapidjson::Value& root);
    void parseResources(const rapidjson::Value& root, const char* key, bool silent = false);
    
    // user info
    void loadUserInfo(const std::string& deviceToken, const httpRequestCallback& success, const httpErrorCallback& onError);
    //------------------------------ NETWORKING END ------------------------------//
    
private:
    M_DISALLOW_COPY_AND_ASSIGN(User);
    void init();
    
    //------------------------------ NETWORKING BEGIN ------------------------------//
    void parseUserInfo(const rapidjson::Document& jsonDict);
    //------------------------------ NETWORKING END ------------------------------//
    
    //------------------------------ DEBUG BEGIN ------------------------------//
    void parseDebugUserInfo(const rapidjson::Document& jsonDict);
    //------------------------------ DEBUG END ------------------------------//
    
private:
    bool _isNewUser;
    bool _isGuest;
    unsigned int _uid;
    std::string _token;
    std::string _name;
};

#endif /* User_h */
