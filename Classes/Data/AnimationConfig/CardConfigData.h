//
//  CardConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/3/4.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardConfigData_h
#define CardConfigData_h

#include <iostream>

#include "EffectData.h"

namespace tinyxml2 { class XMLElement; }

class CardConfigData
{
public:
    explicit CardConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~CardConfigData();
    
    int getId() const;
    const std::string& getIcon() const;
    const std::string& getSmallIcon() const;
    const std::string& getDisabledIcon() const;
    const EffectData& getTips() const;
    
private:
    int _cardId;
    std::string _icon;
    std::string _smallIcon;
    std::string _disabledIcon;
    EffectData _tips;
};

#endif /* CardConfigData_h */
