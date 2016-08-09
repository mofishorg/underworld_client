//
//  SettingsLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SettingsLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SettingUI.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "UniversalButton.h"

using namespace std;

enum class SettingType {
    Music,
    Sound,
    Language,
    APNS,
    Rename,
    Logout,
    Help,
    PrivacyPolicy,
    ServiceTerms,
    Guide,
};

#pragma mark - static
static const Size subNodeSize(579, 513);

static const vector<SettingType> upperTypes = {
    SettingType::Music,
    SettingType::Sound,
    SettingType::Language,
    SettingType::APNS,
    SettingType::Rename,
    SettingType::Logout,
};

static const vector<SettingType> lowerTypes = {
    SettingType::Help,
    SettingType::PrivacyPolicy,
    SettingType::ServiceTerms,
    SettingType::Guide,
};

#pragma mark - SettingNode
class SettingNode : public Node {
public:
    typedef function<void(SettingNode*, SettingType)> Callback;
    static SettingNode* create(SettingType type, const Callback& callback, bool isOn);
    virtual ~SettingNode();
    void toggle(bool isOn);
    void setEnabled(bool enabled);
    void setTitle(const string& title);
    
private:
    SettingNode();
    bool init(SettingType type, const Callback& callback, bool isOn);
    
private:
    SettingType _type;
    Callback _callback;
    bool _isSwitch;
    bool _isOn;
    UniversalButton* _button;
};

SettingNode* SettingNode::create(SettingType type, const Callback& callback, bool isOn) {
    auto ret = new (nothrow) SettingNode();
    if (ret && ret->init(type, callback, isOn)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SettingNode::SettingNode()
:_callback(nullptr)
,_isSwitch(false)
,_isOn(false)
,_button(nullptr) {}

SettingNode::~SettingNode()
{
    removeAllChildren();
}

bool SettingNode::init(SettingType type, const Callback& callback, bool isOn) {
    if (Node::init()) {
        // value:
        // first(bool): if the button is a on/off switch
        // second: if "first" is true, it's the title; else it's the button's title
        static const map<SettingType, std::pair<bool, std::string>> SettingNodeInfo {
            {SettingType::Music, {true, LocalHelper::getString("ui_setting_music")}},
            {SettingType::Sound, {true, LocalHelper::getString("ui_setting_sound")}},
            {SettingType::Language, {true, LocalHelper::getString("ui_setting_language")}},
            {SettingType::APNS, {true, LocalHelper::getString("ui_setting_apns")}},
            {SettingType::Rename, {false, LocalHelper::getString("ui_setting_rename")}},
            {SettingType::Logout, {false, LocalHelper::getString("ui_setting_logout")}},
            {SettingType::Help, {false, LocalHelper::getString("ui_setting_help")}},
            {SettingType::PrivacyPolicy, {false, LocalHelper::getString("ui_setting_privacyPolicy")}},
            {SettingType::ServiceTerms, {false, LocalHelper::getString("ui_setting_serviceTerms")}},
            {SettingType::Guide, {false, LocalHelper::getString("ui_setting_guide")}},
        };
        
        _type = type;
        _callback = callback;
        _isOn = isOn;
        
        if (SettingNodeInfo.find(type) != end(SettingNodeInfo)) {
            const auto& pair = SettingNodeInfo.at(type);
            _isSwitch = pair.first;
            string title(pair.second);
            static const string blankTitle(" ");
            if (CocosUtils::trim(title).empty()) {
                title = blankTitle;
            }
            
            // button
            string bTitle;
            if (_isSwitch) {
                bTitle.assign(isOn ? LocalHelper::getString("ui_setting_on") : LocalHelper::getString("ui_setting_off"));
            } else {
                bTitle.assign(title);
            }
            
            auto bType(isOn ? UniversalButton::BType::Blue : UniversalButton::BType::Red);
            _button = UniversalButton::create(UniversalButton::BSize::Small, bType, bTitle);
            _button->setPressedActionEnabled(true);
            _button->setCallback([this](Ref*) {
                if (_callback) {
                    _callback(this, _type);
                }
            });
            _button->setTitleFontSize(SMALL_FONT_SIZE);
            addChild(_button);
            
            // check if the button is lower
            bool isLower(false);
            for (const auto& t : lowerTypes) {
                if (type == t) {
                    isLower = true;
                    break;
                }
            }
            
            setAnchorPoint(Point::ANCHOR_MIDDLE);
            const auto& bsize(_button->getContentSize());
            if (isLower) {
                setContentSize(bsize);
                _button->setPosition(Point(bsize.width / 2, bsize.height / 2));
            } else {
                auto label = CocosUtils::createLabel(_isSwitch ? title : blankTitle, DEFAULT_FONT_SIZE);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE);
                label->setTextColor(Color4B::BLACK);
                addChild(label);
                
                const auto& lsize(label->getContentSize());
                static const float space(3);
                const Size size(bsize.width, lsize.height + bsize.height + space);
                setContentSize(size);
                _button->setPosition(Point(size.width / 2, bsize.height / 2));
                label->setPosition(Point(size.width / 2, size.height - lsize.height / 2));
            }
        }
        
        return true;
    }
    
    return false;
}

void SettingNode::toggle(bool isOn) {
    if (_isSwitch && _isOn != isOn) {
        _isOn = isOn;
        
        if (_button) {
            auto bType(isOn ? UniversalButton::BType::Blue : UniversalButton::BType::Red);
            auto bTitle(isOn ? LocalHelper::getString("ui_setting_on") : LocalHelper::getString("ui_setting_off"));
            _button->setType(bType);
            setTitle(bTitle);
        }
    }
}

void SettingNode::setEnabled(bool enabled) {
    if (_button) {
        _button->setEnabled(enabled);
    }
}

void SettingNode::setTitle(const string& title) {
    if (_button) {
        _button->setTitle(title);
    }
}

#pragma mark - SettingsLayer
SettingsLayer* SettingsLayer::create()
{
    SettingsLayer *ret = new (nothrow) SettingsLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SettingsLayer::SettingsLayer()
:_observer(nullptr)
,_returnButton(nullptr)
,_subNode(nullptr)
,_contentNode(nullptr)
,_languageLayer(nullptr)
,_languageNode(nullptr) {}

SettingsLayer::~SettingsLayer()
{
    removeAllChildren();
}

void SettingsLayer::registerObserver(SettingsLayerObserver *observer)
{
    _observer = observer;
}

bool SettingsLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        
        auto bg = Sprite::create(SettingUI::getResourcePath("ui_background_2.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        auto subNode = CocosUtils::createBackground(CocosUtils::getResourcePath("ui_background_1.png"), subNodeSize);
        bg->addChild(subNode);
        _subNode = subNode;
        
        const auto& size(bg->getContentSize());
        const auto& subBgSize(subNode->getContentSize());
        const float edge((size.width - subBgSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subBgSize.height / 2 + edge));
        
        CocosUtils::createRedExitButton(bg, [this]() {
            removeFromParent();
        });
        
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_setting_title"), BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subBgSize.height + edge) / 2));
        bg->addChild(title);
        
        _returnButton = createReturnButton(bg, Vec2(8.0f, 10.0f), [this]() {
            if (_languageLayer) {
                _languageLayer->setVisible(false);
            }
            if (_returnButton) {
                _returnButton->setVisible(false);
            }
            if (_contentNode) {
                _contentNode->setVisible(true);
            }
            
            setCurrentLanguage();
        });
        _returnButton->setVisible(false);
        
        createContent();
        setCurrentLanguage();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(SettingsLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(SettingsLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool SettingsLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void SettingsLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - LanguageLayerObserver

#pragma mark - RenameLayerObserver

Node* SettingsLayer::createReturnButton(Node* parent, const Vec2& offset, const function<void()>& callback) const
{
    if (parent) {
        auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Blue, "");
        parent->addChild(button);
        
        const auto& size(button->getContentSize());
        auto icon = Sprite::create(SettingUI::getResourcePath("icon_jiantou.png"));
        icon->setPosition(Point(size.width / 2, size.height / 2));
        button->addChild(icon);
        
        const auto& pSize(parent->getContentSize());
        button->setPosition(Point(offset.x + size.width / 2, pSize.height - (offset.y + size.height / 2)));
        button->setCallback([callback](Ref*) {
            if (callback) { callback(); }
        });
        
        return button;
    }
    
    return nullptr;
}

void SettingsLayer::createContent()
{
    _contentNode = Node::create();
    _contentNode->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _contentNode->setContentSize(subNodeSize);
    _contentNode->setPosition(Point(subNodeSize.width / 2, subNodeSize.height / 2));
    _subNode->addChild(_contentNode);
    
    auto parent(_contentNode);
    if (parent) {
        static const Vec2 edge(12, 28);
        static const float edgeTop(edge.x);
        const auto& size(parent->getContentSize());
        
        auto line = Sprite::create(SettingUI::getResourcePath("ui_line.png"));
        line->setScaleX(size.width - edge.x * 2);
        line->setPosition(Point(size.width / 2, size.height / 2));
        parent->addChild(line);
        
        static const unsigned int column(3);
        static const unsigned int row(2);
        
        // top
        {
            Vec2 offset(Vec2::ZERO);
            const auto& nsize(SettingNode::create(upperTypes.front(), nullptr, true)->getContentSize());
            if (column > 1) {
                offset.x = (size.width - nsize.width * column - edge.x * 2) / (column - 1);
            }
            
            if (row > 1) {
                offset.y = (size.height - line->getPositionY() - (nsize.height * row + edgeTop + edge.y)) / (row - 1);
            }
            
            createSettingNodes(parent, upperTypes, Point(0, line->getPositionY()), row, column, edge, offset);
        }
            
        // bottom
        {
            auto notice = Sprite::create(SettingUI::getResourcePath("ui_tiao_6.png"));
            parent->addChild(notice);
            
            const auto& noticeSize(notice->getContentSize());
            notice->setPosition(Point(size.width / 2, line->getPositionY() - (edgeTop + noticeSize.height / 2)));
            
            static const float offsetX(8);
            auto message = CocosUtils::createLabel(LocalHelper::getString("ui_setting_hint"), SMALL_FONT_SIZE, DEFAULT_FONT, noticeSize - Size(offsetX * 2, 0), TextHAlignment::LEFT, TextVAlignment::CENTER);
            message->setTextColor(Color4B::BLACK);
            message->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            message->setPosition(Point(offsetX, notice->getContentSize().height / 2));
            notice->addChild(message);
            
            Vec2 offset(Vec2::ZERO);
            const auto& nsize(SettingNode::create(lowerTypes.front(), nullptr, true)->getContentSize());
            if (column > 1) {
                offset.x = (size.width - nsize.width * column - edge.x * 2) / (column - 1);
            }
            
            if (row > 1) {
                offset.y = (line->getPositionY() - (nsize.height * row + edgeTop * 2 + edge.y + noticeSize.height)) / (row - 1);
            }
            
            createSettingNodes(parent, lowerTypes, Point::ZERO, row, column, edge, offset);
        }
    }
}

void SettingsLayer::createSettingNodes(Node* parent, const vector<SettingType>& types, const Point& basePoint, int row, int column, const Vec2& edge, const Vec2& space)
{
    for (int i = 0; i < row; ++ i) {
        for (int j = 0; j < column; ++j) {
            const int idx(i * column + j);
            if (idx < types.size()) {
                const auto& type(types.at(idx));
                bool isOn(true);
                bool isEnabled(true);
                checkButtonStatus(type, isOn, isEnabled);
                auto node = SettingNode::create(type, CC_CALLBACK_2(SettingsLayer::onButtonClicked, this), isOn);
                const auto& size(node->getContentSize());
                node->setEnabled(isEnabled);
                const Point point(Point(edge.x + (j + 0.5) * size.width + j * space.x, edge.y + (row - i - 0.5) * size.height + (row - i - 1) * space.y));
                node->setPosition(basePoint + point);
                parent->addChild(node);
                
                if (SettingType::Language == type) {
                    _languageNode = node;
                }
            }
        }
    }
}

void SettingsLayer::checkButtonStatus(SettingType type, bool& isOn, bool& isEnabled) const
{
    auto sm(SoundManager::getInstance());
    if (SettingType::Music == type) {
        isOn = sm->isMusicOn();
    } else if (SettingType::Sound == type) {
        isOn = sm->isSoundOn();
    } else if (SettingType::APNS == type) {
        
    } else if (SettingType::Rename == type) {
        if (true) {
            isEnabled = true;
        } else {
            isEnabled = false;
        }
    }
}

void SettingsLayer::onButtonClicked(SettingNode* node, SettingType type)
{
    auto sm(SoundManager::getInstance());
    sm->playButtonSound();
    
    if (SettingType::Rename == type) {
        auto layer = RenameLayer::create();
        layer->registerObserver(this);
        addChild(layer);
    } else if (SettingType::Language == type) {
        if (_languageLayer) {
            _languageLayer->setVisible(true);
        } else {
            auto layer = LanguageLayer::create(subNodeSize);
            layer->registerObserver(this);
            _subNode->addChild(layer);
            _languageLayer = layer;
        }
        
        if (_returnButton) {
            _returnButton->setVisible(true);
        }
        
        if (_contentNode) {
            _contentNode->setVisible(false);
        }
    } else if (SettingType::Music == type) {
        sm->setMusicOn(!sm->isMusicOn());
        node->toggle(sm->isMusicOn());
    } else if (SettingType::Sound == type) {
        sm->setSoundOn(!sm->isSoundOn());
        node->toggle(sm->isSoundOn());
    }
}

void SettingsLayer::setCurrentLanguage()
{
    if (_languageNode) {
        _languageNode->setTitle(LocalHelper::getCurrentLanguageName());
    }
}
