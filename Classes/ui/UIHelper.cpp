//
//  UIHelper.cpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// 버튼 정의
const Size ButtonSize::SMALL              = Size(200, 80);
const Size ButtonSize::MEDIUM             = Size(250, 80);
const Size ButtonSize::LARGE              = Size(300, 80);

/**
 * 기본 버튼 생성
 */
SBButton* UIHelper::createButton(SBButton::Config config) {
    
    auto btn = SBButton::create(config);
    btn->setZoomScale(0.1f);
    return btn;
}

/**
 * 폰트 버튼 생성
 */
SBButton* UIHelper::createFontButton(const string &title, const Size &size) {
    
    auto btn = createButton(SBButton::Config("", size, title, FONT_RETRO, 50));
    
    auto titleLabel = btn->getTitle();
    titleLabel->setColor(Color3B::WHITE);
    titleLabel->enableOutline(Color4B::BLACK, 3);
    
    return btn;
}

/**
 * VIP 마크 생성
 */
Sprite* UIHelper::createVIPMark(Node *removeAdsBtn) {
    
    auto vip = Sprite::create(DIR_IMG_GAME + "RSP_btn_vip_version.png");
    vip->setAnchorPoint(removeAdsBtn->getAnchorPoint());
    vip->setPosition(removeAdsBtn->getPosition());
    removeAdsBtn->getParent()->addChild(vip, removeAdsBtn->getLocalZOrder()+1);
    
    return vip;
}
