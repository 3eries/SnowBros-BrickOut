//
//  GameView.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include <spine/spine-cocos2dx.h>

#include "SceneManager.h"
#include "UserDefaultKey.h"
#include "User.hpp"
#include "UIHelper.hpp"
#include "TestHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

GameView::GameView() {
}

GameView::~GameView() {
 
    removeListeners(this);
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    
    if( SceneManager::isGameScene() ) {
        initHero();
        initGameListener();
    }
    
    initIAPListener();
    
    return true;
}

void GameView::onEnter() {
    
    Node::onEnter();
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    auto cloud = getChildByTag<SkeletonAnimation*>(Tag::CLOUD);
    cloud->runAction(FadeIn::create(1.5f));
}

void GameView::onExit() {
    
    Node::onExit();
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 게임 진입
 */
void GameView::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void GameView::onGameExit() {
}

/**
 * 게임 리셋
 */
void GameView::onGameReset() {
    
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    SBNodeUtils::recursivePause(this);
}

/**
 * 게임 재게
 */
void GameView::onGameResume() {
    
    SBNodeUtils::recursiveResume(this);
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
}

/**
 * 게임 이어하기
 */
void GameView::onGameContinue() {
}

/**
 * 게임 결과
 */
void GameView::onGameResult() {
}

/**
 * 부스트 시작
 */
void GameView::onBoostStart() {
}

/**
 * 부스트 종료
 */
void GameView::onBoostEnd() {
}

/**
 * 스코어 변경
 */
void GameView::onScoreChanged(int score) {
}

/**
 * 레벨 변경
 */
void GameView::onLevelChanged(const LevelInfo &level) {
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
    
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_bg.png");
    bg->setTag(Tag::BG);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg, (int)ZOrder::BG);
    
    auto cloud = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD);
    cloud->setTag(Tag::CLOUD);
    cloud->setAnchorPoint(Vec2::ZERO);
    cloud->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    cloud->setAnimation(0, ANIM_NAME_RUN, true);
    cloud->update(0);
    cloud->setOpacity(0);
    addChild(cloud, (int)ZOrder::CLOUD);
    
    auto bottomBg = Sprite::create(DIR_IMG_GAME + "RSP_bg_bottom.png");
    bottomBg->setAnchorPoint(ANCHOR_MB);
    bottomBg->setPosition(Vec2BC(0,0));
    addChild(bottomBg, (int)ZOrder::CLOUD);
}

/**
 * 히어로 초기화
 */
void GameView::initHero() {
    
    vector<string> files({
        DIR_IMG_GAME + "hero_idle_01.png",
        DIR_IMG_GAME + "hero_idle_02.png"
    });
    
    auto hero = SBAnimationSprite::create(files, 0.5f);
    hero->setAnchorPoint(ANCHOR_M);
    hero->setPosition(Vec2BC(-194, 345));
    addChild(hero);
    
    hero->runAnimation();
}

/**
 * 게임 리스너 초기화
 */
void GameView::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter            = CC_CALLBACK_0(GameView::onGameEnter, this);
    listener->onGameExit             = CC_CALLBACK_0(GameView::onGameExit, this);
    listener->onGameReset            = CC_CALLBACK_0(GameView::onGameReset, this);
    listener->onGameStart            = CC_CALLBACK_0(GameView::onGameStart, this);
    listener->onGameRestart          = CC_CALLBACK_0(GameView::onGameRestart, this);
    listener->onGamePause            = CC_CALLBACK_0(GameView::onGamePause, this);
    listener->onGameResume           = CC_CALLBACK_0(GameView::onGameResume, this);
    listener->onGameOver             = CC_CALLBACK_0(GameView::onGameOver, this);
    listener->onGameContinue         = CC_CALLBACK_0(GameView::onGameContinue, this);
    listener->onGameResult           = CC_CALLBACK_0(GameView::onGameResult, this);
    listener->onBoostStart           = CC_CALLBACK_0(GameView::onBoostStart, this);
    listener->onBoostEnd             = CC_CALLBACK_0(GameView::onBoostEnd, this);
    listener->onScoreChanged         = CC_CALLBACK_1(GameView::onScoreChanged, this);
    listener->onLevelChanged         = CC_CALLBACK_1(GameView::onLevelChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

/**
 * IAP 리스너 초기화
 */
void GameView::initIAPListener() {
    
    auto onRemoveAds = [=]() {
        // TODO:
    };
    
    // purchase listener
    auto purchaseListener = iap::PurchaseListener::create();
    purchaseListener->setForever(true);
    purchaseListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
    
    // restore listener
    auto restoreListener = iap::RestoreListener::create();
    restoreListener->setForever(true);
    restoreListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, restoreListener);
}
