//
//  TopMenu.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#include "TopMenu.hpp"

#include "Define.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

TopMenu* TopMenu::create() {
    
    auto menu = new TopMenu();
    
    if( menu && menu->init() ) {
        menu->autorelease();
        return menu;
    }
    
    CC_SAFE_DELETE(menu);
    return nullptr;
}

TopMenu::TopMenu() {
    
}

TopMenu::~TopMenu() {
    
    removeListeners(this);
}

bool TopMenu::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    // game_bg_top.png Vec2TC(0, -37) , Size(720, 74)
    setAnchorPoint(ANCHOR_MT);
    setPosition(Vec2TC(0,0));
    
    // 배경
    auto topBg = Sprite::create(DIR_IMG_GAME + "game_bg_top.png");
    topBg->setAnchorPoint(ANCHOR_M);
    topBg->setPosition(Vec2MC(topBg->getContentSize(), 0, 0));
    addChild(topBg);
    
    Size bgSize(topBg->getContentSize());
    setContentSize(bgSize);
    
    // 스테이지
    auto stageLabel = Label::createWithTTF("STAGE 1", FONT_COMMODORE, 28, Size::ZERO,
                                           TextHAlignment::LEFT, TextVAlignment::CENTER);
    stageLabel->setTag(Tag::STAGE);
    stageLabel->setAnchorPoint(ANCHOR_ML);
    stageLabel->setPosition(Vec2ML(bgSize, 92 - 122*0.5f, 0));
    stageLabel->setTextColor(Color4B::WHITE);
    stageLabel->enableOutline(Color4B::BLACK, 3);
    addChild(stageLabel);
    
    // 스코어
    auto scoreLabel = Label::createWithTTF("0", FONT_COMMODORE, 28, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setTag(Tag::SCORE);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(bgSize, 0, 0));
    scoreLabel->setTextColor(Color4B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 3);
    addChild(scoreLabel);
    
    // Floor 진행바
    const float posY = -20;
    
    // Gage
    auto gageBg = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_bg.png");
    gageBg->setAnchorPoint(ANCHOR_M);
    gageBg->setPosition(Vec2BC(bgSize, 0, posY));
    addChild(gageBg);
    
    auto gageBgSize = gageBg->getContentSize();
    
    auto gage = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
    gage->setAnchorPoint(ANCHOR_ML);
    gage->setPosition(Vec2ML(gageBgSize, (gageBgSize.width - gage->getContentSize().width)*0.5f, 0));
    gage->setScaleX(1);
    gageBg->addChild(gage);
    
    floorProgressBar.gage = gage;
    
    // Label
    floorProgressBar.label = Label::createWithTTF("", FONT_COMMODORE, 22, Size::ZERO,
                                                  TextHAlignment::LEFT, TextVAlignment::CENTER);
    floorProgressBar.label->setAnchorPoint(ANCHOR_ML);
    floorProgressBar.label->setPosition(Vec2BC(bgSize, 100, posY));
    floorProgressBar.label->setTextColor(Color4B::WHITE);
    floorProgressBar.label->enableOutline(Color4B::BLACK, 3);
    addChild(floorProgressBar.label);
    
    // Listener
    initGameListener();
    
    return true;
}

void TopMenu::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * Floor 진행바 UI 업데이트
 */
void TopMenu::updateFloorProgressUI(int floor, int floorLen) {
    
    floorProgressBar.gage->setScaleX((float)floor / floorLen);
    floorProgressBar.label->setString(STR_FORMAT("%d/%d", floor, floorLen));
}

/**
 * 게임 진입
 */
void TopMenu::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void TopMenu::onGameExit() {
}

/**
 * 게임 리셋
 */
void TopMenu::onGameReset() {
}

/**
 * 게임 시작
 */
void TopMenu::onGameStart() {
    
    updateFloorProgressUI(GameManager::getFloor().floor, GameManager::getStage().floorLen);
}

/**
 * 게임 재시작
 */
void TopMenu::onGameRestart() {
}

/**
 * 게임 오버
 */
void TopMenu::onGameOver() {
}

/**
 * 게임 이어하기
 */
void TopMenu::onGameContinue() {
}

/**
 * 게임 결과
 */
void TopMenu::onGameResult() {
}

/**
 * 부스트 시작
 */
void TopMenu::onBoostStart() {
}

/**
 * 부스트 종료
 */
void TopMenu::onBoostEnd() {
}

/**
 * 스테이지 클리어
 */
void TopMenu::onStageClear() {
}

/**
 * 다음 스테이지
 */
void TopMenu::onNextStage(const StageData &stage) {
    
    auto stageLabel = getChildByTag<Label*>(Tag::STAGE);
    
    if( stageLabel ) {
        stageLabel->setString(TO_STRING(stage.stage));
    }
}

/**
 * 다음 층
 */
void TopMenu::onNextFloor(const FloorData &floor) {
    
    if( !floor.isNull() ) {
        updateFloorProgressUI(floor.floor, GameManager::getStage().floorLen);
    }
}

/**
 * 스코어 변경
 */
void TopMenu::onScoreChanged(int score) {
    
    auto scoreLabel = getChildByTag<Label*>(Tag::SCORE);
    
    if( scoreLabel ) {
        scoreLabel->setString(TO_STRING(score));
    }
}

/**
 * 게임 리스너 초기화
 */
void TopMenu::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter            = CC_CALLBACK_0(TopMenu::onGameEnter, this);
    listener->onGameExit             = CC_CALLBACK_0(TopMenu::onGameExit, this);
    listener->onGameReset            = CC_CALLBACK_0(TopMenu::onGameReset, this);
    listener->onGameStart            = CC_CALLBACK_0(TopMenu::onGameStart, this);
    listener->onGameRestart          = CC_CALLBACK_0(TopMenu::onGameRestart, this);
    listener->onGameOver             = CC_CALLBACK_0(TopMenu::onGameOver, this);
    listener->onGameContinue         = CC_CALLBACK_0(TopMenu::onGameContinue, this);
    listener->onGameResult           = CC_CALLBACK_0(TopMenu::onGameResult, this);
    listener->onBoostStart           = CC_CALLBACK_0(TopMenu::onBoostStart, this);
    listener->onBoostEnd             = CC_CALLBACK_0(TopMenu::onBoostEnd, this);
    listener->onStageClear           = CC_CALLBACK_0(TopMenu::onStageClear, this);
    listener->onNextStage            = CC_CALLBACK_1(TopMenu::onNextStage, this);
    listener->onNextFloor            = CC_CALLBACK_1(TopMenu::onNextFloor, this);
    listener->onScoreChanged         = CC_CALLBACK_1(TopMenu::onScoreChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

