//
//  RestorePopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#include "RestorePopup.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"

#include "BannerView.hpp"
#include "UserCoinView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

RestorePopup::RestorePopup() : BasePopup(PopupType::GAME_RESTORE),
onRestoreListener(nullptr) {
    
}

RestorePopup::~RestorePopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool RestorePopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initRestore();
    initRewardedVideo();
    
    // IAP 리스너
    {
        auto onRemoveAds = [=]() {
            // TODO:
        };
        
        // purchase listener
        auto purchaseListener = iap::PurchaseListener::create();
        purchaseListener->setForever(true);
        purchaseListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
        
        // restore listener
        auto restoreListener = iap::RestoreListener::create();
        restoreListener->setForever(true);
        restoreListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, restoreListener);
    }
    
    return true;
}

void RestorePopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void RestorePopup::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 닫기
        case Tag::BTN_CLOSE: {
            onRestoreListener(false);
        } break;
            
        // 리스토어
        case Tag::BTN_RESTORE: {
            // 코인 체크
            if( !User::isEnoughCoin(GAME_CONFIG->getRestoreCost()) ) {
                PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
                return;
            }
            
            User::spendCoin(GAME_CONFIG->getRestoreCost());
            onRestoreListener(true);
            
        } break;
            
        // 광고
        case Tag::BTN_REWARDED_VIDEO: {
            
        } break;
    }
}

void RestorePopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color4B(0,0,0,255*0.85f));
    
}

void RestorePopup::initContentView() {
    
    BasePopup::initContentView();
    
    // 배너
    auto bannerView = BannerView::create();
    bannerView->setTag(Tag::BANNER);
    addContentChild(bannerView, SBZOrder::BOTTOM);
    
    // 유저 코인
    addContentChild(UserCoinView::create(UserCoinView::Type::NORMAL), SBZOrder::BOTTOM);
    
    // 배경
    {
        // restore_timer_long_hand.png Vec2MC(0, 96) , Size(80, 200)
        // restore_timer_short_hand.png Vec2MC(100, 36) , Size(108, 48)
        // restore_timer_circle.png Vec2MC(-1, 50) , Size(582, 582)
        auto longHand = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_timer_long_hand.png");
        longHand->setAnchorPoint(ANCHOR_M);
        longHand->setPosition(Vec2MC(0, 96));
        addContentChild(longHand);
        
        auto shortHand = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_timer_short_hand.png");
        shortHand->setAnchorPoint(ANCHOR_M);
        shortHand->setPosition(Vec2MC(100, 36));
        addContentChild(shortHand);
        
        auto circle = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_timer_circle.png");
        circle->setAnchorPoint(ANCHOR_M);
        circle->setPosition(Vec2MC(0, 50));
        addContentChild(circle);
    }
    
    // title
    // restore_title.png Vec2TC(-1, -276) , Size(576, 88)
    auto title = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_title.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -276));
    addContentChild(title);
    
    // 닫기 버튼
    // common_btn_close_red.png Vec2TR(-46, -146) , Size(76, 76)
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_close_red.png");
    closeBtn->setTag(Tag::BTN_CLOSE);
    closeBtn->setZoomScale(0.1f);
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(Vec2TR(-46, -146));
    addContentChild(closeBtn);
    
    closeBtn->setOnClickListener(CC_CALLBACK_1(RestorePopup::onClick, this));
}

/**
 * 리스토어 버튼 초기화
 */
void RestorePopup::initRestore() {
    
    // restore_btn_bg_ok.png Vec2BC(0, 312) , Size(480, 160)
    auto btn = Widget::create();
    btn->setTag(Tag::BTN_RESTORE);
    btn->setTouchEnabled(true);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2BC(0, 312));
    addContentChild(btn);
    
    auto bg = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_btn_bg_ok.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(bg->getContentSize(), 0, 0));
    btn->addChild(bg);
    
    btn->setContentSize(bg->getContentSize());
    btn->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN:     btn->setScale(0.97f);    break;
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED:  btn->setScale(1.0f);    break;
            default: break;
        }
    });
    
    btn->addClickEventListener([=](Ref*) {
        this->onClick(btn);
    });
    
    // common_icon_coin_small.png Vec2BC(-72, 279) , Size(45, 45)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(btn->convertToNodeSpace(Vec2BC(-72, 279)));
    btn->addChild(coinIcon);
    
    // 9999 size:42 stroke:4 Vec2BC(29, 279) , Size(131, 41)
    auto amountLabel = Label::createWithTTF(TO_STRING(GAME_CONFIG->getRestoreCost()), FONT_COMMODORE, 42, Size::ZERO,
                                            TextHAlignment::RIGHT, TextVAlignment::CENTER);
    amountLabel->setAnchorPoint(ANCHOR_MR);
    amountLabel->setPosition(btn->convertToNodeSpace(Vec2BC(29, 279) + Vec2(131*0.5f, 2)));
    amountLabel->setTextColor(Color4B(255,255,255,255));
    amountLabel->enableOutline(Color4B(0,0,0,255), 4);
    btn->addChild(amountLabel);
    
    // restore_btn_text_ok.png Vec2BC(0, 336) , Size(108, 64)
    auto okText = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_btn_text_ok.png");
    okText->setAnchorPoint(ANCHOR_M);
    okText->setPosition(btn->convertToNodeSpace(Vec2BC(0, 336)));
    btn->addChild(okText);
}

/**
 * 광고 보기 버튼 초기화
 */
void RestorePopup::initRewardedVideo() {
    
    // rewarded video
    // restore_btn_bg_ad.png Vec2BC(0, 176) , Size(332, 88)
    auto btn = Widget::create();
    btn->setTag(Tag::BTN_REWARDED_VIDEO);
    btn->setTouchEnabled(true);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2BC(0, 176));
    addContentChild(btn);
    
    auto bg = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_btn_bg_ad.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(bg->getContentSize(), 0, 0));
    btn->addChild(bg);
    
    btn->setContentSize(bg->getContentSize());
    btn->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN:     btn->setScale(0.97f);    break;
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED:  btn->setScale(1.0f);    break;
            default: break;
        }
    });
    
    btn->addClickEventListener([=](Ref*) {
        this->onClick(btn);
    });
    
    // restore_icon_ad.png Vec2BC(-102, 176) , Size(72, 56)
    auto adIcon = Sprite::create(DIR_IMG_GAME_RESTORE + "restore_icon_ad.png");
    adIcon->setAnchorPoint(ANCHOR_M);
    adIcon->setPosition(btn->convertToNodeSpace(Vec2BC(-102, 176)));
    btn->addChild(adIcon);
    
    // common_icon_coin_small.png Vec2BC(-21, 175) , Size(45, 45)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(btn->convertToNodeSpace(Vec2BC(-21, 175)));
    btn->addChild(coinIcon);
    
    // +999 size:48 stroke:4 color:154,25,255 Vec2BC(73, 176) , Size(144, 45)
    auto amountLabel = Label::createWithTTF("+999", FONT_COMMODORE, 48-2, Size::ZERO,
                                            TextHAlignment::RIGHT, TextVAlignment::CENTER);
    amountLabel->setAnchorPoint(ANCHOR_MR);
    amountLabel->setPosition(btn->convertToNodeSpace(Vec2BC(73, 176) + Vec2(144*0.5f, 2)));
    amountLabel->setTextColor(Color4B(154,25,255,255));
    amountLabel->enableOutline(Color4B(0,0,0,255), 4);
    btn->addChild(amountLabel);
}

/**
 * 등장 연출
 */
void RestorePopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(0, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 퇴장 연출
 */
void RestorePopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 등장 연출 완료
 */
void RestorePopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
