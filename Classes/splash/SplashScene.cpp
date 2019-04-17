//
//  SplashScene.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "SplashScene.hpp"

#include <spine/spine-cocos2dx.h>

#include "Define.h"
#include "User.hpp"
#include "UserDefaultKey.h"
#include "PatchManager.hpp"
#include "ResourceHelper.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"
#include "FunnyHelper.hpp"
#include "ContentManager.hpp"

#include "LogoView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace spine;
using namespace std;

static const float  CHECK_REPLACE_MAIN_SCENE_INTERVAL   = 0.05f;
static const string CHECK_REPLACE_MAIN_SCENE_SCHEDULER  = "CHECK_REPLACE_MAIN_SCENE_SCHEDULER";

static const float  LAUNCH_IMAGE_MIN_DURATION           = 4.0f;
static const string LAUNCH_IMAGE_SCHEDULER              = "LAUNCH_IMAGE_SCHEDULER";

SplashScene::SplashScene() :
isReplacedMainScene(false),
isLaunchImageFinished(false),
isLoginCompleted(false),
logoMode(true) {
    
}

SplashScene::~SplashScene() {
    
}

bool SplashScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    ResourceHelper::preload();
    
    initLaunchImage();
    login();
    
    // 메인 화면 전환 체크 스케줄러 실행
    schedule([=](float dt) {
        if( isLaunchImageFinished && isLoginCompleted ) {
            this->replaceMainScene();
        }
    }, CHECK_REPLACE_MAIN_SCENE_INTERVAL, CHECK_REPLACE_MAIN_SCENE_SCHEDULER);
    
    // Firebase 스크린 설정, 모듈이 초기화 되기 전에 실행될 수 있기 때문에 스케줄러 구현
    scheduleOnce([=](float dt) {
        firebase::Analytics::setScreenName(FA_SCREEN_SPLASH);
    }, 0.1f, "SPLASH_SCREEN_NAME");
    
    return true;
}

void SplashScene::onEnter() {
    
    BaseScene::onEnter();
}

void SplashScene::onExit() {
    
    BaseScene::onExit();
}

/**
 * 메인화면으로 전환
 */
void SplashScene::replaceMainScene() {
    
    if( isReplacedMainScene ) {
        return;
    }
    
    isReplacedMainScene = true;
    replaceScene(SceneType::MAIN);
}

/**
 * UI 초기화
 */
void SplashScene::initLaunchImage() {

    SBDirector::postDelayed(this, [=]() {
        this->launchImageFinished();
    }, 1.0f, true);
    
    return;
    
    // 시리즈 로고는 30분동안 재노출 하지 않음
    double now = SBSystemUtils::getCurrentTimeSeconds();
    double viewTime = UserDefault::getInstance()->getDoubleForKey(UserDefaultKey::SPLASH_LOGO_VIEW_TIME, 0);
    double dt = now - viewTime;
    
    CCLOG("SplashScene::initLaunchImage logo view dt: %f", dt);
    
    // 30분 경과, 로고 출력
    if( dt >= (60*30) ) {
        UserDefault::getInstance()->setDoubleForKey(UserDefaultKey::SPLASH_LOGO_VIEW_TIME, now);
        UserDefault::getInstance()->flush();
        
        auto logoView = LogoView::create();
        addChild(logoView);
        
        logoView->setOnFinishedListener([=]() {
            this->launchImageFinished();
        });
        logoView->run();
    }
    // 명언
    else {
        logoMode = false;
        
        auto layer = SBNodeUtils::createZeroSizeNode();
        addChild(layer);
        
        // bg
        // layer->addChild(LayerColor::create(Color4B::BLACK));
        
        // famous saying
        auto famousSaying = FunnyHelper::getRandomFamousSaying();
        
        auto saying = Label::createWithTTF(famousSaying.saying, FONT_FAMOUS_SAYING, 140,
                                           Size(SB_WIN_SIZE.width*0.9f, 0),
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
        saying->setColor(Color3B::WHITE);
        saying->setAnchorPoint(ANCHOR_MB);
        saying->setPosition(Vec2MC(0, 40));
        layer->addChild(saying);
        
        auto person = Label::createWithTTF("-" + famousSaying.person + "-", FONT_FAMOUS_SAYING, 110);
        person->setColor(Color3B::WHITE);
        person->setAnchorPoint(ANCHOR_MT);
        person->setPosition(Vec2MC(0, -20));
        layer->addChild(person);
        
        // 명언 텍스트 연출
        {
            saying->setOpacity(0);
            saying->runAction(FadeIn::create(1.0f));
        }
        
        // 인물 텍스트 연출
        {
            person->setOpacity(0);
            
            auto delay = DelayTime::create(1.5f);
            auto fadeIn = FadeIn::create(1.0f);
            person->runAction(Sequence::create(delay, fadeIn, nullptr));
        }
        
        // 명언 fade out
        {
            SBNodeUtils::recursiveCascadeOpacityEnabled(layer, true);
            
            auto delay = DelayTime::create(3.5f);
            auto fadeOut = FadeOut::create(0.5f);
            layer->runAction(Sequence::create(delay, fadeOut, nullptr));
        }
        
        // 완료 스케줄러
        scheduleOnce([=](float dt) {
            this->launchImageFinished();
        }, LAUNCH_IMAGE_MIN_DURATION, LAUNCH_IMAGE_SCHEDULER);
    }
}

/**
 * 런치 이미지 종료
 */
void SplashScene::launchImageFinished() {
    
    CCLOG("SplashScene::launchImageFinished");
    
    if( !isLaunchImageFinished ) {
        isLaunchImageFinished = true;
    }
}

/**
 * 로그인
 */
void SplashScene::login() {
    
    auto patchMgr = PatchManager::getInstance();
    patchMgr->setOnAppVersion([=](AppVersion version) {
    });
    
    patchMgr->setOnGameConfig([=](string version, string json) {
    });
    
    patchMgr->patch();
    
    loginCompleted();
}

/**
 * 로그인 완료
 */
void SplashScene::loginCompleted() {
    
    if( isLoginCompleted ) {
        return;
    }
    
    isLoginCompleted = true;
    
    ContentManager::getInstance()->init();
    User::getInstance()->init();
    ContentResourceHelper::preload();
}


