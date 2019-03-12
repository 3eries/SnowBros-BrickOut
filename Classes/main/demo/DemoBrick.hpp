//
//  DemoBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#ifndef DemoBrick_hpp
#define DemoBrick_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../content/data/model/DemoData.h"

class DemoBrick : public cocos2d::Node, public SBPhysicsObject {
public:
    static DemoBrick* create(const DemoBrickData &demoBrickData, b2World *world);
    ~DemoBrick();
    
private:
    DemoBrick();
    
    bool init(const DemoBrickData &demoBrickData, b2World *world);
    
    void setImage(BrickImageType type, bool isRunAnimation);
    
public:
    void onContactBrick();
    
private:
    DemoBrickData data;
    
    SBAnimationSprite *image;
    BrickImageType imageType;
};

#endif /* DemoBrick_hpp */
