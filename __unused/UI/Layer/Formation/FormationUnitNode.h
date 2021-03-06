//
//  FormationUnitNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationUnitNode_h
#define FormationUnitNode_h

#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class FormationUnitNode : public Widget
{
public:
    static FormationUnitNode* create(int cardId, const std::string& renderKey, const Size& size);
    virtual ~FormationUnitNode();
    
    int getCardId() const;
    
protected:
    FormationUnitNode();
    bool init(int cardId, const std::string& renderKey, const Size& size);
    
protected:
    int _cardId;
};

#endif /* FormationUnitNode_h */
