//
//  DemoView.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/03/2019.
//

#include "DemoView.hpp"

#include "Define.h"
#include "ContentManager.hpp"
#include "GameConfiguration.hpp"
#include "../../game/GameDefine.h"

#include "DemoMap.hpp"
#include "DemoBrick.hpp"
#include "DemoBall.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static const string SCHEDULER_SHOOT                 = "SCHEDULER_SHOOT";

#define DEBUG_DRAW_PHYSICS      0

DemoView::DemoView() :
world(nullptr) {
}

DemoView::~DemoView() {
    
    CC_SAFE_DELETE(world);
}

bool DemoView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    // 스테이지 랜덤 추출
    auto stages = Database::getDemoStages();
    stage = stages[arc4random() % stages.size()];
    
    initPhysics();
    initMap();
    initBrick();
    initBall();
    
    return true;
}

void DemoView::onEnter() {
    
    Node::onEnter();
}

void DemoView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    scheduleUpdate();
    
    // 볼 발사!
    SBDirector::postDelayed(this, [=]() {
        this->shoot();
    }, 0.5f);
}

void DemoView::onExit() {
    
    SBPhysics::loopObjects(world, [=](SBPhysicsObject *obj) {
        obj->removeBody();
    });
    SBPhysics::removeBodies(world);
    
    Node::onExit();
}

/**
 * 볼 발사
 */
void DemoView::shoot() {
    
    shootIndex = 0;
    fallenBallCount = 0;
    
    // 발사 각도 설정
    float shootingAngle = ((arc4random() % 6) + 2) * 10; // 20 ~ 70
    shootingAngle *= (arc4random() % 2 == 0 ? 1 : -1);
    shootingAngle = MIN(SHOOTING_MAX_ANGLE, shootingAngle);
    shootingAngle = MAX(-SHOOTING_MAX_ANGLE, shootingAngle);
    
    // 속도 설정
    // b2Vec2 velocity(random(-30, 30), BALL_MAX_VELOCITY);
    // b2Vec2 velocity(-30, BALL_MAX_VELOCITY);
    Vec2 endPosition = shootingPosition;
    endPosition += (Vec2::forAngle(-CC_DEGREES_TO_RADIANS(shootingAngle-90)) * MAP_DIAGONAL);
    
    // Log::i("shootingAngle: %f, endPosition: %f,%f", shootingAngle, endPosition.x, endPosition.y);
    
    Vec2 diff = endPosition - shootingPosition;
    
    b2Vec2 velocity = PTM(diff);
    velocity.Normalize();
    velocity.x *= BALL_MAX_VELOCITY;
    velocity.y *= BALL_MAX_VELOCITY;
    
    // Log::i("shoot velocity: %f,%f", velocity.x, velocity.y);
    
    // 볼 업데이트
    for( auto ball : balls ) {
        // 발사 직후 바닥 충돌 방지를 위해 좌표 보정
        ball->setSyncLocked(true);
        ball->setBodyPosition(ball->getPosition() + Vec2(0,4));
        
        // 상태 변경
        ball->setBodyAwake(true);
        ball->setVisible(false);
    }
    
    balls[0]->setVisible(true);
    
    // 하나씩 발사
    schedule([=](float dt) {
        
        auto ball = balls[shootIndex];
        
        // 발사
        ball->setSyncLocked(false);
        ball->shoot(velocity);
        
        ++shootIndex;
        
        // 슈팅 진행중
        if( shootIndex < balls.size() ) {
            ballCountLabel->setString("X" + TO_STRING(balls.size() - shootIndex));
            // 다음볼 show
            balls[shootIndex]->setVisible(true);
        }
        // 슈팅 완료
        else {
            ballCountLabel->setVisible(false);
            this->shootStop();
        }
    }, SHOOTING_INTERVAL, SCHEDULER_SHOOT);
}

void DemoView::shootStop() {
    
    unschedule(SCHEDULER_SHOOT);
}

void DemoView::onFallFinished() {
    
    ballCountLabel->setVisible(false);
    // ballCountLabel->setVisible(true);
    ballCountLabel->setPosition(Vec2(shootingPosition.x, ballCountLabel->getPositionY()));
    ballCountLabel->setString("X" + TO_STRING(balls.size()));
    
    SBDirector::postDelayed(this, [=]() {
        this->shoot();
    }, 1.5f);
}

/**
 * 볼 & 바닥 충돌
 */
void DemoView::onContactFloor(DemoBall *ball) {
    
    ++fallenBallCount;
//    Log::i("onContactFloor fallenBallCount: %d", fallenBallCount);
    
    // 첫번째 볼 추락
    const bool isFallenFirstBall = (fallenBallCount == 1);
    
    if( isFallenFirstBall ) {
        shootingPosition = Vec2(ball->getPosition().x, SHOOTING_POSITION_Y);
    }
    
    // 시작 위치로 이동
    auto pos = shootingPosition;
    ball->setPosition(Vec2(ball->getPositionX(), pos.y));
    
    auto move = MoveTo::create(BALL_JOIN_MOVE_DURATION, pos);
    auto callFunc = CallFunc::create([=]() {
        
        // 두번째 볼부터 hide
        if( !isFallenFirstBall ) {
            ball->setVisible(false);
        }
    });
    ball->runAction(Sequence::create(move, callFunc, nullptr));
    
    // 모든 볼 추락
    if( fallenBallCount == balls.size() ) {
        SBDirector::postDelayed(this, CC_CALLBACK_0(DemoView::onFallFinished, this), BALL_JOIN_MOVE_DURATION*0.5f);
    }
}

void DemoView::update(float dt) {
    
    world->Step(PHYSICS_FPS, 8, 3);
    
    for( auto ball : balls ) {
        ball->afterStep();
    }
}

/**
 * 충돌 여부 설정
 */
bool DemoView::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !b2ContactFilter::ShouldCollide(fixtureA, fixtureB) ) {
        return false;
    }
    
    return true;
}

/**
 * 충돌 시작
 */
void DemoView::BeginContact(b2Contact *contact) {
    
    // 바닥 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            contact->SetEnabled(false);
            
            auto ball = (DemoBall*)objs.obj1;
            ball->onContactFloor();
            onContactFloor(ball);
            
            return;
        }
    }
    
    // 벽 체크
    {
        PhysicsCategory wallCategorys[] = {
            PhysicsCategory::WALL_LEFT,
            PhysicsCategory::WALL_RIGHT,
            PhysicsCategory::WALL_TOP,
        };
        
        for( auto wallCategory : wallCategorys ) {
            auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, wallCategory, contact);
            
            if( objs.obj1 && objs.obj2 ) {
                auto ball = (DemoBall*)objs.obj1;
                ball->onContactWall();
            }
        }
    }
}

/**
 * 충돌 종료
 */
void DemoView::EndContact(b2Contact *contact) {
}

/**
 * 두 형태가 접촉을 계속하는 동안 호출됩니다.
 */
void DemoView::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
}

/**
 * 두 형태가 접촉 후, 충돌을 통한 물리 계산을 이미 처리했을 때 호출됩니다.
 */
void DemoView::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    // 벽돌 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (DemoBall*)objs.obj1;
            ball->onContactBrick(SBPhysics::getContactPoint(contact));
            
            auto brick = (DemoBrick*)objs.obj2;
            brick->onContactBrick();
        }
    }
}

/**
 * 물리 세계 초기화
 */
void DemoView::initPhysics() {
    
    world = new b2World(b2Vec2(0, 0));
    world->SetContactFilter(this);
    world->SetContactListener(this);
    
    // DebugDraw
#if DEBUG_DRAW_PHYSICS
    {
        auto view = DebugDrawView::create(world);
        addChild(view, SBZOrder::MIDDLE);
        
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit;
        
        view->getDebugDraw()->SetFlags(flags);
    }
#endif
}

/**
 * 맵 초기화
 */
void DemoView::initMap() {
    
    addChild(DemoMap::create(stage.stage, world));
}

/**
 * 벽돌 초기화
 */
void DemoView::initBrick() {
    
    for( auto brickData : stage.bricks ) {
        auto brick = DemoBrick::create(brickData, world);
        addChild(brick);
    }
}

/**
 * 볼 초기화
 */
void DemoView::initBall() {
    
    shootingPosition = FIRST_SHOOTING_POSITION;
    
    for( int i = 0; i < GAME_CONFIG->getFirstBallCount(); ++i ) {
        auto ball = DemoBall::create(world);
        ball->setPosition(shootingPosition);
        ball->setBodyAwake(false);
        ball->setCollisionLocked(true);
        addChild(ball);
        
        balls.push_back(ball);
    }
    
    ballCountLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    ballCountLabel->setVisible(false);
    ballCountLabel->setAnchorPoint(ANCHOR_M);
    ballCountLabel->setPosition(Vec2BC(0, 25));
    ballCountLabel->setTextColor(Color4B::WHITE);
    ballCountLabel->enableOutline(Color4B::BLACK, 3);
    addChild(ballCountLabel);
}
