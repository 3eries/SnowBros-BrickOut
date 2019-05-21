//
//  PreviewBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 17/05/2019.
//

#ifndef PreviewBrick_hpp
#define PreviewBrick_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../game/object/tile/Brick.hpp"

class PreviewBrick : public Brick {
public:
    static PreviewBrick* create(const BrickDef &def);
    ~PreviewBrick();
    
private:
    PreviewBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void initGameListener() override;
    void initPhysicsListener() override;
    
    bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
};

#endif /* PreviewBrick_hpp */
