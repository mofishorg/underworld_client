//
//  HeroLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroLocalData_h
#define HeroLocalData_h

#include "AbstractLocalData.h"
#include <vector>
#include <map>

class AttributeData;
class SkillLocalData;
class ArtifactLocalData;

class HeroLocalData : public AbstractLocalData
{
public:
    HeroLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroLocalData();
    
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const SkillLocalData* getSkillData() const;
    const std::vector<int>& getArtifacts() const;
    const ArtifactLocalData* getUnlockedArtifactData(int index) const;
    
private:
    std::map<int, AttributeData *> _attributes;
    int _skillId;
    std::vector<int> _artifactIds;
};

#endif /* HeroLocalData_h */