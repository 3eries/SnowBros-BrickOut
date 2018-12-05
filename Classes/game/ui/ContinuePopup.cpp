//
//  ContinuePopup.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ContinuePopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const int   COUNTDOWN_START     = 10;

static const float ENTER_DURATION       = 0.3f;
static const float EXIT_DURATION        = 0.2f;

ContinuePopup* ContinuePopup::create(bool isAdsLoaded) {
    
    auto popup = new ContinuePopup();
    
    if( popup && popup->init(isAdsLoaded) ) {
        popup->autorelease();
        return popup;
    }
    
    delete popup;
    return nullptr;
}

ContinuePopup::ContinuePopup() : BasePopup(PopupType::CONTINUE),
isAdsLoaded(false),
isTimeOut(false),
isContinue(false),
onContinueListener(nullptr),
onTimeOutListener(nullptr),
count(COUNTDOWN_START) {
    
}

ContinuePopup::~ContinuePopup() {
    
}

bool ContinuePopup::init(bool isAdsLoaded) {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initMenu(isAdsLoaded);
    
    return true;
}

void ContinuePopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void ContinuePopup::onExit() {
    
    BasePopup::onExit();
}

/**
 * 카운트 다운
 */
void ContinuePopup::countdown() {
    
    anim->clearTracks();
    anim->setAnimation(0, TO_STRING(count), false);
    anim->update(0);
}

/**
 * 타임 아웃
 */
void ContinuePopup::timeOut() {
    
    CCLOG("ContinuePopup::timeOut isContinue: %d", isContinue);
    
    if( isContinue ) {
        return;
    }
    
    isTimeOut = true;
    unscheduleAllCallbacks();
    
    dismissWithAction([=]() {
        if( onTimeOutListener ) {
            onTimeOutListener();
        }
    });
}

/**
 * 이어하기 버튼 클릭
 */
void ContinuePopup::onClickContinue() {
    
    CCLOG("ContinuePopup::onClickContinue isTimeOut: %d", isTimeOut);
    
    if( isTimeOut ) {
        return;
    }
    
    isContinue = true;
    
    anim->clearTracks();
    anim->stopAllActions();
    anim->unscheduleAllCallbacks();
    
    if( onContinueListener ) {
        onContinueListener();
    }
}

void ContinuePopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    // setBackgroundColor(Color::POPUP_BG);
    // setBackgroundColor(Color4B(0,0,0,255*0.5f));
}

void ContinuePopup::initContentView() {
    
    BasePopup::initContentView();
    
    // 카운트 다운
    anim = SkeletonAnimation::createWithJsonFile(ANIM_CONTINUE);
    anim->setAnchorPoint(ANCHOR_M);
    anim->setPosition(Vec2MC(0, 0));
    anim->setVisible(false);
    addChild(anim);
    
    SBSpineHelper::clearAnimation(anim, ANIM_NAME_CLEAR);
    
    // 애니메이션 시작 리스너
    anim->setStartListener([=](spTrackEntry *entry) {
        
        CCLOG("countdown animation start: %s", entry->animation->name);

        anim->setTimeScale(1);
    });
    
    // 애니메이션 완료 리스너
    anim->setCompleteListener([=](spTrackEntry *entry) {

        CCLOG("countdown animation completed: %s", entry->animation->name);
        
        if( !SBStringUtils::isInteger(entry->animation->name) ) {
            return;
        }
        
        if( count > 0 ) {
            count--;
            this->countdown();
        } else {
            this->timeOut();
        }
    });
}

void ContinuePopup::initMenu(bool isAdsLoaded) {
    
    // 터치 영역
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        if( eventType == Widget::TouchEventType::BEGAN ) {
            // 연출 5배속
            // anim->setTimeScale(5.0f);
            //
            
            // 다음 애니메이션으로 skip
            if( count > 0 ) {
                SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
                
                count--;
                this->countdown();
            } else {
                // anim->setTimeScale(5.0f);
            }
        }
    });
    
    // 이어하기 버튼
    // btn_continue.png Vec2BC(0, 392) , Size(520, 152)
    string file = "btn_continue_free.png";
    
    if( User::isOwnRemoveAdsItem() ) {
        file = "btn_continue_vip.png";
    } else if( isAdsLoaded ) {
        file = "btn_continue.png";
    }
    
    auto continueBtn = SBButton::create(DIR_IMG_GAME + file);
    continueBtn->setTag(Tag::BTN_CONTINUE);
    continueBtn->setZoomScale(0.05f);
    continueBtn->setAnchorPoint(ANCHOR_M);
    continueBtn->setPosition(Vec2BC(0, 392));
    addChild(continueBtn, 1);
    
    continueBtn->setOnClickListener([=](Node*) {
        this->onClickContinue();
    });
}

/**
 * 등장 연출
 */
void ContinuePopup::runEnterAction(SBCallback onFinished) {

    const float DURATION = MAX(ENTER_DURATION, anim->getAnimationDuration(ANIM_NAME_ENTER));
    
    BasePopup::runEnterAction(DURATION, onFinished);
    
    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 등장 애니메이션 재생
    anim->setVisible(true);
    anim->setAnimation(0, ANIM_NAME_ENTER, false);
    anim->update(0);
    
    // 버튼
    auto scale1 = ScaleTo::create(0.15f, 1.1f);
    auto scale2 = ScaleTo::create(0.05f, 1.0f);
    auto scaleSeq = Sequence::create(scale1, scale2, nullptr);
    
    getChildByTag(Tag::BTN_CONTINUE)->setScale(0);
    getChildByTag(Tag::BTN_CONTINUE)->runAction(scaleSeq);
    
    // 콜백
    auto delay = DelayTime::create(DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        // 터치 잠금 해제
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 퇴장 연출
 */
void ContinuePopup::runExitAction(SBCallback onFinished) {
    
    const float DURATION = MAX(EXIT_DURATION, anim->getAnimationDuration(ANIM_NAME_EXIT));
    
    BasePopup::runExitAction(DURATION, onFinished);

    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 퇴장 애니메이션 재생
    anim->setAnimation(0, ANIM_NAME_EXIT, false);
    
    // 버튼
    auto scale = ScaleTo::create(0.15f, 0);
    getChildByTag(Tag::BTN_CONTINUE)->runAction(scale);
    
    // 콜백
    auto delay = DelayTime::create(DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        // 터치 잠금 해제
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 등장 연출 완료
 */
void ContinuePopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 카운트 다운 시작
    countdown();
}
