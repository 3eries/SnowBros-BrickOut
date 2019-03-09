//
//  DemoView.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/03/2019.
//

#ifndef DemoView_hpp
#define DemoView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "DemoData.hpp"

class DemoBall;

class DemoView : public cocos2d::Node, public b2ContactFilter, public b2ContactListener {
public:
    CREATE_FUNC(DemoView);
    ~DemoView();
    
private:
    DemoView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initPhysics();
    void initMap();
    void initBrick();
    void initBall();
    
private:
    void shoot();
    void shootStop();
    
    void onFallFinished();
    void onContactFloor(DemoBall *ball);
    
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
    DemoStageData stage;
    b2World *world;
    
    std::vector<DemoBall*> balls;
    cocos2d::Label *ballCountLabel;
    
    int shootIndex;                  // 발사된 볼 인덱스
    int fallenBallCount;             // 떨어진 볼 개수
    cocos2d::Vec2 shootingPosition;  // 볼 발사 위치
};

#endif /* DemoView_hpp */
