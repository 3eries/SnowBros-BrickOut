//
//  ShieldBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 28/02/2019.
//

#ifndef ShieldBrick_hpp
#define ShieldBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class ShieldBrick : public Brick {
public:
    static ShieldBrick* create(const BrickDef &def);
    ~ShieldBrick();
    
private:
    ShieldBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    bool onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint) override;
    
private:
    SBAnimationSprite *shieldAnim;
};

#endif /* ShieldBrick_hpp */
