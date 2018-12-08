//
//  GameMap.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#ifndef GameMap_hpp
#define GameMap_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Box2D/Box2D.h"

class GameMap : public cocos2d::Node, public SBPhysicsObject {
public:
    CREATE_FUNC(GameMap);
    ~GameMap();
    
private:
    GameMap();
    
    bool init() override;
};

#endif /* GameMap_hpp */
