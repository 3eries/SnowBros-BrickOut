//
//  Ball.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 08/12/2018.
//

#ifndef Ball_hpp
#define Ball_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

namespace Game {
    class Tile;
}

class Ball : public cocos2d::Node, public SBPhysicsObject {
public:
    CREATE_FUNC(Ball);
    virtual ~Ball();
    
protected:
    Ball();
    
    virtual bool init() override;
    virtual void cleanup() override;
    
    virtual void initImage();
    virtual void initPhysicsListener();
    
public:
    static b2Body* createBody(SBPhysicsObject *userData = nullptr);
    
    virtual bool beforeStep() override;
    virtual bool afterStep() override;
    
    virtual void shoot(b2Vec2 velocity);
    virtual void fallToFloor();
    
    virtual void checkMovement(float dt);
    
    virtual void sleepWithAction();
    virtual void runHitAction(cocos2d::Vec2 contactPoint);
    
protected:
    virtual void onBeginContact(b2Contact *contact);
    virtual void onEndContact(b2Contact *contact);
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
    virtual void onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    virtual void onContactItem(Ball *ball, Game::Tile *item);
    virtual void onContactWall(Ball *ball);
    virtual void onContactFloor(Ball *ball);
    
protected:
    cocos2d::Sprite *image;
    
    CC_SYNTHESIZE(int, power, Power);
    SB_SYNTHESIZE_BOOL(fall, Fall);   // 추락 여부
    
    // 충돌 횟수
    // 발사 후 충돌을 카운팅합니다
    // 모든 충돌 횟수는 추락 시 리셋됩니다
    size_t contactCount;
    size_t brickContactCount; // 연속된 브릭 충돌 횟수
    size_t wallContactCount;  // 연속된 벽 충돌 횟수
};

#endif /* Ball_hpp */
