//
//  ChatManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatManager_h
#define ChatManager_h

#include "json/document.h"
#include "CocosGlobal.h"
#include <vector>
#include <map>

class ChatData;
class ChatMark;

class ChatManager
{
public:
    ChatManager();
    virtual ~ChatManager();
    
    void parse(const rapidjson::Value& jsonDict);
    const std::vector<const ChatData*>& getChatData(ChatType type);
    
    void sendMessage(ChatType type, int contacter, const std::string& message);
    
    void recieve();
protected:
    M_DISALLOW_COPY_AND_ASSIGN(ChatManager);

private:
    void clear();
    void parseChannel(ChatType type, const rapidjson::Value& messages, const char* key);
    
private:
    std::map<ChatType, std::vector<const ChatData*>> _chatData;
    ChatMark* _mark;
};

#endif /* ChatManager_h */
