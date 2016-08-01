//
//  UniversalButton.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UniversalButton.h"
#include "CocosUtils.h"

using namespace std;

static const string folder("GameImages/public/");

static string getButtonFile(UniversalButton::BSize size, UniversalButton::BType type) {
    const bool big(UniversalButton::BSize::Big == size);
    string file;
    switch (type) {
        case UniversalButton::BType::Blue:
        {
            if (big) {
                file = "button_lanse.png";
            } else {
                file = "button_lanse_1.png";
            }
        }
            break;
        case UniversalButton::BType::Red:
        {
            if (big) {
                file = "button_hongse_1.png";
            } else {
                file = "button_hongse_1.png";
            }
        }
            break;
        case UniversalButton::BType::Green:
        {
            if (big) {
                file = "button_lvse_1.png";
            } else {
                file = "button_lvse_1.png";
            }
        }
            break;
            
        default:
            break;
    }
    
    if (!file.empty()) {
        return folder + file;
    }
    
    return "";
}

UniversalButton::UniversalButton()
:_bSize(BSize::Big)
,_bType(BType::Blue)
,_callback(nullptr)
,_button(nullptr) {}

UniversalButton::~UniversalButton()
{
    removeAllChildren();
}

UniversalButton* UniversalButton::create(BSize size, BType type, const string& title)
{
    auto p = new (nothrow) UniversalButton();
    if(p && p->init(size, type, title)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool UniversalButton::init(BSize size, BType type, const string& title)
{
    if (Node::init()) {
        _bSize = size;
        _bType = type;
        
        const auto& file = getButtonFile(size, type);
        if (!file.empty()) {
            const string disabled = folder + ((UniversalButton::BType::Blue == type) ? "button_huise.png" : "button_huise.png");
            auto button = ui::Button::create(file, file, disabled);
            addChild(button);
            
            button->setTitleFontName(DEFAULT_FONT);
            button->setTitleFontSize(DEFAULT_FONT_SIZE);
            button->setTitleText(title);
            button->setTitleColor(Color3B::BLACK);
            
            _button = button;
            
            setAnchorPoint(Point::ANCHOR_MIDDLE);
            adjust();
        }
        
        return true;
    }
    
    return false;
}

void UniversalButton::setType(BType type)
{
    if (_bType != type) {
        _bType = type;
        
        if (_button) {
            const auto& file = getButtonFile(_bSize, type);
            _button->loadTextures(file, file);
            adjust();
        }
    }
}

void UniversalButton::setTitle(const string& title)
{
    if (_button) {
        _button->setTitleText(title);
    }
}

void UniversalButton::setCallback(const Callback& callback)
{
    _callback = callback;
    
    if (_button) {
        _button->addClickEventListener(callback);
    }
}

void UniversalButton::setPressedActionEnabled(bool enabled)
{
    if (_button) {
        _button->setPressedActionEnabled(enabled);
    }
}

void UniversalButton::setEnabled(bool enabled)
{
    if (_button) {
        _button->setEnabled(enabled);
    }
}

UniversalButton::BType UniversalButton::getType() const
{
    return _bType;
}

Label* UniversalButton::getLabel() const
{
    if (_button) {
        return _button->getTitleRenderer();
    }
    
    return nullptr;
}

void UniversalButton::adjust()
{
    if (_button) {
        const auto& size(_button->getContentSize());
        setContentSize(size);
        const Point mid(size.width / 2, size.height / 2);
        _button->setPosition(mid);
    }
}