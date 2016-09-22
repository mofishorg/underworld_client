//
//  AttributeProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/9/22.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AttributeProperty_h
#define AttributeProperty_h

#include <string>

namespace tinyxml2 { class XMLElement; }

class AttributeProperty
{
public:
    explicit AttributeProperty(tinyxml2::XMLElement* xmlElement);
    virtual ~AttributeProperty();
    
    enum class Type { ABSOLUTE, RELATIVE, ENUM };
    
    int getId() const;
    Type getType() const;
    int getPriority() const;
    const std::string& getEnmuPrefix() const;
    const std::string& getName() const;
    const std::string& getIcon() const;
    
private:
    int _id;
    Type _type;
    int _priority;
    std::string _enmuPrefix;
    std::string _name;
    std::string _icon;
};

#endif /* AttributeProperty_h */