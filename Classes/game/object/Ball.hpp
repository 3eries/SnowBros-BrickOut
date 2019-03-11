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
    ~Ball();
    
private:
    Ball();
    
    bool init() override;
    void cleanup() override;
    
    void initPhysicsListener();
    
public:
    static b2Body* createBody(SBPhysicsObject *userData = nullptr);
    
    bool beforeStep() override;
    bool afterStep() override;
    
    void shoot(b2Vec2 velocity);
    void fallToFloor();
    
    void checkMovement(float dt);
    
    void sleepWithAction();
    
private:
    void onBeginContact(b2Contact *contact);
    void onEndContact(b2Contact *contact);
    void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
    void onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    void onContactItem(Ball *ball, Game::Tile *item);
    void onContactWall(Ball *ball);
    void onContactFloor(Ball *ball);
    
private:
    cocos2d::Sprite *image;
    
    CC_SYNTHESIZE(int, damage, Damage);
    
    SB_SYNTHESIZE_BOOL(fall, Fall);   // 추락 여부
    
    // 충돌 횟수
    // 발사 후 충돌을 카운팅합니다
    // 모든 충돌 횟수는 추락 시 리셋됩니다
    size_t contactCount;
    size_t brickContactCount; // 연속된 브릭 충돌 횟수
    size_t wallContactCount;  // 연속된 벽 충돌 횟수
};

#endif /* Ball_hpp */
