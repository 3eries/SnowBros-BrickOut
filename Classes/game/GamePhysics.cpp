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
#include "object/tile/Item.hpp"

USING_NS_CC;
using namespace std;

static const string SCHEDULER_UPDATE                  = "SCHEDULER_UPDATE";

#define DEBUG_LOG               0

PhysicsManager::PhysicsManager() :
world(nullptr), debugDrawView(nullptr), map(nullptr),
onUpdateListener(nullptr),
onContactBrickListener(nullptr),
onContactItemListener(nullptr),
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
    loopBodies([=](b2Body *body)         { world->DestroyBody(body); });
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
void PhysicsManager::loopBodies(function<void(b2Body*)> callback) {
    
    if( !world ) {
        return;
    }
    
    // GetBodyList()를 이용한 루프 진행 중 바디가 제거되어,
    // dangling pointer가 발생할 수 있으므로 별도의 리스트 생성
    vector<b2Body*> bodies;
    
    for( auto b = world->GetBodyList(); b; b = b->GetNext() ) {
        bodies.push_back(b);
    }
    
    for( auto b : bodies ) {
        callback(b);
    }
}

void PhysicsManager::loopObjects(function<void(SBPhysicsObject*)> callback) {
    
    loopBodies([=](b2Body *b) {
        
        auto userData = b->GetUserData();
        
        if( userData ) {
            auto obj = (SBPhysicsObject*)userData;
            callback(obj);
        }
    });
}
 
/**
 * 충돌 여부 설정
 */
bool PhysicsManager::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !b2ContactFilter::ShouldCollide(fixtureA, fixtureB) ) {
        return false;
    }
    
    auto userDataA = fixtureA->GetBody()->GetUserData();
    auto userDataB = fixtureB->GetBody()->GetUserData();
    
    if( !userDataA || !userDataB ) {
        return false;
    }
    
    auto objA = (SBPhysicsObject*)userDataA;
    auto objB = (SBPhysicsObject*)userDataB;
    
    if( objA->isCollisionLocked() || objB->isCollisionLocked() ) {
        return false;
    }
    
    if( !fixtureA->GetBody()->IsAwake() ||
        !fixtureB->GetBody()->IsAwake() ) {
        CCLOG("PhysicsManager::ShouldCollide body is sleep.");
        MessageBox("PhysicsManager::ShouldCollide body is sleep.", "");
        // return false;
    }
    
    // 벽돌 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
            // 볼 비활성화
            if( !ball->isAwake() ) {
                return false;
            }
            
            // 벽돌이 깨짐
            if( brick->isBroken() ) {
                return false;
            }
        }
    }
    
    // 아이템 체크
    /*
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::ITEM, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
        }
    }
    */
    
    // 바닥 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            
            // 볼 비활성화
            if( !ball->isAwake() ) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * 충돌 시작
 */
void PhysicsManager::BeginContact(b2Contact *contact) {
    
#if DEBUG_LOG
    CCLOG("PhysicsManager::BeginContact");
#endif
    
    auto fixtureA = contact->GetFixtureA();
    auto fixtureB = contact->GetFixtureB();

    if( !fixtureA->GetBody()->IsAwake() ||
        !fixtureB->GetBody()->IsAwake() ) {
        CCLOG("PhysicsManager::BeginContact body is sleep.");
        MessageBox("PhysicsManager::BeginContact body is sleep.", "");
        // return;
    }
    
    
    // 벽돌 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
#if DEBUG_LOG
            CCLOG("\t> Brick hp: %d, ball awake: %d, brick awake: %d", brick->getHp(), ball->isAwake(), brick->isAwake());
#endif
            
            return;
        }
    }
    
    // 아이템 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::ITEM, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
#if DEBUG_LOG
            CCLOG("\t> Item ball awake: %d, item awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
            return;
        }
    }
    
    // 바닥 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
            if( objs.obj1->isAwake() ) {
                contact->SetEnabled(false);
                this->onContactFloorListener((Ball*)objs.obj1);
            } else {
                CCLOG("\t>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Floor ball is sleep.");
                MessageBox("Floor ball is sleep.", "");
            }
            
#if DEBUG_LOG
            CCLOG("\t> Floor ball awake: %d, floor awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
            return;
        }
    }
}

void PhysicsManager::EndContact(b2Contact *contact) {
    
#if DEBUG_LOG
    CCLOG("PhysicsManager::EndContact");
#endif
}

void PhysicsManager::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
#if DEBUG_LOG
    CCLOG("PhysicsManager::PreSolve contact enabled: %d", contact->IsEnabled());
#endif
    
    // 벽돌 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
            // 벽돌이 이미 깨진 경우, 충돌 비활성화
            if( brick->isBroken() ) {
                contact->SetEnabled(false);
            }
            
#if DEBUG_LOG
            CCLOG("\t> Brick hp: %d, ball awake: %d, brick awake: %d", brick->getHp(), ball->isAwake(), brick->isAwake());
#endif
            
            return;
        }
    }
    
    // 아이템 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::ITEM, contact);
        
        if( objs.obj1 && objs.obj2 ) {
#if DEBUG_LOG
            CCLOG("\t> Item ball awake: %d, item awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
        
            // 충돌 비활성화
            contact->SetEnabled(false);
            
            if( objs.obj1->isAwake() && objs.obj2->isAwake() ) {
                this->onContactItemListener((Ball*)objs.obj1, (Item*)objs.obj2);
            }
            
            return;
        }
    }
    
    // 바닥 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, contact);
        
        if( objs.obj1 && objs.obj2 ) {
#if DEBUG_LOG
            CCLOG("\t> Floor ball awake: %d, floor awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
            return;
        }
    }
}

void PhysicsManager::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
#if DEBUG_LOG
    CCLOG("PhysicsManager::PostSolve");
#endif
    
    // 벽돌 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
            if( ball->isAwake() && brick->isAwake() ) {
                this->onContactBrickListener(ball, brick);
            }
            
#if DEBUG_LOG
            CCLOG("\t> Brick hp: %d, ball awake: %d, brick awake: %d", brick->getHp(), ball->isAwake(), brick->isAwake());
#endif
            
            return;
        }
    }
    
    // 아이템 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::ITEM, contact);
        
        if( objs.obj1 && objs.obj2 ) {
#if DEBUG_LOG
            CCLOG("\t> Item ball awake: %d, item awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
            return;
        }
    }
    
    // 바닥 체크
    {
        auto objs = findObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, contact);
        
        if( objs.obj1 && objs.obj2 ) {
#if DEBUG_LOG
            CCLOG("\t> Floor ball awake: %d, floor awake: %d", objs.obj1->isAwake(), objs.obj2->isAwake());
#endif
            return;
        }
    }
}

PhysicsManager::CollisionObjects PhysicsManager::findObjects(uint16 categoryBits1, uint16 categoryBits2,
                                                             b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !fixtureA->GetBody()->GetUserData() ||
        !fixtureB->GetBody()->GetUserData() ) {
        return CollisionObjects();
    }
    
    /*
    if( !fixtureA->GetBody()->IsAwake() ||
        !fixtureB->GetBody()->IsAwake() ) {
        return;
    }
     */
    
    auto objA = (SBPhysicsObject*)fixtureA->GetBody()->GetUserData();
    auto objB = (SBPhysicsObject*)fixtureB->GetBody()->GetUserData();
    
    if( objA->isCollisionLocked() || objB->isCollisionLocked() ) {
        return CollisionObjects();
    }
    
    CollisionObjects objs;
    
    if( fixtureA->GetFilterData().categoryBits == categoryBits1 &&
        fixtureB->GetFilterData().categoryBits == categoryBits2 ) {
        objs.obj1 = objA;
        objs.obj2 = objB;
    }
    else if( fixtureB->GetFilterData().categoryBits == categoryBits1 &&
             fixtureA->GetFilterData().categoryBits == categoryBits2 ) {
        objs.obj1 = objB;
        objs.obj2 = objA;
    }
    
    return objs;
}

PhysicsManager::CollisionObjects PhysicsManager::findObjects(uint16 categoryBits1, uint16 categoryBits2,
                                                             b2Contact *contact) {
    
    return findObjects(categoryBits1, categoryBits2, contact->GetFixtureA(), contact->GetFixtureB());
}

