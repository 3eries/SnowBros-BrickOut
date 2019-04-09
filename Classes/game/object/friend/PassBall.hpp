//
//  PassBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 02/04/2019.
//

#ifndef PassBall_hpp
#define PassBall_hpp

#include <stdio.h>

#include "FriendBall.hpp"

class PassBall : public FriendBall {
public:
    static PassBall* create(const FriendBallDef &def);
    virtual ~PassBall();
    
protected:
    PassBall(const FriendBallDef &def);
    
    virtual bool init() override;
    virtual void initImage() override;
    
public:
    virtual bool afterStep() override;
    
    virtual void shoot(b2Vec2 velocity) override;
    virtual void fallToFloor() override;
    
protected:
    virtual void onBeginContact(b2Contact *contact) override;
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
    virtual bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    virtual void onContactFloor(Ball *ball) override;
    
protected:
    std::vector<Brick*> contactBricks;
};

#endif /* PassBall_hpp */
