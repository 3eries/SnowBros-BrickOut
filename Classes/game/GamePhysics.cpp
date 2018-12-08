//
//  GamePhysics.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "GamePhysics.hpp"

#include "GameDefine.h"

#include "object/Ball.hpp"
#include "object/tile/Brick.hpp"

USING_NS_CC;
using namespace std;

static const string SCHEDULER_UPDATE                  = "SCHEDULER_UPDATE";

PhysicsManager::PhysicsManager() :
world(nullptr), debugDrawView(nullptr), map(nullptr),
onUpdateListener(nullptr),
onContactBrickListener(nullptr),
onContactFloorListener(nullptr) {
    
}

PhysicsManager::~PhysicsManager() {
    
    CC_SAFE_DELETE(world);
}

b2World* PhysicsManager::initWorld() {
    
    world = new b2World(b2Vec2(0, 0));
    world->SetContactFilter(this);
    world->SetContactListener(this);
    
    return world;
}

/**
 * 모든 바디 제거
 */
void PhysicsManager::removeBodies() {
    loopObjects([](SBPhysicsObject *obj) { obj->removeBody(); });
}

/**
 * 스케줄러 시작
 */
void PhysicsManager::startScheduler() {
    SBScheduleHelper::schedule(CC_CALLBACK_1(PhysicsManager::update, this), this, PHYSICS_FPS, SCHEDULER_UPDATE);
}

/**
 * 스케줄러 정지
 */
void PhysicsManager::stopScheduler() {
    SBScheduleHelper::unschedule(this, SCHEDULER_UPDATE);
}

/**
 * 스케줄러 일시정지
 */
void PhysicsManager::pauseScheduler() {
    SBScheduleHelper::pause(this);
}

/**
 * 스케줄러 재게
 */
void PhysicsManager::resumeScheduler() {
    SBScheduleHelper::resume(this);
}

/**
 * 물리 세계 업데이트
 */
void PhysicsManager::update(float dt) {
    
    loopObjects([](SBPhysicsObject *obj) { obj->beforeStep(); });
    world->Step(PHYSICS_FPS, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    loopObjects([](SBPhysicsObject *obj) { obj->afterStep(); });
    
    onUpdateListener();
}

/**
 * 물리 오브젝트 루프
 */
void PhysicsManager::loopObjects(function<void(SBPhysicsObject*)> callback) {
    
    if( !world ) {
        return;
    }
    
    // GetBodyList()를 이용한 루프 진행 중 바디가 제거되어,
    // dangling pointer가 발생할 수 있으므로 별도의 리스트 생성
    vector<SBPhysicsObject*> objs;
    
    for( auto b = world->GetBodyList(); b; b = b->GetNext() ) {
        auto userData = b->GetUserData();
        
        if( userData ) {
            auto obj = (SBPhysicsObject*)userData;
            objs.push_back(obj);
        }
    }
    
    for( auto obj : objs ) {
        callback(obj);
    }
}

/**
 * 충돌 여부 설정
 */
bool PhysicsManager::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !b2ContactFilter::ShouldCollide(fixtureA, fixtureB) ) {
        return false;
    }
    
    return true;
}

/**
 * 충돌 시작
 */
void PhysicsManager::BeginContact(b2Contact *contact) {
    
    CCLOG("BeginContact");
    
    auto fixtureA = contact->GetFixtureA();
    auto fixtureB = contact->GetFixtureB();

    auto userDataA = fixtureA->GetBody()->GetUserData();
    auto userDataB = fixtureB->GetBody()->GetUserData();

    if( !userDataA || !userDataB ) {
        return;
    }

    auto objA = (SBPhysicsObject*)userDataA;
    auto objB = (SBPhysicsObject*)userDataB;

    auto contactBrick = [=](SBPhysicsObject *ballObj, SBPhysicsObject *brickObj) {

        auto ball = (Ball*)ballObj;
        auto brick = (Brick*)brickObj;

        this->onContactBrickListener(ball, brick);

        // 벽돌이 깨진 경우, 충돌 객체 비활성화
        if( brick->isBroken() ) {
            // contact->SetEnabled(false);
        }
    };

    auto contactFloor = [=](SBPhysicsObject *ballObj) {

        auto ball = (Ball*)ballObj;
        this->onContactFloorListener(ball);

        contact->SetEnabled(false);
    };

    // Check Brick
    if( fixtureA->GetFilterData().categoryBits == PhysicsCategory::BALL &&
        fixtureB->GetFilterData().categoryBits == PhysicsCategory::BRICK ) {
        contactBrick(objA, objB);
    }
    else if( fixtureB->GetFilterData().categoryBits == PhysicsCategory::BALL &&
             fixtureA->GetFilterData().categoryBits == PhysicsCategory::BRICK ) {
        contactBrick(objB, objA);
    }
    // Check Floor
    else if( fixtureA->GetFilterData().categoryBits == PhysicsCategory::BALL &&
             fixtureB->GetFilterData().categoryBits == PhysicsCategory::FLOOR ) {
        contactFloor(objA);
    }
    else if( fixtureB->GetFilterData().categoryBits == PhysicsCategory::BALL &&
             fixtureA->GetFilterData().categoryBits == PhysicsCategory::FLOOR ) {
        contactFloor(objB);
    }
}

void PhysicsManager::EndContact(b2Contact *contact) {
    
    // CCLOG("EndContact");
}

void PhysicsManager::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    auto fixtureA = contact->GetFixtureA();
    auto fixtureB = contact->GetFixtureB();
    
    if( !fixtureA || !fixtureB ) {
        MessageBox("fixture is null", "");
        contact->SetEnabled(false);
    }
    
    // 벽과의 충돌은 무시한다
    /*
     if( fixtureA->GetFilterData().categoryBits == PhysicsCategory::WALL ||
     fixtureB->GetFilterData().categoryBits == PhysicsCategory::WALL ) {
     contact->SetEnabled(false);
     }
     */
}

void PhysicsManager::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
}
