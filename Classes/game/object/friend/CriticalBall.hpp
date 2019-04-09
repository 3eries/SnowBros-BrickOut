//
//  CriticalBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/04/2019.
//

#ifndef CriticalBall_hpp
#define CriticalBall_hpp

#include <stdio.h>

#include "FriendBall.hpp"

class CriticalBall : public FriendBall {
public:
    static CriticalBall* create(const FriendBallDef &def);
    virtual ~CriticalBall();
    
protected:
    CriticalBall(const FriendBallDef &def);
    
    virtual bool init() override;
    virtual void initImage() override;
    
public:
    virtual void shoot(b2Vec2 velocity) override;
    virtual void fallToFloor() override;
    
protected:
    virtual void onBeginContact(b2Contact *contact) override;
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
    virtual bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    virtual void onContactFloor(Ball *ball) override;
};

#endif /* CriticalBall_hpp */
