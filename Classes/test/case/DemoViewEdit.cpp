//
//  DemoViewEdit.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#include "DemoViewEdit.hpp"

#include "Define.h"
#include "ContentManager.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

DemoViewEdit::DemoViewEdit() {
}

DemoViewEdit::~DemoViewEdit() {
}

bool DemoViewEdit::init() {
    
    if( !TestCase::init() ) {
        return false;
    }
    
    setTitle("Demo View Edit");
    
    // 메뉴 초기화
    auto testHelper = TestHelper::getInstance();
    
    const int FONT_SIZE = 50;
    posY = -200;
    
    // ball on/off
    {
        auto label = Label::createWithTTF("BALL", FONT_COMMODORE, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_MR);
        label->setPosition(Vec2TC(-50, posY));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2TC(150, posY));
        addChild(checkBox);
        
        checkBox->setSelected(testHelper->isDemoViewBallEnabled());
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            testHelper->setDemoViewBallEnabled(isSelected);
            
            return false;
        });
    }
    
    return true;
}

