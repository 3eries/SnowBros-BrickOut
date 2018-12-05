//
//  ResultPopup.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResultPopup.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UIHelper.hpp"
#include "PopupManager.hpp"
#include "RankingManager.hpp"

#include "ExitAlertPopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float ResultPopup::SLIDE_IN_DURATION               = EffectDuration::POPUP_SLIDE_NORMAL;
const float ResultPopup::SLIDE_OUT_DURATION              = EffectDuration::POPUP_SLIDE_NORMAL;

#define                  SLIDE_IN_POSITION               Vec2(0, 0)
#define                  SLIDE_OUT_POSITION              Vec2TL(0, -stone->getBoundingBox().getMinY())

ResultPopup* ResultPopup::create(int score) {
    
    auto popup = new ResultPopup(score);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

ResultPopup::ResultPopup(int score) : BasePopup(PopupType::GAME_RESULT),
firstEnterAction(true),
score(score) {
    
}

ResultPopup::~ResultPopup() {
    
}

bool ResultPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ResultPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

bool ResultPopup::onBackKeyReleased() {
    
    if( PopupManager::getPopupCount() > 1 || !PopupManager::exists(PopupType::GAME_RESULT) ) {
        return false;
    }
    
    // 광고가 열렸다 닫힌 경우 예외처리
    if( !Director::getInstance()->isValid() ) {
        return true;
    }
    
    // 등/퇴장 연출중
    if( runningEnterAction || runningExitAction ) {
        return true;
    }
    
    // 앱 종료 알림 팝업 생성
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    auto popup = ExitAlertPopup::create();
    popup->setOnExitAppListener([=]() {
        SBSystemUtils::exitApp();
    });
    
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
    
    return true;
}

void ResultPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
}

void ResultPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // addContentChild(SBNodeUtils::createBackgroundNode(getContentView(), Color4B(255,0,0,255*0.3f)), -1);
    
    // RSP_popup_bg_big.png Vec2MC(0, 46) , Size(696, 956)
    stone = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_big.png");
    stone->setAnchorPoint(ANCHOR_M);
    stone->setPosition(Vec2MC(0, 46));
    addContentChild(stone);
    
    // images
    {
        // RSP_popup_title_rip.png Vec2MC(-4, 310) , Size(296, 108)
        // RSP_popup_text_score.png Vec2MC(0, 158) , Size(272, 60)
        // RSP_popup_text_top3.png Vec2MC(0, -72) , Size(216, 60)
        SBUIInfo infos[] = {
            SBUIInfo(ANCHOR_M,   Vec2MC(-4, 310),  "RSP_popup_title_rip.png"),      // rip
            SBUIInfo(ANCHOR_M,   Vec2MC(0, 158),   "RSP_popup_text_score.png"),     // score title
            SBUIInfo(ANCHOR_M,   Vec2MC(0, -72),   "RSP_popup_text_top3.png"),      // top3 title
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto spr = Sprite::create(DIR_IMG_GAME + info.file);
            info.apply(spr);
            addContentChild(spr);
            
            /**
            if( i > 0 ) {
                spr->setScale(0.7f);
            }
             */
        }
    }
    
    // score
    {
        // RSP_popup_bg_score.png Vec2MC(-2, 66) , Size(420, 100)
        auto scoreBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_score.png");
        scoreBg->setAnchorPoint(ANCHOR_M);
        scoreBg->setPosition(Vec2MC(-2, 66));
        addContentChild(scoreBg);
        
        scoreLabel = Label::createWithTTF(TO_STRING(score), FONT_RETRO, 85);
        scoreLabel->setAnchorPoint(ANCHOR_M);
        scoreLabel->setPosition(Vec2MC(scoreBg->getContentSize(), 0, 5));
        scoreLabel->setColor(Color3B(255,255,0));
        scoreBg->addChild(scoreLabel);
    }
}

/**
 * 등장 연출
 */
void ResultPopup::runEnterAction(float duration, SBCallback onFinished) {
    
    duration *= enterTimeScale;
    BasePopup::runEnterAction(duration, onFinished);
    
    CCLOG("ResultPopup::runEnterAction");
    
    // 액션 완료
    auto onActionFinished = [=]() {
        CCLOG("ResultPopup::runEnterAction onActionFinished");
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    runSlideAction(onActionFinished, duration, SLIDE_OUT_POSITION, SLIDE_IN_POSITION);
}

void ResultPopup::runEnterAction(SBCallback onFinished) {
    
    runEnterAction(SLIDE_IN_DURATION, onFinished);
}

/**
 * 퇴장 연출
 */
void ResultPopup::runExitAction(float duration, SBCallback onFinished) {
    
    duration *= exitTimeScale;
    BasePopup::runExitAction(duration, onFinished);
    
    CCLOG("ResultPopup::runExitAction");
    
    // slide out
    runSlideAction([=]() {
        CCLOG("ResultPopup::runExitAction onActionFinished");
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, duration, SLIDE_IN_POSITION, SLIDE_OUT_POSITION);
}

void ResultPopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(SLIDE_OUT_DURATION, onFinished);
}

/**
 * 등장 연출 완료
 */
void ResultPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    if( firstEnterAction ) {
        firstEnterAction = false;
    }
}
