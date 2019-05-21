//
//  PreviewBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 17/05/2019.
//

#ifndef PreviewBall_hpp
#define PreviewBall_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../game/object/friend/FriendBall.hpp"

class PreviewBall : public FriendBall {
public:
    static PreviewBall* create(const FriendBallDef &def);
    virtual ~PreviewBall();
    
protected:
    PreviewBall(const FriendBallDef &def);
    
    virtual bool init() override;
    virtual void initImage() override;
    virtual void initPhysicsListener() override;
    
public:
    virtual void    shoot(b2Vec2 velocity) override;
    virtual float   getMaxVelocity() override;
    virtual void    fallToFloor() override;
    
public:
    virtual void onBeginContact(b2Contact *contact) override;
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
    virtual bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    virtual void onContactFloor(Ball *ball) override;
    
    void onPassTypePreSolve(b2Contact *contact);
    
protected:
    std::vector<Brick*> contactBricks;
};

#endif /* PreviewBall_hpp */
