#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "CocosGlobal.h"
#include "DataManager.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "UserDefaultsDataManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

USING_NS_CC;

//static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);

static cocos2d::Size designResolutionSize = cocos2d::Size(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Underworld_Client", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("Underworld_Client");
#endif
        director->setOpenGLView(glview);
    }
    
#if COCOS2D_DEBUG
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
#endif

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
//    Size frameSize = glview->getFrameSize();
//    // if the frame's height is larger than the height of medium size.
//    if (frameSize.height > mediumResolutionSize.height)
//    {        
//        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
//    }
//    // if the frame's height is larger than the height of small size.
//    else if (frameSize.height > smallResolutionSize.height)
//    {        
//        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
//    }
//    // if the frame's height is smaller than the height of medium size.
//    else
//    {        
//        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
//    }

    register_all_packages();
    
    // set search paths
    std::vector<std::string> searchPaths = FileUtils::getInstance()->getSearchPaths();
    
    std::vector<std::string>::iterator iter;
    
    iter = searchPaths.begin();
    searchPaths.insert(iter, DEFAULT_RESOURCE_FOLDER);
    
    iter = searchPaths.begin();
    searchPaths.insert(iter, ONLINE_UPDATE_SEARCH_PATH);
    
    FileUtils::getInstance()->setSearchPaths(searchPaths);
    
    // set default music and sound, must set here instead of "Utils" (android may crash)
    SoundManager* sm = SoundManager::getInstance();
    UserDefaultsDataManager* um = UserDefaultsDataManager::getInstance();
    sm->setMusicOn(false /*um->getMusicOn()*/);
    sm->setSoundOn(false /*um->getSoundOn()*/);
    
    // game initialization
    DataManager::getInstance()->init();
    LocalHelper::init();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::init();
#endif

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SoundManager* sm = SoundManager::getInstance();
    if (sm->isMusicOn())
    {
        sm->pauseBackgroundMusic();
    }
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SoundManager* sm = SoundManager::getInstance();
    if (sm->isMusicOn())
    {
        sm->resumeBackgroundMusic();
    }
}
