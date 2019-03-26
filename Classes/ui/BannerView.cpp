//
//  BannerView.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/03/2019.
//

#include "BannerView.hpp"

#include "Define.h"

USING_NS_CC;
USING_NS_SB;

BannerView::BannerView() {
}

BannerView::~BannerView() {
}

bool BannerView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(Size::ZERO);
    
    auto bg = Sprite::create(DIR_IMG_COMMON + "common_banner_bg.png");
    bg->setAnchorPoint(ANCHOR_MT);
    bg->setPosition(Vec2TC(0,0));
    addChild(bg);
    
    auto loading = Sprite::create(DIR_IMG_COMMON + "common_banner_loading.png");
    loading->setAnchorPoint(ANCHOR_M);
    loading->setPosition(Vec2MC(bg->getContentSize(), 0,0));
    bg->addChild(loading);
    
    return true;
}

void BannerView::onEnter() {
    
    Node::onEnter();
    
    updateNativeUI();
}

void BannerView::onExit() {
    
    // TODO: hide native ui
    
    Node::onExit();
}

void BannerView::setVisible(bool isVisible) {
    
    Node::setVisible(isVisible);
    
    updateNativeUI();
}

void BannerView::updateNativeUI() {
    
    // TODO:
}
