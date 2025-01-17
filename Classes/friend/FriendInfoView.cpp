//
//  FriendInfoView.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 18/04/2019.
//

#include "FriendInfoView.hpp"

#include "Define.h"
#include "User.hpp"
#include "ContentManager.hpp"

#include "preview/FriendPreviewView.hpp"

USING_NS_CC;
using namespace std;

FriendInfoView::FriendInfoView() :
onClickButtonListener(nullptr) {
}

FriendInfoView::~FriendInfoView() {
}

bool FriendInfoView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(Size::ZERO);
    
    initPreview();
    initText();
    initButton();
    
    return true;
}

/**
 * 프렌드 설정
 */
void FriendInfoView::setFriend(const FriendData &data) {
    
    this->data = data;
    string friendId = data.friendId;
    
    // update preview
    removeChildByTag(Tag::PREVIEW);
    
    auto previewView = FriendPreviewView::create(data);
    previewView->setTag(Tag::PREVIEW);
    addChild(previewView);
    
    // update text
    auto nameLabel = getChildByTag<Label*>(Tag::LABEL_NAME);
    nameLabel->setOverflow(Label::Overflow::NONE);
    nameLabel->setString(data.name);
    nameLabel->setOverflow(Label::Overflow::SHRINK);
    
    auto typeLabel = getChildByTag<Label*>(Tag::LABEL_TYPE);
    typeLabel->setOverflow(Label::Overflow::NONE);
    typeLabel->setString(friendTypeToString(data.type));
    typeLabel->setOverflow(Label::Overflow::SHRINK);
    
    getChildByTag<Label*>(Tag::LABEL_DAMAGE)->setString(TO_STRING(data.damage));
    
    // update button
    bool isOwned = User::isOwnedFriend(friendId);
    bool isExistDeck = User::isExistFriendsDeck(friendId);
    
    getChildByTag(Tag::BTN_SELECT)->setVisible(isOwned && !isExistDeck);
    getChildByTag(Tag::BTN_UNSELECT)->setVisible(isExistDeck);
    
    auto buyBtn = getChildByTag<SBButton*>(Tag::BTN_BUY);
    buyBtn->setVisible(!isOwned);
    
    // update price
    if( !isOwned ) {
        auto priceLabel = buyBtn->getContentsLayer()->getChildByTag<Label*>(Tag::LABEL_PRICE);
        priceLabel->setString(TO_STRING(data.price));
    }
}

void FriendInfoView::updateSelf() {
    
    setFriend(data);
}

/**
 * 프리뷰 초기화
 */
void FriendInfoView::initPreview() {
    
}

/**
 * 텍스트 설명 초기화
 */
void FriendInfoView::initText() {
    
    // title
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2TR(-276, -244),    DIR_IMG_FRIEND + "friends_text_name.png"),
        SBUIInfo(ANCHOR_M, Vec2TR(-256, -312),    DIR_IMG_FRIEND + "friends_text_damage.png"),
        SBUIInfo(ANCHOR_M, Vec2TR(-274, -380),    DIR_IMG_FRIEND + "friends_text_type.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(info.file);
        info.apply(spr);
        addChild(spr);
    }
    
    // name
    auto nameLabel = Label::createWithTTF("NICK", FONT_COMMODORE, 36, Size(165, 50),
                                          TextHAlignment::LEFT, TextVAlignment::CENTER);
    nameLabel->setTag(Tag::LABEL_NAME);
    nameLabel->setAnchorPoint(ANCHOR_M);
    nameLabel->setPosition(Vec2TR(-130, -244 + 2));
    nameLabel->setOverflow(Label::Overflow::SHRINK);
    nameLabel->setTextColor(Color4B(255,255,255,255));
    nameLabel->enableOutline(Color4B::BLACK, 4);
    addChild(nameLabel);
    
    // damage
    auto damageLabel = Label::createWithTTF("1", FONT_COMMODORE, 36, Size::ZERO,
                                            TextHAlignment::CENTER, TextVAlignment::CENTER);
    damageLabel->setTag(Tag::LABEL_DAMAGE);
    damageLabel->setAnchorPoint(ANCHOR_M);
    damageLabel->setPosition(Vec2TR(-160, -312 + 2));
    damageLabel->setTextColor(Color4B(255,255,255,255));
    damageLabel->enableOutline(Color4B::BLACK, 4);
    addChild(damageLabel);
    
    // type
    auto typeLabel = Label::createWithTTF("NORMAL", FONT_COMMODORE, 36, Size(165, 50),
                                          TextHAlignment::LEFT, TextVAlignment::CENTER);
    typeLabel->setTag(Tag::LABEL_TYPE);
    typeLabel->setAnchorPoint(ANCHOR_M);
    typeLabel->setPosition(Vec2TR(-130, -380 + 2));
    typeLabel->setOverflow(Label::Overflow::SHRINK);
    typeLabel->setTextColor(Color4B(255,255,255,255));
    typeLabel->enableOutline(Color4B::BLACK, 4);
    addChild(typeLabel);
}

/**
 * 버튼 초기화
 */
void FriendInfoView::initButton() {
    
    Vec2 BTN_POSITION = Vec2TR(-184, -478);
    
    Button buttonTypes[] = {
        Button::BUY,
        Button::SELECT,
        Button::UNSELECT,
    };
    
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_BUY,      ANCHOR_M, BTN_POSITION,    DIR_IMG_FRIEND + "friends_btn_buy.png"),
        SBUIInfo(Tag::BTN_SELECT,   ANCHOR_M, BTN_POSITION,    DIR_IMG_FRIEND + "friends_btn_select.png"),
        SBUIInfo(Tag::BTN_UNSELECT, ANCHOR_M, BTN_POSITION,    DIR_IMG_FRIEND + "friends_btn_unselect.png"),
    };
    
    map<Button, SBButton*> btns;
    
    for( int i = 0; i < sizeof(buttonTypes)/sizeof(Button); ++i ) {
        auto type = buttonTypes[i];
        auto info = infos[i];
        
        auto btn = SBButton::create(info.file);
        btn->setZoomScale(ButtonZoomScale::WEAK);
        btn->setVisible(false);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener([=](Node*) {
            onClickButtonListener(type);
        });
    }

    auto buyBtn = getChildByTag<SBButton*>(Tag::BTN_BUY);
    
    // coin icon
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(buyBtn->getContentsLayer()->convertToNodeSpace(Vec2TR(-184, -472)));
    buyBtn->getContentsLayer()->addChild(coinIcon);
    
    // price
    auto priceLabel = Label::createWithTTF("0", FONT_COMMODORE, 34, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    priceLabel->setTag(Tag::LABEL_PRICE);
    priceLabel->setAnchorPoint(ANCHOR_M);
    priceLabel->setPosition(buyBtn->getContentsLayer()->convertToNodeSpace(Vec2TR(-110, -471 + 1)));
    priceLabel->setTextColor(Color4B(255,255,255,255));
    priceLabel->enableOutline(Color4B::BLACK, 4);
    buyBtn->getContentsLayer()->addChild(priceLabel);
}
