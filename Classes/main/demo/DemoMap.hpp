//
//  DemoMap.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#ifndef DemoMap_hpp
#define DemoMap_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../content/data/model/StageData.h"

class DemoMap : public cocos2d::Node, public SBPhysicsObject {
public:
    static DemoMap* create(const StageData &stage, b2World *world);
    ~DemoMap();
    
private:
    DemoMap();
    
    bool init(const StageData &stage, b2World *world);
};

#endif /* DemoMap_hpp */
