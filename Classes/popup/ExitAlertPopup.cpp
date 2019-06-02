//
//  ExitAlertPopup.cpp
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#include "ExitAlertPopup.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

ExitAlertPopup::ExitAlertPopup() : BasePopup(PopupType::EXIT_APP),
onExitAppListener(nullptr) {
    
}

ExitAlertPopup::~ExitAlertPopup() {
}

bool ExitAlertPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ExitAlertPopup::onEnter() {
    
    BasePopup::onEnter();
}

void ExitAlertPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void ExitAlertPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // title
    // RSP_popup_title_give_up.png Vec2MC(0, 210) , Size(392, 60)
    auto title = Label::createWithTTF("QUIT THE GAME?", FONT_COMMODORE, 50, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 10));
    addChild(title);
    
    // YES
    auto yesBtn = SBButton::create("YES", FONT_COMMODORE, 50);
    yesBtn->setAnchorPoint(ANCHOR_MR);
    yesBtn->setPosition(Vec2MC(-60, -170));
    yesBtn->setContentSize(Size(100, 80));
    addChild(yesBtn);
    
    yesBtn->setOnClickListener([=](Node*) {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onExitAppListener);
    });
    
    // NO
    auto noBtn = SBButton::create("NO", FONT_COMMODORE, 50);
    noBtn->setAnchorPoint(ANCHOR_ML);
    noBtn->setPosition(Vec2MC(60, -170));
    noBtn->setContentSize(Size(100, 80));
    addChild(noBtn);
    
    noBtn->setOnClickListener([=](Node*) {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
}

/**
 * 등장 연출
 */
void ExitAlertPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(0, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 퇴장 연출
 */
void ExitAlertPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 등장 연출 완료
 */
void ExitAlertPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}

