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
    
    setAnchorPoint(ANCHOR_MT);
    setPosition(Vec2TC(0,0));
    
    // UI
    initBg();
    initStage();
    initScore();
    initMenu();
    
    // Listener
    initGameListener();
    
    return true;
}

void TopMenu::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

void TopMenu::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_PAUSE: {
            GameManager::onGamePause();
        } break;
            
        default: break;
    }
}

/**
 * Stage UI 업데이트
 */
void TopMenu::updateStageUI(const StageData &stage) {
    
    auto stageLabel = getChildByTag<Label*>(Tag::STAGE);
    
    if( stageLabel ) {
        stageLabel->setString(STR_FORMAT("%d", stage.stage));
    }
}

/**
 * Floor 진행바 UI 업데이트
 */
void TopMenu::updateFloorProgressUI(const FloorData &floor) {
    
    auto stage = GameManager::getStage();
    
    if( floor.isNull() || floor.floor > stage.bossFloor ) {
        // floorProgressBar.layer->setVisible(false);
        return;
    }
    
    int floorLen = stage.bossFloor;
    
    // floorProgressBar.layer->setVisible(true);
    floorProgressBar.gage->setScaleX((float)floor.floor / floorLen);
    floorProgressBar.label->setString(STR_FORMAT("%d/%d", floor.floor, floorLen));
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
    
    auto stage = GameManager::getStage();
    
    updateStageUI(stage);
    updateFloorProgressUI(GameManager::getFloor());
}

/**
 * 게임 시작
 */
void TopMenu::onGameStart() {
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
    
    updateStageUI(stage);
}

/**
 * 층 변경
 */
void TopMenu::onFloorChanged(const FloorData &floor) {
    
    updateFloorProgressUI(floor);
}

/**
 * 다음 층
 */
void TopMenu::onNextFloor(const FloorData &floor) {
    
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
 * 배경 UI 초기화
 */
void TopMenu::initBg() {
    
    auto topBg = Sprite::create(DIR_IMG_GAME + "game_bg_top.png");
    topBg->setAnchorPoint(ANCHOR_M);
    topBg->setPosition(Vec2MC(topBg->getContentSize(), 0, 0));
    addChild(topBg);
    
    Size bgSize(topBg->getContentSize());
    setContentSize(bgSize);
}

/**
 * 스테이지 UI 초기화
 */
void TopMenu::initStage() {
    
    const auto bgSize(getContentSize());
    
    // 스테이지
    // game_bg_stage.png Vec2TL(76, -36) , Size(144, 72)
    auto stageBg = Sprite::create(DIR_IMG_GAME + "game_bg_stage.png");
    stageBg->setAnchorPoint(ANCHOR_M);
    stageBg->setPosition(Vec2TL(bgSize, 76, -36));
    addChild(stageBg, 1);
    
    // BM_font Vec2TL(76, -44) , Size(64, 40)
    auto stageLabel = Label::createWithBMFont(FONT_GAME_STAGE, "1",
                                              TextHAlignment::CENTER);
    stageLabel->setTag(Tag::STAGE);
    stageLabel->setAnchorPoint(ANCHOR_M);
    stageLabel->setPosition(Vec2TL(bgSize, 76, -44 + 4));
    addChild(stageLabel, 1);
    
    // Floor 진행바
    auto layer = SBNodeUtils::createZeroSizeNode();
    addChild(layer);
    
    floorProgressBar.layer = layer;
    
    // Gage
    // game_gage_stage_bg.png Vec2TL(218, -38) , Size(164, 52)
    auto gageBg = Sprite::create(DIR_IMG_GAME + "game_gage_stage_bg.png");
    gageBg->setAnchorPoint(ANCHOR_M);
    gageBg->setPosition(Vec2TL(bgSize, 218, -38));
    layer->addChild(gageBg);
    
    auto gageBgSize = gageBg->getContentSize();
    
    auto gage = Sprite::create(DIR_IMG_GAME + "game_gage_stage.png");
    gage->setAnchorPoint(ANCHOR_ML);
    gage->setPosition(Vec2ML(gageBgSize, (gageBgSize.width - gage->getContentSize().width)*0.5f, 0));
    gage->setScaleX(1);
    gageBg->addChild(gage);
    
    floorProgressBar.gage = gage;
    
    // Label
    // 12/34 size:28 stroke:3 Vec2TL(220, -16) , Size(110, 29)
    floorProgressBar.label = Label::createWithTTF("", FONT_COMMODORE, 28, Size::ZERO,
                                                  TextHAlignment::CENTER, TextVAlignment::CENTER);
    floorProgressBar.label->setAnchorPoint(ANCHOR_M);
    floorProgressBar.label->setPosition(Vec2TL(bgSize, 220, -16));
    floorProgressBar.label->setTextColor(Color4B::WHITE);
    floorProgressBar.label->enableOutline(Color4B::BLACK, 3);
    layer->addChild(floorProgressBar.label);
}

/**
 * 스코어 UI 초기화
 */
void TopMenu::initScore() {
    
    const auto bgSize(getContentSize());
    
    // game_bg_score.png Vec2TC(116, -32) , Size(336, 64)
    auto scoreBg = Sprite::create(DIR_IMG_GAME + "game_bg_score.png");
    scoreBg->setAnchorPoint(ANCHOR_M);
    scoreBg->setPosition(Vec2TC(bgSize, 116, -32));
    addChild(scoreBg);
    
    // 1234 size:33 stroke:3 Vec2TC(116, -42) , Size(105, 33)
    auto scoreLabel = Label::createWithTTF("0", FONT_COMMODORE, 33, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setTag(Tag::SCORE);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2TC(bgSize, 116, -42));
    scoreLabel->setTextColor(Color4B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 3);
    addChild(scoreLabel);
}

/**
 * 메뉴 초기화
 */
void TopMenu::initMenu() {
    
    // game_btn_pause.png Vec2TR(-38, -38) , Size(68, 68)
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_PAUSE,      ANCHOR_M,   Vec2TR(getContentSize(), -38, -38),     "game_btn_pause.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        info.apply(btn);
        btn->setZoomScale(0.05f);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(TopMenu::onClick, this));
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
    listener->onFloorChanged         = CC_CALLBACK_1(TopMenu::onFloorChanged, this);
    listener->onNextFloor            = CC_CALLBACK_1(TopMenu::onNextFloor, this);
    listener->onScoreChanged         = CC_CALLBACK_1(TopMenu::onScoreChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

