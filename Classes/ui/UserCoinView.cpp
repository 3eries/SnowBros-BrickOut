//
//  UserCoinView.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#include "UserCoinView.hpp"

#include "Define.h"
#include "User.hpp"
#include "PopupManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

UserCoinView* UserCoinView::create(Type type) {
    
    auto view = new UserCoinView();
    
    if( view && view->init(type) ) {
        view->autorelease();
        return view;
    }
    
    CC_SAFE_DELETE(view);
    return nullptr;
}

UserCoinView::UserCoinView() :
updateLocekd(false) {
}

UserCoinView::~UserCoinView() {
}

bool UserCoinView::init(Type type) {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    // UI 초기화
    switch( type ) {
        case Type::NORMAL:      initNormal();       break;
        case Type::SHOP:        initShop();         break;
    }
    
    updateCoin(false);
    
    // 코인 업데이트 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_USER_COIN, [=](EventCustom *event) {
        this->updateCoin(true);
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void UserCoinView::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 상점
        case Tag::SHOP_BTN: {
            PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
        } break;
            
        default: break;
    }
}

/**
 * 코인 업데이트
 */
void UserCoinView::updateCoin(bool withAction) {
    
    if( updateLocekd ) {
        return;
    }
    
    auto coinLabel = getChildByTag<Label*>(Tag::COIN_LABEL);
    int  coin = User::getCoin();
    
    if( !withAction ) {
        coinLabel->setString(TO_STRING(coin));
        return;
    }
    
    // 액션
    auto onChanged = [=](float value) {
        
        int i = (int)value;
        coinLabel->setString(TO_STRING(i));
    };
    
    auto numberAction = ActionFloat::create(0.3f, TO_INTEGER(coinLabel->getString()), coin, onChanged);
    coinLabel->runAction(numberAction);
}

/**
 * 기본 UI 초기화
 */
void UserCoinView::initNormal() {
    
    // bg
    // common_bg_coin_user.png Vec2TL(112, -138) , Size(200, 52)
    auto bg = SBButton::create(DIR_IMG_COMMON + "common_bg_coin_user.png");
    bg->setTag(Tag::BG);
    bg->setZoomScale(0);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2TL(112, -138));
    addChild(bg);
    
    bg->setOnClickListener([=](Node*) {
        this->onClick(getChildByTag(Tag::SHOP_BTN));
    });
    
    // icon
    // common_icon_coin_tiny.png Vec2TL(38, -138) , Size(36, 36)
    auto icon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
    icon->setTag(Tag::COIN_ICON);
    icon->setAnchorPoint(ANCHOR_M);
    icon->setPosition(Vec2TL(38, -138));
    addChild(icon);
    
    // coin
    // 999999 size:28 Vec2TL(116, -137) , Size(119, 19)
    auto coinLabel = Label::createWithTTF("0", FONT_COMMODORE, 28, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    coinLabel->setTag(Tag::COIN_LABEL);
    coinLabel->setAnchorPoint(ANCHOR_M);
    coinLabel->setPosition(Vec2TL(116, -137) + Vec2(0, 0));
    coinLabel->setTextColor(Color4B(255,255,255,255));
    addChild(coinLabel);
    
    // shop
    // common_btn_shop.png Vec2TL(208, -136) , Size(56, 56)
    auto shopBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_shop.png");
    shopBtn->setTag(Tag::SHOP_BTN);
    shopBtn->setAnchorPoint(ANCHOR_M);
    shopBtn->setPosition(Vec2TL(208, -136));
    shopBtn->setZoomScale(ButtonZoomScale::HARD);
    addChild(shopBtn);
    
    shopBtn->setOnClickListener(CC_CALLBACK_1(UserCoinView::onClick, this));
}

/**
 * 상점 전용 UI 초기화
 */
void UserCoinView::initShop() {
    
    // shop_bg_coin_user.png Vec2TL(112, -138) , Size(200, 52)
    auto bg = Sprite::create(DIR_IMG_SHOP + "shop_bg_coin_user.png");
    bg->setTag(Tag::BG);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2TL(112, -138));
    addChild(bg);
    
    // common_icon_coin_tiny.png Vec2TL(38, -138) , Size(36, 36)
    auto icon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
    icon->setTag(Tag::COIN_ICON);
    icon->setAnchorPoint(ANCHOR_M);
    icon->setPosition(Vec2TL(38, -138));
    addChild(icon);
    
    // 999999 size:32 Vec2TL(129, -138) , Size(139, 22)
    auto coinLabel = Label::createWithTTF("0", FONT_COMMODORE, 32, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    coinLabel->setTag(Tag::COIN_LABEL);
    coinLabel->setAnchorPoint(ANCHOR_M);
    coinLabel->setPosition(Vec2TL(129, -138) + Vec2(0, 0));
    coinLabel->setTextColor(Color4B(255,255,255,255));
    addChild(coinLabel);
}

Node* UserCoinView::getBackground() {
    return getChildByTag(Tag::BG);
}

Node* UserCoinView::getCoinIcon() {
    return getChildByTag(Tag::COIN_ICON);
}

Node* UserCoinView::getCoinLabel() {
    return getChildByTag(Tag::COIN_LABEL);
}

Node* UserCoinView::getShopButton() {
    return getChildByTag(Tag::SHOP_BTN);
}

