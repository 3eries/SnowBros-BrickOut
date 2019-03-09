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

#include "DemoData.hpp"

class DemoBrick : public cocos2d::Node, public SBPhysicsObject {
public:
    static DemoBrick* create(const DemoBrickData &demoBrickData, b2World *world);
    ~DemoBrick();
    
private:
    DemoBrick();
    
    bool init(const DemoBrickData &demoBrickData, b2World *world);
};

#endif /* DemoBrick_hpp */
