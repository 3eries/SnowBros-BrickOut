//
//  FriendPreviewView.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/05/2019.
//

#ifndef FriendPreviewView_hpp
#define FriendPreviewView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../content/data/model/FriendData.h"

class Ball;
class Brick;
class PreviewBall;

class FriendPreviewView : public cocos2d::Node, public b2ContactFilter, public b2ContactListener {
public:
    static FriendPreviewView* create(const FriendData &data);
    ~FriendPreviewView();
    
private:
    FriendPreviewView();
    
    bool init(const FriendData &data);
    void onEnter() override;
    void onExit() override;
    
    void initPhysics();
    void initBg();
    void initFriend();
    void initBall();
    void initBrick();
    
private:
    void shoot();
    void shootStop();
    
    void onFallFinished();
    void onContactFloor(Ball *ball);
    
    void onBrickBreak(Brick *brick);
    
private:
    void update(float dt) override;
    
    // b2ContactFilter
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;
    
    // b2ContactListener
    void BeginContact(b2Contact *contact) override;
    void EndContact(b2Contact *contact) override;
    void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    
private:
    FriendData data;
    b2World *world;
    
    cocos2d::ClippingNode *clippingNode;
    cocos2d::Node *bg;
    spine::SkeletonAnimation *friendImage;
    
    std::vector<PreviewBall*> balls;
    cocos2d::Label *ballCountLabel;
    
    int shootIndex;                  // 발사된 볼 인덱스
    int fallenBallCount;             // 떨어진 볼 개수
    cocos2d::Vec2 shootingPosition;  // 볼 발사 위치
};

#endif /* FriendPreviewView_hpp */
