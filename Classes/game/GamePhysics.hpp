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

#include "GameDefine.h"
#include "object/GameMap.hpp"

class Ball;

namespace Game {
class Tile;
}

class GamePhysicsListener : public cocos2d::Ref {
public:
    SB_REF_CREATE_FUNC(GamePhysicsListener);
    ~GamePhysicsListener() {}
    
public:
    // 물리 세계 업데이트
    std::function<void()>                                    onUpdate;
    
    std::function<void(b2Contact*)>                          onBeginContact;
    std::function<void(b2Contact*)>                          onEndContact;
    std::function<void(b2Contact*, const b2Manifold*)>       onPreSolve;
    std::function<void(b2Contact*, const b2ContactImpulse*)> onPostSolve;
    
    std::function<void(Ball*,Game::Tile*,cocos2d::Vec2)>     onContactBrick;
    std::function<void(Ball*,Game::Tile*)>                   onContactItem;
//    std::function<void(Ball*,PhysicsCategory)>               onContactWall;
    std::function<void(Ball*)>                               onContactWall;
    std::function<void(Ball*)>                               onContactFloor;
    
private:
    GamePhysicsListener() : target(nullptr), contactTarget(nullptr),
    onUpdate(nullptr),
    onBeginContact(nullptr), onEndContact(nullptr), onPreSolve(nullptr), onPostSolve(nullptr),
    onContactBrick(nullptr), onContactItem(nullptr),
    onContactWall(nullptr), onContactFloor(nullptr) {}
    
    CC_SYNTHESIZE(cocos2d::Ref*, target, Target);
    
    // 충돌 타겟
    // 타겟이 속한 충돌일 경우만 충돌 리스너 실행
    // null인 경우, 검사 없이 리스너 실행
    CC_SYNTHESIZE(SBPhysicsObject*, contactTarget, ContactTarget);
};

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
    
private:
    // b2ContactFilter
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;
    
    // b2ContactListener
    void BeginContact(b2Contact *contact) override;
    void EndContact(b2Contact *contact) override;
    void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
public:
    void addListener(GamePhysicsListener *listener);
    void removeListener(GamePhysicsListener *listener);
    void removeListener(cocos2d::Ref *target);

private:
    void dispatchOnUpdate();
    void dispatchOnBeginContact(b2Contact *contact);
    void dispatchOnEndContact(b2Contact *contact);
    void dispatchOnPreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    void dispatchOnPostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
    void dispatchOnContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    void dispatchOnContactItem(Ball *ball, Game::Tile *item);
    void dispatchOnContactWall(Ball *ball);
    void dispatchOnContactFloor(Ball *ball);
    
private:
    CC_SYNTHESIZE_READONLY(b2World*, world, World);
    CC_SYNTHESIZE(DebugDrawView*, debugDrawView, DebugDrawView);
    CC_SYNTHESIZE(GameMap*, map, Map);
    
    cocos2d::Vector<GamePhysicsListener*> listeners;
};

#endif /* GamePhysics_hpp */
