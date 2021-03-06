//
//  FrameLoader.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FrameLoader.h"
#include "cocos2d.h"
#include "DataManager.h"

using namespace std;
USING_NS_CC;

static const string PlistExtension(".plist");
static const string TextureExtension(".pvr.ccz");

static FrameLoader* s_pInstance(nullptr);
FrameLoader* FrameLoader::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) FrameLoader();
    }
    
    return s_pInstance;
}

void FrameLoader::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

FrameLoader::FrameLoader()
:_isLoading(false)
,_callback(nullptr) {}

FrameLoader::~FrameLoader() {}

#pragma mark - synchronous
void FrameLoader::addAllFrames()
{
    add(DataManager::getInstance()->getPVRFiles());
}

void FrameLoader::add(const vector<string>& files)
{
    for (const auto& file : files) {
        add(file);
    }
}

void FrameLoader::add(const string& file)
{
    universalAdd(file, nullptr);
}

#pragma mark - asynchronous
void FrameLoader::addAllFramesAsync(const function<void()>& callback)
{
    addAsync(DataManager::getInstance()->getPVRFiles(), callback);
}

void FrameLoader::addAsync(const vector<string>& files, const function<void()>& callback)
{
    if (!_isLoading && !files.empty()) {
        _isLoading = true;
        _files = files;
        if (callback) {
            _callback = callback;
        } else {
            _callback = nullptr;
        }
        
        recursiveAddAsync();
    }
}

void FrameLoader::addAsync(const string &file, const function<void(string)>& callback)
{
    function<void(string)> cb(nullptr);
    if (callback) {
        cb = callback;
    } else {
        cb = [](string) {};
    }
    
    universalAdd(file, cb);
}

void FrameLoader::removeCachedFrames()
{
    for (auto iter = begin(_resources); iter != end(_resources); ++iter) {
        SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(*iter);
    }
    
    _resources.clear();
}

void FrameLoader::universalAdd(const string& file, const function<void(string)>& callback)
{
    if (file.empty() || _resources.find(file) != end(_resources)) {
        if (callback) {
            callback(file);
        }
    } else {
        const string& plist(file);
        auto pos = plist.rfind(PlistExtension);
        if (string::npos == pos) {
            return;
        }
        
        const string textureFile = plist.substr(0, pos) + TextureExtension;
        auto fileUtils = FileUtils::getInstance();
        if (!fileUtils->isFileExist(plist) || !fileUtils->isFileExist(textureFile)) {
            CC_ASSERT(false);
            if (callback) {
                callback(file);
            }
        } else {
            if (callback) {
                Director::getInstance()->getTextureCache()->addImageAsync(textureFile, [=](Texture2D* texture) {
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, texture);
                    _resources.insert(file);
                    if (callback) {
                        callback(file);
                    }
                });
            } else {
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, textureFile);
                _resources.insert(file);
            }
        }
    }
}

void FrameLoader::recursiveAddAsync()
{
    if (!_files.empty()) {
        auto iter = _files.begin();
        string file = *iter;
        _files.erase(iter);
        addAsync(file, [this](const string& file) {
            recursiveAddAsync();
        });
    } else {
        _isLoading = false;
        
        if (_callback) {
            _callback();
        }
    }
}
