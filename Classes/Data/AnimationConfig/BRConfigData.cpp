//
//  BRConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/4/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BRConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "CocosGlobal.h"

using namespace std;

BRConfigData::BRConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("render_key");
        {
            const char *data = xmlElement->Attribute("resource");
            if (data && strlen(data) > 0) {
                _resource = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("shadow_resource");
            if (data && strlen(data) > 0) {
                _shadowResource = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("explode_resource");
            if (data && strlen(data) > 0) {
                _explodeResource = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("explode_sound");
            if (data && strlen(data) > 0) {
                _explodeSound = data;
            }
        }

    }
}

BRConfigData::~BRConfigData()
{
    
}

const string& BRConfigData::getName() const
{
    return _name;
}

const string& BRConfigData::getResource() const
{
    return _resource;
}

const string& BRConfigData::getShadowResource() const
{
    return _shadowResource;
}

const string& BRConfigData::getExplodeResource() const
{
    return _explodeResource;
}

const string& BRConfigData::getExplodeSound() const
{
    return _explodeSound;
}
