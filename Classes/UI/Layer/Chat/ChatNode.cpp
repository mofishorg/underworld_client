//
//  ChatNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatNode.h"

using namespace std;

ChatNode* ChatNode::create()
{
    ChatNode *ret = new (nothrow) ChatNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatNode::ChatNode()
:_observer(nullptr)
{
    
}

ChatNode::~ChatNode()
{
    removeAllChildren();
}

void ChatNode::registerObserver(ChatNodeObserver *observer)
{
    _observer = observer;
}

bool ChatNode::init()
{
    if (Node::init()) {
        
        return true;
    }
    
    return false;
}
