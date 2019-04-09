//
//  SplashBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 02/04/2019.
//

#ifndef SplashBall_hpp
#define SplashBall_hpp

#include <stdio.h>

#include "FriendBall.hpp"

class SplashBall : public FriendBall {
public:
    static SplashBall* create(const FriendBallDef &def);
    virtual ~SplashBall();
    
protected:
    SplashBall(const FriendBallDef &def);
    
    virtual bool init() override;
    virtual void initImage() override;
    
public:
    virtual void shoot(b2Vec2 velocity) override;
    virtual void fallToFloor() override;
    
    virtual spine::SkeletonAnimation* createBrickDamageAnimation(Brick *brick,
                                                                 cocos2d::Vec2 contactPoint) override;
protected:
    virtual void onBeginContact(b2Contact *contact) override;
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
    virtual bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    virtual void onContactFloor(Ball *ball) override;
};

#endif /* SplashBall_hpp */
