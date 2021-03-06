//
//  SeniorCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SeniorCard.h"
#include "ResourceButton.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "ResourceManager.h"
#include "AbstractData.h"
#include "AbstractProperty.h"
#include "AbstractUpgradeProperty.h"

using namespace std;

SeniorCard* SeniorCard::create()
{
    auto ret = new (nothrow) SeniorCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SeniorCard::SeniorCard()
:_name(nullptr)
,_button(nullptr) {}

SeniorCard::~SeniorCard()
{
    removeAllChildren();
}

bool SeniorCard::init()
{
    if (JuniorCard::init()) {
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setTextColor(Color4B::BLACK);
        addChild(label);
        _name = label;
        
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 0, Color4B::BLACK);
        addChild(button);
        _button = button;
        
        CocosUtils::fixWidgetTouchEvent(button, _touchInvalid, [this](Ref* pSender) {
            auto button(dynamic_cast<ResourceButton*>(pSender));
            if (button) {
                if (button->isResourceEnough()) {
                    if (_observer) {
                        _observer->onBaseCardClickedResourceButton(this);
                    }
                } else {
                    MessageBox("资源不足", nullptr);
                }
            }
        });
        
        const Size originalSize(getContentSize());
        const auto& nsize(_name->getContentSize());
        const auto& bsize(_button->getContentSize());
        static const float nameSpaceY(15);
        static const float buttonSpaceY(8);
        const float height(originalSize.height + nsize.height + bsize.height + nameSpaceY + buttonSpaceY);
        const float width = MAX(MAX(originalSize.width, nsize.width), bsize.width);
        setContentSize(Size(width, height));
        
        resetPositions(Point((width - originalSize.width) / 2, bsize.height + buttonSpaceY));
        _button->setPosition(Point(width / 2, bsize.height / 2));
        _name->setPosition(width / 2, height - nsize.height / 2);
        _infoButton->setVisible(true);
        
        return true;
    }
    
    return false;
}

void SeniorCard::updateProperty(const AbstractProperty* property)
{
    JuniorCard::updateProperty(property);
    if (_name) {
        _name->setString(property ? LocalHelper::getString(property->getName()) : "");
    }
}

void SeniorCard::updateData(const AbstractData* data)
{
    JuniorCard::updateData(data);
    
    const bool show(nullptr != data);
    if (_button) {
        _button->setVisible(show);
    }
    
    do {
        CC_BREAK_IF(!show);
        auto up(data->getUpgradeProperty());
        CC_BREAK_IF(!up);
        const auto& pair(up->getResourceCost());
        const auto type(pair.first);
        CC_ASSERT(pair.first != ResourceType::MAX);
        const auto count(pair.second);
        _button->setType(type);
        _button->setCount(count);
        _button->setEnabled(canUpgrade());
        _button->setResourceEnough(ResourceManager::getInstance()->getResourceCount(type) >= count);
    } while (false);
}
