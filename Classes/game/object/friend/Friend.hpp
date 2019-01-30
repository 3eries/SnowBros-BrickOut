//
//  Friend.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef Friend_hpp
#define Friend_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class Friend : public cocos2d::Node {
public:
    static Friend* create();
    ~Friend();
    
private:
    Friend();
    
    bool init() override;
    
public:
    void setImageFlippedX(bool flippedX);
    
private:
    SBAnimationSprite *image;
};

#endif /* Friend_hpp */
