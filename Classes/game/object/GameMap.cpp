//
//  GameMap.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "GameMap.hpp"

#include "Define.h"
#include "../GameDefine.h"
#include "../GameManager.hpp"

USING_NS_CC;
using namespace std;

GameMap::GameMap() : SBPhysicsObject(this),
bg(nullptr) {
    
}

GameMap::~GameMap() {
    
}

bool GameMap::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    setSyncLocked(true);
    
    // 게임 리스너 초기화
    initGameListener();
    
    // 영역 확인용
    /*
    {
        auto n = LayerColor::create(Color4B(255, 0, 0, 255*0.2f));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(MAP_POSITION);
        n->setContentSize(MAP_CONTENT_SIZE);
        addChild(n);
    }
    */
     
    // 물리 객체 초기화
    b2BodyDef bodyDef;
    bodyDef.position = PTM(MAP_POSITION);
    bodyDef.userData = (SBPhysicsObject*)this;
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
    float left   = PTM(-MAP_CONTENT_SIZE.width*0.5f);
    float right  = PTM( MAP_CONTENT_SIZE.width*0.5f);
    float bottom = PTM(-MAP_CONTENT_SIZE.height*0.5f);
    float top    = PTM( MAP_CONTENT_SIZE.height*0.5f);
    
    b2Vec2 vectors[4][2] = {
        { b2Vec2(left, bottom), b2Vec2(left, top) },          // left
        { b2Vec2(right, bottom), b2Vec2(right, top) },        // right
        { b2Vec2(left, bottom), b2Vec2(right, bottom) },      // bottom
        { b2Vec2(left, top), b2Vec2(right, top) },            // top
    };
    
    PhysicsCategory categorys[] = {
        PhysicsCategory::WALL_LEFT,
        PhysicsCategory::WALL_RIGHT,
        PhysicsCategory::FLOOR,
        PhysicsCategory::WALL_TOP,
    };
    
    for( int i = 0; i < 4; ++i ) {
        b2Vec2 v1 = vectors[i][0];
        b2Vec2 v2 = vectors[i][1];
        
        b2EdgeShape shape;
        shape.Set(v1, v2);
        
        b2Filter filter;
        filter.categoryBits = categorys[i];
        filter.maskBits = PHYSICS_MASK_BITS_WALL;
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 0.1f;      // 밀도
        fixtureDef.restitution = 1;     // 반발력 - 물체가 다른 물체에 닿았을때 팅기는 값
        fixtureDef.friction = 0;        // 마찰력
        fixtureDef.filter = filter;
        body->CreateFixture(&fixtureDef);
    }
    
    return true;
}

/**
 * 배경 업데이트
 */
void GameMap::updateBackground(const StageData &stage) {
    
    if( bg ) {
        bg->removeFromParent();
    }
    
    bg = createBackground(stage);
    addChild(bg);
}

/**
 * 게임 진입
 */
void GameMap::onGameEnter() {
    
    updateBackground(Database::getFirstWorld().getFirstStage());
}

/**
 * 부스트 시작
 */
void GameMap::onBoostStart() {
}

/**
 * 부스트 진행
 */
void GameMap::onBoosting() {
    
    auto boostStage = GAME_MANAGER->getBoostStage();
    int stageDiff = boostStage.stageSeq-1;
    bool isBoostMultiStage = (stageDiff >= 2);
    
    const float MOVE_DURATION = 0.8f;
    
    SBDirector::postDelayed(this, [=]() {
        
        GameManager::onBoostEnd();
        
    }, isBoostMultiStage ? MOVE_DURATION*2 : MOVE_DURATION);
    
    // 이동 연출
    auto getMoveAction = [=]() -> MoveBy* {
        return MoveBy::create(MOVE_DURATION, Vec2(0, -SB_WIN_SIZE.height-bg->top->getContentSize().height));
    };
    
    // Step 1. 원래 배경 퇴장 & 다음 배경 등장
    bg->runAction(getMoveAction());
    
    auto nextStage = isBoostMultiStage ? Database::getStageBySeq(GAME_MANAGER->getStage().stageSeq+1) : boostStage;
    
    auto nextStageBg = createBackground(nextStage);
    nextStageBg->setAnchorPoint(ANCHOR_MB);
    nextStageBg->setPosition(Vec2TC(0, bg->top->getContentSize().height));
    addChild(nextStageBg);
    
    auto callFunc = CallFunc::create([=]() {
        
        // Step 2. 부스트된 스테이지가 여러개면 Step1을 한번 더 진행
        if( isBoostMultiStage ) {
            nextStageBg->runAction(Sequence::create(getMoveAction(), RemoveSelf::create(), nullptr));
            
            auto boostStageBg = createBackground(boostStage);
            boostStageBg->setAnchorPoint(ANCHOR_MB);
            boostStageBg->setPosition(Vec2TC(0, bg->top->getContentSize().height));
            this->addChild(boostStageBg);
    
            boostStageBg->runAction(Sequence::create(getMoveAction(), RemoveSelf::create(), nullptr));
            
        } else {
            nextStageBg->removeFromParent();
        }
    });
    nextStageBg->runAction(Sequence::create(getMoveAction(), callFunc, nullptr));
}

/**
 * 부스트 종료
 */
void GameMap::onBoostEnd() {
    
    updateBackground(GAME_MANAGER->getBoostStage());
}

/**
 * 스테이지 변경
 */
void GameMap::onStageChanged(const StageData &stage) {
    
    updateBackground(stage);
}

/**
 * 다음 스테이지로 이동
 */
void GameMap::onMoveNextStage(const StageData &stage) {
    
    auto nextStageBg = createBackground(stage);
    nextStageBg->setAnchorPoint(ANCHOR_MB);
    nextStageBg->setPosition(Vec2TC(0, bg->top->getContentSize().height));
    addChild(nextStageBg);
    
    auto move = MoveBy::create(MOVE_NEXT_STAGE_DURATION,
                               Vec2(0, -SB_WIN_SIZE.height-bg->top->getContentSize().height));
    auto callFunc = CallFunc::create([=]() {
        
        GameManager::onMoveNextStageFinished();
    });
    bg->runAction(Sequence::create(move, callFunc, nullptr));
    
    auto remove = RemoveSelf::create();
    nextStageBg->runAction(Sequence::create(move->clone(), remove, nullptr));
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameMap::onMoveNextStageFinished(const StageData &stage) {
}

GameMap::Background* GameMap::createBackground(const StageData &stage) {
    
    Background *bg = Background::create();
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    bg->setContentSize(SB_WIN_SIZE);
    
    auto top = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
    top->setAnchorPoint(ANCHOR_MB);
    top->setPosition(Vec2TC(0,0));
    top->setFlippedY(true);
    bg->addChild(top);
    
    auto center = Sprite::create(ContentResourceHelper::getStageBackgroundFile(stage.originStage));
    center->setAnchorPoint(ANCHOR_M);
    center->setPosition(Vec2MC(0,0));
    center->setScale(4);
    bg->addChild(center);
    
    auto bottom = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
    bottom->setAnchorPoint(ANCHOR_MB);
    bottom->setPosition(Vec2BC(0,0));
    bg->addChild(bottom);
    
    bg->top = top;
    bg->center = center;
    bg->bottom = bottom;
    
    return bg;
}

/**
 * 게임 리스너 초기화
 */
void GameMap::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter               = CC_CALLBACK_0(GameMap::onGameEnter, this);
    listener->onBoostStart              = CC_CALLBACK_0(GameMap::onBoostStart, this);
    listener->onBoosting                = CC_CALLBACK_0(GameMap::onBoosting, this);
    listener->onBoostEnd                = CC_CALLBACK_0(GameMap::onBoostEnd, this);
    listener->onStageChanged            = CC_CALLBACK_1(GameMap::onStageChanged, this);
    listener->onMoveNextStage           = CC_CALLBACK_1(GameMap::onMoveNextStage, this);
    listener->onMoveNextStageFinished   = CC_CALLBACK_1(GameMap::onMoveNextStageFinished, this);
    GameManager::getEventDispatcher()->addListener(listener);
}
