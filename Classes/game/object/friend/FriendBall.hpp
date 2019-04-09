//
//  FriendBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/03/2019.
//

#ifndef FriendBall_hpp
#define FriendBall_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../Ball.hpp"
#include "../../../content/data/model/FriendData.h"

struct FriendBallDef {
    FriendData data;
    int index;
    
    FriendBallDef(const FriendData &_data) : data(_data), index(-1) {}
};

class TileLayer;
class Brick;

class FriendBall : public Ball {
public:
    static FriendBall* create(const FriendBallDef &def);
    virtual ~FriendBall();
    
protected:
    FriendBall(const FriendBallDef &def);
    
    virtual bool init() override;
    virtual void initImage() override;
    
public:
    virtual void shoot(b2Vec2 velocity) override;
    virtual void fallToFloor() override;
    
    virtual SBAnimationSprite*        createHitAnimation() override;
    virtual spine::SkeletonAnimation* createBrickDamageAnimation(Brick *brick,
                                                                 cocos2d::Vec2 contactPoint);
protected:
    virtual void onBeginContact(b2Contact *contact) override;
    virtual void onEndContact(b2Contact *contact) override;
    virtual void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    virtual void onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    virtual bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    virtual void onContactItem(Ball *ball, Game::Tile *item) override;
    virtual void onContactWall(Ball *ball) override;
    virtual void onContactFloor(Ball *ball) override;
    
protected:
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    CC_SYNTHESIZE_READONLY(int, ballIndex, BallIndex);
    CC_SYNTHESIZE(TileLayer*, tileLayer, TileLayer);
    
    CC_SYNTHESIZE(std::function<void(FriendBall*, Game::Tile*)>,
                  onContactBrickListener, OnContactBrickListener);
    CC_SYNTHESIZE(std::function<void(FriendBall*)>,
                  onContactFloorListener, OnContactFloorListener);
};

#endif /* FriendBall_hpp */
