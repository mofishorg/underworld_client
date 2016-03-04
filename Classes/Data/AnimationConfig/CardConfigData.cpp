//
//  CardConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/4.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

CardConfigData::CardConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("name");
        
        const char *data = xmlElement->Attribute("icon");
        if (data && strlen(data) > 0) {
            static const string suffix(".png");
            {
                static const string prefix("GameImages/icons/unit/big/normal/");
                _icon = prefix + data + suffix;
            }
            {
                static const string prefix("GameImages/icons/unit/small/");
                _smallIcon = prefix + data + "_1" + suffix;
            }
            {
                static const string prefix("GameImages/icons/unit/big/disabled/");
                _disabledIcon = prefix + data + "_2" + suffix;
            }
        }
    }
}

CardConfigData::~CardConfigData()
{
    
}

const string& CardConfigData::getName() const
{
    return _name;
}

const string& CardConfigData::getIcon() const
{
    return _icon;
}

const string& CardConfigData::getSmallIcon() const
{
    return _smallIcon;
}

const string& CardConfigData::getDisabledIcon() const
{
    return _disabledIcon;
}