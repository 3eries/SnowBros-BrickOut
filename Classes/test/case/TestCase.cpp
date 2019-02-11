//
//  TestCase.cpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#include "TestCase.hpp"

USING_NS_CC;
using namespace std;

TestCase::TestCase() :
titleLabel(nullptr) {
}

TestCase::~TestCase() {
}

bool TestCase::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    titleLabel = Label::createWithSystemFont("", "", 60, Size::ZERO,
                                             TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_MT);
    titleLabel->setPosition(Vec2TC(0, -20));
    titleLabel->setTextColor(Color4B::WHITE);
    addChild(titleLabel);
    
    // Back 버튼
    auto backBtn = SBButton::create("Back", "", 50);
    backBtn->setAnchorPoint(ANCHOR_BR);
    backBtn->setPosition(Vec2BR(-10, 10));
    backBtn->setContentSize(Size(250, 80));
    addChild(backBtn);
    
    backBtn->setOnClickListener([=](Node*) {
        backBtn->setTouchEnabled(false);
        Director::getInstance()->popScene();
    });
    
    return true;
}

bool TestCase::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    Director::getInstance()->popScene();
    
    return true;
}

void TestCase::setTitle(const string &title) {
    
    if( titleLabel ) {
        titleLabel->setString(title);
    }
}
