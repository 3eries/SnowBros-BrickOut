//
//  GamePhysics.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#ifndef GamePhysics_hpp
#define GamePhysics_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Box2D/Box2D.h"

#include "object/GameMap.hpp"

class Ball;
class Brick;
class Item;

// velocityIterations : 바디들을 정상적으로 이동시키기 위해서 필요한 충돌들을 반복적으로 계산
// positionIterations : 조인트 분리와, 겹침현상을 줄이기 위해서 바디의 위치를 반복적으로 적용
// 값이 클수록 정확한 연산이 가능하지만 성능이 떨어진다.

// 매뉴얼상의 권장값 (VELOCITY_ITERATIONS=8, POSITION_ITERATIONS=3)
static const int VELOCITY_ITERATIONS            = 8;
static const int POSITION_ITERATIONS            = 3;

/** @class PhysicsManager
 * @brief 이 클래스는 물리 세계의 오브젝트 동기화, 충돌 검사를 담당합니다.
 */
class PhysicsManager : public cocos2d::Ref, public b2ContactFilter, public b2ContactListener {
public:
    PhysicsManager();
    ~PhysicsManager();
    
    b2World* initWorld();
    
    void removeBodies();
    
public:
    void startScheduler();
    void stopScheduler();
    void pauseScheduler();
    void resumeScheduler();
    
    void update(float dt);
    
    void loopBodies(std::function<void(b2Body*)> callback);
    void loopObjects(std::function<void(SBPhysicsObject*)> callback);
    
private:
    // b2ContactFilter
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;
    
    // b2ContactListener
    void BeginContact(b2Contact *contact) override;
    void EndContact(b2Contact *contact) override;
    void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
    struct CollisionObjects {
        SBPhysicsObject *obj1;
        SBPhysicsObject *obj2;
        
        CollisionObjects() : obj1(nullptr), obj2(nullptr) {}
    };

    CollisionObjects findObjects(uint16 categoryBits1, uint16 categoryBits2,
                                 b2Fixture *fixtureA, b2Fixture *fixtureB);
    CollisionObjects findObjects(uint16 categoryBits1, uint16 categoryBits2,
                                 b2Contact *contact);
private:
    /*
    template <class T1, class T2>
    static void findObjects(std::function<void(T1,T2)> callback, b2Contact *contact) {
        
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        
        if( !fixtureA->GetBody()->GetUserData() ||
            !fixtureB->GetBody()->GetUserData() ) {
            return;
        }
        
        if( !fixtureA->GetBody()->IsAwake() ||
            !fixtureB->GetBody()->IsAwake() ) {
            return;
        }
        
        auto userDataA = (SBPhysicsObject*)fixtureA->GetBody()->GetUserData();
        auto userDataB = (SBPhysicsObject*)fixtureB->GetBody()->GetUserData();
        
        T1 obj1 = dynamic_cast<T1>(userDataA);
        T2 obj2 = dynamic_cast<T2>(userDataA);
        
        if( !obj1 ) obj1 = dynamic_cast<T1>(userDataB);
        if( !obj2 ) obj2 = dynamic_cast<T2>(userDataB);
        
        if( obj1 && obj2 ) {
            callback(obj1, obj2);
        }
    }*/
//    auto contactItem = [=](SBPhysicsObject *ballObj, SBPhysicsObject *itemObj) {
//
//        auto ball = (Ball*)ballObj;
//        auto item = (Item*)itemObj;
//
//
//    };
    
private:
    CC_SYNTHESIZE_READONLY(b2World*, world, World);
    CC_SYNTHESIZE(DebugDrawView*, debugDrawView, DebugDrawView);
    CC_SYNTHESIZE(GameMap*, map, Map);
    
    CC_SYNTHESIZE(std::function<void()>,
                  onUpdateListener, OnUpdateListener);
    CC_SYNTHESIZE(std::function<void(Ball*,Brick*)>,
                  onContactBrickListener, OnContactBrickListener);
    CC_SYNTHESIZE(std::function<void(Ball*,Item*)>,
                  onContactItemListener, OnContactItemListener);
    CC_SYNTHESIZE(std::function<void(Ball*)>,
                  onContactFloorListener, OnContactFloorListener);
};

#endif /* GamePhysics_hpp */
