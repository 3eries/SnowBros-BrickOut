//
//  GiftPopup.cpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#include "GiftPopup.hpp"

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

GiftPopup* GiftPopup::create(GiftRewardItem item) {
    
    auto popup = new GiftPopup(item);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

GiftPopup::GiftPopup(GiftRewardItem item) : BasePopup(PopupType::GIFT),
giftRewardItem(item),
onGetListener(nullptr) {
    
}

GiftPopup::~GiftPopup() {
    
}

bool GiftPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void GiftPopup::onEnter() {
    
    BasePopup::onEnter();
}

bool GiftPopup::onBackKeyReleased() {
    
    if( SceneManager::getInstance()->onBackKeyReleased() ) {
        return false;
    }
    
    dismissWithAction();
    
    return true;
}

void GiftPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void GiftPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_boost.png Vec2MC(0, 52) , Size(488, 472)
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_boost.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 52));
    addContentChild(stoneBg);
    
    Size bgSize(stoneBg->getContentSize());
    
    // title
    // RSP_popup_title_boost.png Vec2TC(0, -65) , Size(360, 64)
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_boost.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(bgSize, 0, -65));
    stoneBg->addChild(title);
    
    // score
    // 256 size:132 color:255,237,0 shadow:size6, shadow_color:105,59,0 Vec2MC(0, 13) , Size(271, 95)
    auto scoreLabel = Label::createWithTTF(TO_STRING(giftRewardItem.amount), FONT_SABO, 132);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(bgSize, 0, 13 + 5));
    scoreLabel->setTextColor(Color4B(255,237,0,255));
    scoreLabel->enableShadow(Color4B(105,59,0,255), Size(0, -6));
    stoneBg->addChild(scoreLabel);
    
    // get 버튼
    // btn_boost.png Vec2BC(0, 95) , Size(360, 128)
    auto getBtn = SBButton::create(DIR_IMG_GAME + "btn_boost.png");
    getBtn->setZoomScale(0);
    getBtn->setAnchorPoint(ANCHOR_M);
    getBtn->setPosition(Vec2BC(bgSize, 0, 95));
    stoneBg->addChild(getBtn);
    
    getBtn->setOnClickListener([=](Node*) {
        getBtn->setTouchEnabled(false);
        onGetListener(this);
    });
    
    auto scale1 = ScaleTo::create(0.5f, 1.05f);
    auto scale2 = ScaleTo::create(0.5f, 1.0f);
    getBtn->runAction(RepeatForever::create(Sequence::create(scale1, scale2, nullptr)));
}

/**
 * 등장 연출
 */
void GiftPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void GiftPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void GiftPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->dismissWithAction();
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
}
