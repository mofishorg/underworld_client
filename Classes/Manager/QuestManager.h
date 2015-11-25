//
//  QuestManager.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestManager_h
#define QuestManager_h

#include "cocos2d.h"
#include "json/document.h"
#include "CocosGlobal.h"

class QuestData;

class QuestManager
{
public:
    QuestManager();
    virtual ~QuestManager();
    
    void initQuest(QuestType type, const rapidjson::Document& jsonDict);
    void updateQuestProgress(QuestType type, int questId, int progress);
    
    // getters
    const std::vector<const QuestData*>& getQuestData(QuestType type);
    int getQuestProgress(QuestType type, int questId) const;
    
protected:
    CC_DISALLOW_COPY_AND_ASSIGN(QuestManager);
    
private:
    typedef std::pair<const QuestData*, int> QuestProgress;
    
private:
    std::map<QuestType, std::vector<const QuestData*>> _questData;
    std::map<QuestType, std::map<int, QuestProgress>> _questProgress;
};

#endif /* QuestManager_h */