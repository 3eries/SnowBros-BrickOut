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

void TopMenu::onEnter() {
    
    updateStageUI(GameManager::getStage());
    updateFloorProgressUI(GameManager::getFloor());
    
    Node::onEnter();
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

    auto worldLabel = getChildByTag<Label*>(Tag::WORLD);
    
    if( worldLabel ) {
        worldLabel->setString(STR_FORMAT("W %d", stage.world));
    }
    
    auto stageLabel = getChildByTag<Label*>(Tag::STAGE);
    
    if( stageLabel ) {
        string stageStr = (/*isBoss()*/false) ? "B" : TO_STRING(stage.stage);
        stageLabel->setString(stageStr);
    }
}

/**
 * Floor 진행바 UI 업데이트
 */
void TopMenu::updateFloorProgressUI(const FloorData &floor) {
    
    auto stage = GameManager::getStage();
    
    if( floor.isNull() ) {
        // floorProgressBar.layer->setVisible(false);
        return;
    }
    
    int cnt = floor.floor - stage.getFirstFloor().floor + 1;
    int end = (int)stage.floors.size();
    
    // floorProgressBar.layer->setVisible(true);
    floorProgressBar.gage->setScaleX((float)cnt / end);
    floorProgressBar.label->setString(STR_FORMAT("%d/%d", cnt, end));
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
 * 스테이지 변경
 */
void TopMenu::onStageChanged(const StageData &stage) {
    
    updateStageUI(stage);
}

/**
 * 스테이지 클리어
 */
void TopMenu::onStageClear() {
}

/**
 * 다음 스테이지로 이동
 */
void TopMenu::onMoveNextStage(const StageData &stage) {
}

/**
 * 다음 스테이지로 이동 완료
 */
void TopMenu::onMoveNextStageFinished(const StageData &stage) {
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
    
    /*
    auto scoreLabel = getChildByTag<Label*>(Tag::SCORE);
    
    if( scoreLabel ) {
        scoreLabel->setString(TO_STRING(score));
    }
    */
}

/**
 * 배경 UI 초기화
 */
void TopMenu::initBg() {
    
    auto topBg = Sprite::create(DIR_IMG_GAME + "game_bg_top.png");
    topBg->setAnchorPoint(ANCHOR_M);
    topBg->setPosition(Vec2MC(topBg->getContentSize(), 0, 0));
    // topBg->setOpacity(255*0.4f);
    addChild(topBg);
    
    Size bgSize(topBg->getContentSize());
    setContentSize(bgSize);
}

/**
 * 스테이지 UI 초기화
 */
void TopMenu::initStage() {
    
    const auto bgSize(getContentSize());
    
    // 스테이지 배경
    auto stageBg = Sprite::create(DIR_IMG_GAME + "game_bg_stage.png");
    stageBg->setAnchorPoint(ANCHOR_M);
    stageBg->setPosition(Vec2TL(bgSize, 88, -38));
    addChild(stageBg, 1);
    
    // 월드 번호
    auto worldLabel = Label::createWithBMFont(FONT_WORLD_SMALL, "W 1", TextHAlignment::CENTER);
    worldLabel->setTag(Tag::WORLD);
    worldLabel->setAnchorPoint(ANCHOR_M);
    worldLabel->setPosition(Vec2TL(bgSize, 88, -14));
    addChild(worldLabel, 1);
    
    // 스테이지 번호
    auto stageLabel = Label::createWithBMFont(FONT_WORLD_MEDIUM, "1", TextHAlignment::CENTER);
    stageLabel->setTag(Tag::STAGE);
    stageLabel->setAnchorPoint(ANCHOR_M);
    stageLabel->setPosition(Vec2TL(bgSize, 88, -44));
    addChild(stageLabel, 1);
    
    // Floor 진행바
    auto layer = SBNodeUtils::createZeroSizeNode();
    addChild(layer);
    
    floorProgressBar.layer = layer;
    
    // Gage
    auto gageBg = Sprite::create(DIR_IMG_GAME + "game_gage_stage_bg.png");
    gageBg->setAnchorPoint(ANCHOR_M);
    gageBg->setPosition(Vec2TC(bgSize, 20, -38));
    layer->addChild(gageBg);
    
    auto gageBgSize = gageBg->getContentSize();
    
    auto gage = Sprite::create(DIR_IMG_GAME + "game_gage_stage.png");
    gage->setAnchorPoint(ANCHOR_ML);
    gage->setPosition(Vec2ML(gageBgSize, (gageBgSize.width - gage->getContentSize().width)*0.5f, 0));
    gage->setScaleX(1);
    gageBg->addChild(gage);
    
    floorProgressBar.gage = gage;
    
    auto gageBossIcon = Sprite::create(DIR_IMG_GAME + "game_icon_boss.png");
    gageBossIcon->setAnchorPoint(ANCHOR_M);
    gageBossIcon->setPosition(Vec2TC(bgSize, 246, -38));
    layer->addChild(gageBossIcon);
    
    // Label
    floorProgressBar.label = Label::createWithTTF("", FONT_COMMODORE, 30, Size::ZERO,
                                                  TextHAlignment::CENTER, TextVAlignment::CENTER);
    floorProgressBar.label->setAnchorPoint(ANCHOR_M);
    floorProgressBar.label->setPosition(Vec2TC(bgSize, 19, -14));
    floorProgressBar.label->setTextColor(Color4B::WHITE);
    floorProgressBar.label->enableOutline(Color4B::BLACK, 3);
    layer->addChild(floorProgressBar.label);
}

/**
 * 스코어 UI 초기화
 */
void TopMenu::initScore() {
    
    /*
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
    */
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
    listener->onGameEnter               = CC_CALLBACK_0(TopMenu::onGameEnter, this);
    listener->onGameExit                = CC_CALLBACK_0(TopMenu::onGameExit, this);
    listener->onGameReset               = CC_CALLBACK_0(TopMenu::onGameReset, this);
    listener->onGameStart               = CC_CALLBACK_0(TopMenu::onGameStart, this);
    listener->onGameRestart             = CC_CALLBACK_0(TopMenu::onGameRestart, this);
    listener->onGameOver                = CC_CALLBACK_0(TopMenu::onGameOver, this);
    listener->onGameContinue            = CC_CALLBACK_0(TopMenu::onGameContinue, this);
    listener->onGameResult              = CC_CALLBACK_0(TopMenu::onGameResult, this);
    listener->onBoostStart              = CC_CALLBACK_0(TopMenu::onBoostStart, this);
    listener->onBoostEnd                = CC_CALLBACK_0(TopMenu::onBoostEnd, this);
    listener->onStageChanged            = CC_CALLBACK_1(TopMenu::onStageChanged, this);
    listener->onStageClear              = CC_CALLBACK_0(TopMenu::onStageClear, this);
    listener->onMoveNextStage           = CC_CALLBACK_1(TopMenu::onMoveNextStage, this);
    listener->onMoveNextStageFinished   = CC_CALLBACK_1(TopMenu::onMoveNextStageFinished, this);
    listener->onFloorChanged            = CC_CALLBACK_1(TopMenu::onFloorChanged, this);
    listener->onNextFloor               = CC_CALLBACK_1(TopMenu::onNextFloor, this);
    listener->onScoreChanged            = CC_CALLBACK_1(TopMenu::onScoreChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

