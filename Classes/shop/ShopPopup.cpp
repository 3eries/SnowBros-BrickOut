//
//  ShopPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#include "ShopPopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "UserDefaultKey.h"

#include "SceneManager.h"
#include "UIHelper.hpp"

#include "BannerView.hpp"
#include "UserCoinView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

ShopPopup::ShopPopup() : BasePopup(PopupType::SHOP) {
    
}

ShopPopup::~ShopPopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool ShopPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initCoin();
    initRemoveAds();
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

void ShopPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void ShopPopup::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        case Tag::BTN_CLOSE: {
            dismissWithAction();
        } break;
            
        case Tag::BTN_REWARDED_VIDEO: {
            
        } break;
    }
}

/**
 * 인앱 상품 구매
 */
void ShopPopup::purchase(const string &prodName) {
    
    Log::i("ShopPopup::purchase: %s", prodName.c_str());
}

void ShopPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color4B(0,0,0,255*0.85f));
    
}

void ShopPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // 배너
    auto bannerView = BannerView::create();
    bannerView->setTag(Tag::BANNER);
    addContentChild(bannerView, SBZOrder::BOTTOM);
    
    // 유저 코인
    addContentChild(UserCoinView::create(UserCoinView::Type::SHOP), SBZOrder::BOTTOM);
    
    // bg
    shopBg = Sprite::create(DIR_IMG_SHOP + "shop_bg.png");
    shopBg->setAnchorPoint(ANCHOR_M);
    shopBg->setPosition(Vec2MC(0, -30));
    addContentChild(shopBg);
    
    // title
    auto title = Sprite::create(DIR_IMG_SHOP + "shop_title.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -302));
    addContentChild(title);
    
    // 닫기 버튼
    // common_btn_close_red.png Vec2TR(-46, -146) , Size(76, 76)
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_close_red.png");
    closeBtn->setTag(Tag::BTN_CLOSE);
    closeBtn->setZoomScale(0.1f);
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(Vec2TR(-46, -146));
    addContentChild(closeBtn);
    
    closeBtn->setOnClickListener(CC_CALLBACK_1(ShopPopup::onClick, this));
}

void ShopPopup::initCoin() {
    
    string prodName[] = {
        "coin_01", "coin_02", "coin_03",
    };
    
    int amount[] = {
        500, 3000, 7500,
    };
    
    string price[] = {
        "$0.99", "$4.99", "$9.99",
    };
    
    // coin bg
    // shop_bg_coin_purchase.png Vec2MC(-220, 112) , Size(224, 328)
    // shop_bg_coin_purchase.png Vec2MC(0, 112) , Size(224, 328)
    // shop_bg_coin_purchase.png Vec2MC(220, 112) , Size(224, 328)
    Vec2 bgPos[] = {
        Vec2MC(-220, 112),
        Vec2MC(0, 112),
        Vec2MC(220, 112),
    };
    
    // coin amount
    // 500 size:48 stroke:4 Vec2MC(-218, 236) , Size(108, 45)
    // 3000 size:48 stroke:4 Vec2MC(2, 236) , Size(142, 45)
    // 7500 size:48 stroke:4 Vec2MC(222, 236) , Size(142, 45)
    Vec2 amountPos[] = {
        Vec2MC(-220, 236),
        Vec2MC(0, 236),
        Vec2MC(220, 236),
    };
    
    // coin icon
    // shop_icon_coin1.png Vec2MC(-218, 122) , Size(108, 92)
    // shop_icon_coin2.png Vec2MC(10, 132) , Size(148, 128)
    // shop_icon_coin3.png Vec2MC(220, 134) , Size(184, 132)
    Vec2 iconPos[] = {
        Vec2MC(-220, 122),
        Vec2MC(0, 132),
        Vec2MC(220, 134),
    };
    
    // coin price bg
    // shop_btn_coin_purchase.png Vec2MC(-220, 10) , Size(192, 84)
    // shop_btn_coin_purchase.png Vec2MC(0, 10) , Size(192, 84)
    // shop_btn_coin_purchase.png Vec2MC(220, 10) , Size(192, 84)
    Vec2 priceBgPos[] = {
        Vec2MC(-220, 10),
        Vec2MC(0, 10),
        Vec2MC(220, 10),
    };
    
    // coin price
    // $0.99 size:48 stroke:4 Vec2MC(-217, 20) , Size(165, 45)
    // $4.99 size:48 stroke:4 Vec2MC(2, 20) , Size(165, 45)
    // $9.99 size:48 stroke:4 Vec2MC(222, 20) , Size(165, 45)
    Vec2 pricePos[] = {
        Vec2MC(-220, 20),
        Vec2MC(0, 20),
        Vec2MC(220, 20),
    };
    
    for( int i = 0; i < 3; ++i ) {
        // shop_bg_coin_purchase.png Vec2MC(-220, 112) , Size(224, 328)
        auto bg = Sprite::create(DIR_IMG_SHOP + "shop_bg_coin_purchase.png");
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(bgPos[i]);
        addContentChild(bg);
        
        // 500 size:48 stroke:4 Vec2MC(-218, 236) , Size(108, 45)
        auto coinAmountLabel = Label::createWithTTF(TO_STRING(amount[i]), FONT_COMMODORE, 48, Size::ZERO,
                                                    TextHAlignment::CENTER, TextVAlignment::CENTER);
        coinAmountLabel->setAnchorPoint(ANCHOR_M);
        coinAmountLabel->setPosition(amountPos[i] + Vec2(0, 2));
        coinAmountLabel->setTextColor(Color4B(255,255,255,255));
        coinAmountLabel->enableOutline(Color4B(0,0,0,255), 4);
        addContentChild(coinAmountLabel);
        
        // shop_icon_coin2.png Vec2MC(10, 132) , Size(148, 128)
        string iconFile = STR_FORMAT("shop_icon_%s.png", prodName[i].c_str());
        
        auto icon = Sprite::create(DIR_IMG_SHOP + iconFile);
        icon->setAnchorPoint(ANCHOR_M);
        icon->setPosition(iconPos[i]);
        addContentChild(icon);
        
        // shop_btn_coin_purchase.png Vec2MC(-220, 10) , Size(192, 84)
        auto priceBg = SBButton::create(DIR_IMG_SHOP + "shop_btn_coin_purchase.png");
        priceBg->setZoomScale(-0.04f);
        priceBg->setAnchorPoint(ANCHOR_M);
        priceBg->setPosition(priceBgPos[i]);
        addContentChild(priceBg);
        
        priceBg->setOnClickListener([=](Node*) {
            this->purchase(prodName[i]);
            
            // FIXME: 하드코딩
            User::addCoin(amount[i]);
        });
        
        // $9.99 size:48 stroke:4 Vec2MC(222, 20) , Size(165, 45)
        auto priceLabel = Label::createWithTTF(price[i], FONT_COMMODORE, 48-3, Size::ZERO,
                                               TextHAlignment::CENTER, TextVAlignment::CENTER);
        priceLabel->setAnchorPoint(ANCHOR_M);
        // priceLabel->setPosition(pricePos[i] + Vec2(0, 2));
        priceLabel->setPosition(Vec2MC(priceBg->getContentSize(), 0, 0) + Vec2(0, 7));
        priceLabel->setTextColor(Color4B(255,255,255,255));
        priceLabel->enableOutline(Color4B(0,0,0,255), 4);
        priceBg->getContentsLayer()->addChild(priceLabel);
    }
}

void ShopPopup::initRemoveAds() {
    
    auto btn = Widget::create();
    btn->setTouchEnabled(true);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2MC(0, -136));
    addContentChild(btn);
    
    auto bg = Sprite::create(DIR_IMG_SHOP + "shop_btn_remove_ads.png");
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
        this->purchase("remove_ads");
    });
    
    // amount
    // $2.99 size:48 stroke:4 Vec2MC(216, -131) , Size(165, 45)
    auto amountLabel = Label::createWithTTF("$2.99", FONT_COMMODORE, 48-2, Size::ZERO,
                                            TextHAlignment::RIGHT, TextVAlignment::CENTER);
    amountLabel->setAnchorPoint(ANCHOR_MR);
    amountLabel->setPosition(btn->convertToNodeSpace(Vec2MC(216, -131) + Vec2(165*0.5f, 2)));
    amountLabel->setTextColor(Color4B(255,255,255,255));
    amountLabel->enableOutline(Color4B(0,0,0,255), 4);
    btn->addChild(amountLabel);
}

void ShopPopup::initRewardedVideo() {
    
    // rewarded video
    // shop_btn_reward.png Vec2MC(0, -300) , Size(664, 160)
    auto btn = Widget::create();
    btn->setTag(Tag::BTN_REWARDED_VIDEO);
    btn->setTouchEnabled(true);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2MC(0, -300));
    addContentChild(btn);
    
    auto bg = Sprite::create(DIR_IMG_SHOP + "shop_btn_reward.png");
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
    
    // common_icon_coin_small.png Vec2MC(129, -295) , Size(45, 45)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(btn->convertToNodeSpace(Vec2MC(129, -295)));
    btn->addChild(coinIcon);
    
    // +999 size:48 stroke:4 color:0,255,0 Vec2MC(227, -295) , Size(143, 45)
    auto amountLabel = Label::createWithTTF("+999", FONT_COMMODORE, 48-2, Size::ZERO,
                                            TextHAlignment::RIGHT, TextVAlignment::CENTER);
    amountLabel->setAnchorPoint(ANCHOR_MR);
    amountLabel->setPosition(btn->convertToNodeSpace(Vec2MC(227, -295) + Vec2(143*0.5f, 2)));
    amountLabel->setTextColor(Color4B(0,255,0,255));
    amountLabel->enableOutline(Color4B(0,0,0,255), 4);
    btn->addChild(amountLabel);
}

/**
 * 등장 연출
 */
void ShopPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(0, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 퇴장 연출
 */
void ShopPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 등장 연출 완료
 */
void ShopPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
    
    auto bgTouchNode = SBNodeUtils::createTouchNode();
    bgTouchNode->setAnchorPoint(shopBg->getAnchorPoint());
    bgTouchNode->setPosition(shopBg->getPosition());
    bgTouchNode->setContentSize(shopBg->getContentSize());
    addChild(bgTouchNode);
}
