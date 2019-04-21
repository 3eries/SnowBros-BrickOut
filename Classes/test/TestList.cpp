//
//  TestList.cpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#include "TestList.hpp"

#include "Define.h"

#include "case/GameConfigEdit.hpp"
#include "case/DemoViewEdit.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define CELL_HEIGHT         80
#define CELL_PADDING        10

TestList::TestList() {
    
}

TestList::~TestList() {
    
}

/**
 * 테스트 리스트 초기화
 */
void TestList::initTestList() {
    
    addTest("Game Config Edit", []() { return GameConfigEdit::create(); });
    addTest("Demo View Edit", []() { return DemoViewEdit::create(); });
}

void TestList::addTest(const string &title, function<Scene*()> getScene) {
    
    int i = (int)cells.size();
    
    auto cell = Widget::create();
    cell->setAnchorPoint(ANCHOR_M);
    cell->setPosition(Vec2(SB_WIN_SIZE.width*0.5f, 0));
    cell->setContentSize(Size(SB_WIN_SIZE.width, CELL_HEIGHT));
    scrollView->addChild(cell);
    
    cells.push_back(cell);
    
    cell->setTouchEnabled(true);
    cell->addClickEventListener([=](Ref*) {
        
        CCLOG("click: %d", i);
        
        auto scene = getScene();
        Director::getInstance()->pushScene(scene);
    });
    
    cell->addTouchEventListener([=](Ref*, Widget::TouchEventType event) {
        
        switch( event ) {
            case Widget::TouchEventType::BEGAN:     cell->setScale(1.05f);      break;
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED:  cell->setScale(1.f);        break;
            default: break;
        }
    });
    
    // cell->addChild(SBNodeUtils::createBackgroundNode(cell, Color4B(255,0,0,255*0.5f)));
    
    auto titleLabel = Label::createWithSystemFont(STR_FORMAT("%d. %s", i+1, title.c_str()),
                                                  "", CELL_HEIGHT*0.7f, Size::ZERO,
                                                  TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(cell->getContentSize(), 0, 0));
    titleLabel->setTextColor(Color4B::WHITE);
    titleLabel->setScale((CELL_HEIGHT*0.9f) / titleLabel->getContentSize().height);
    cell->addChild(titleLabel);
};

bool TestList::init() {

    if( !BaseScene::init() ) {
        return false;
    }
    
    // 스크롤뷰 생성
    scrollView = ScrollView::create();
    scrollView->setAnchorPoint(ANCHOR_M);
    scrollView->setPosition(Vec2MC(0, -20));
    scrollView->setContentSize(SB_WIN_SIZE + Size(0, -200));
    scrollView->setDirection(ScrollView::Direction::VERTICAL);
    addChild(scrollView);
    
    // 테스트 리스트 초기화
    initTestList();
    
    // Back 버튼
    auto backBtn = SBButton::create("Back", "", 50);
    backBtn->setAnchorPoint(ANCHOR_BR);
    backBtn->setPosition(Vec2BR(-10, 10));
    backBtn->setContentSize(Size(250, 80));
    addChild(backBtn);
    
    backBtn->setOnClickListener([=](Node*) {
        backBtn->setTouchEnabled(false);
        replaceScene(SceneType::MAIN);
    });
    
    // 정렬
    float containerHeight = (CELL_HEIGHT * cells.size()) + (CELL_PADDING * (cells.size()-1));
    containerHeight = MAX(scrollView->getContentSize().height, containerHeight);
    scrollView->setInnerContainerSize(Size(SB_WIN_SIZE.width, containerHeight));
    
    float posY = containerHeight;
    
    for( int i = 0; i < cells.size(); ++i ) {
        auto cell = cells[i];
        cell->setPositionY(posY-CELL_HEIGHT*0.5f);
        
        posY -= CELL_HEIGHT;
        posY -= CELL_PADDING;
    }
    
    return true;
}

bool TestList::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    replaceScene(SceneType::MAIN);
    
    return true;
}
