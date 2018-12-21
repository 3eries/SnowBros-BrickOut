//
//  StageClearPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#include "StageClearPopup.hpp"

#include "Define.h"
#include "User.hpp"

#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

StageClearPopup* StageClearPopup::create(const StageData &stage) {
    
    auto popup = new StageClearPopup(stage);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

StageClearPopup::StageClearPopup(const StageData &stage) : BasePopup(PopupType::STAGE_CLEAR),
stage(stage),
onNextListener(nullptr) {
    
}

StageClearPopup::~StageClearPopup() {
}

bool StageClearPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }

    return true;
}

void StageClearPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void StageClearPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void StageClearPopup::initContentView() {
    
    BasePopup::initContentView();

    // 타이틀
    auto titleLabel = Label::createWithTTF(STR_FORMAT("STAGE %d\nCLEAR", stage.stage),
                                           FONT_COMMODORE, 60, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(0, 200));
    addContentChild(titleLabel);
    
    // 다음 스테이지 이동 버튼
    auto nextBtn = SBButton::create(DIR_IMG_MAIN + "RSP_btn_start.png");
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setPosition(Vec2MC(0, -300));
    addContentChild(nextBtn);
    
    nextBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onNextListener);
    });
}

/**
 * 등장 연출
 */
void StageClearPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, FADE_DURATION);
}

/**
 * 퇴장 연출
 */
void StageClearPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, FADE_DURATION);
}

/**
 * 등장 연출 완료
 */
void StageClearPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
