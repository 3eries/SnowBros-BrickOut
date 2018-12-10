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

class Ball : public cocos2d::Node, public SBPhysicsObject {
public:
    CREATE_FUNC(Ball);
    ~Ball();
    
private:
    Ball();
    
    bool init() override;
    
public:
    void beforeStep() override;
    void afterStep() override;
    
    void shoot(b2Vec2 velocity);
    void sleepWithAction();
    
private:
    cocos2d::Sprite *image;
    
    CC_SYNTHESIZE(int, damage, Damage);
};

#endif /* Ball_hpp */
