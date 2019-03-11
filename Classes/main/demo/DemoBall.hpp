//
//  DemoBall.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#ifndef DemoBall_hpp
#define DemoBall_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class DemoBall : public cocos2d::Node, public SBPhysicsObject {
public:
    static DemoBall* create(b2World *world);
    ~DemoBall();
    
private:
    DemoBall();
    bool init(b2World *world);

public:
    bool afterStep() override;
    
    void shoot(b2Vec2 velocity);
    
    void onContactBrick(cocos2d::Vec2 contactPoint);
    void onContactWall();
    void onContactFloor();
    
private:
    cocos2d::Sprite *image;
    
    SB_SYNTHESIZE_BOOL(fall, Fall); // 추락 여부
    
    // 충돌 횟수
    // 발사 후 충돌을 카운팅합니다
    // 모든 충돌 횟수는 추락 시 리셋됩니다
    size_t contactCount;
    size_t brickContactCount; // 연속된 브릭 충돌 횟수
    size_t wallContactCount;  // 연속된 벽 충돌 횟수
};

#endif /* DemoBall_hpp */
