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

#include "ContentManager.hpp"

class GameMap : public cocos2d::Node, public SBPhysicsObject {
public:
    CREATE_FUNC(GameMap);
    ~GameMap();
    
private:
    GameMap();
    
    bool init() override;
    void initGameListener();
    
    class Background : public cocos2d::Node {
    public:
        CREATE_FUNC(Background);
        Background() {}
        ~Background() {}
        
        cocos2d::Node *top;
        cocos2d::Node *center;
        cocos2d::Node *bottom;
    };
    
    Background* createBackground(const StageData &stage);
    
private:
    void onStageChanged(const StageData &stage);

    void onMoveNextStage(const StageData &stage);
    void onMoveNextStageFinished(const StageData &stage);
    
private:
    Background *bg;
};

#endif /* GameMap_hpp */
