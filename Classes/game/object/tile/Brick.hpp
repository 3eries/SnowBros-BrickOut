//
//  Brick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#ifndef Brick_hpp
#define Brick_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Tile.hpp"

#include "../../db/BrickData.h"

class Brick : public Game::Tile {
public:
    static Brick* create(const BrickData &data, int hp);
    virtual ~Brick();
    
protected:
    Brick(const BrickData &data, int hp);
    
    virtual bool init() override;
    virtual void initPhysics() override;
    
public:
    virtual void enterWithAction() override;
    virtual void removeWithAction() override;
    
    virtual void onBreak();
    
    virtual void sufferDamage(int damage);
    virtual void setHp(int hp, bool updateUI = true);
    virtual void updateHpUI();
    
    bool    isBroken();
    
    float   getHpGageRatio();
    
protected:
    CC_SYNTHESIZE_READONLY(BrickData, data, Data);
    CC_SYNTHESIZE_READONLY(int, originalHp, OriginalHp);
    CC_SYNTHESIZE_READONLY(int, hp, Hp);
    
    CC_SYNTHESIZE(SBCallbackNode, onBreakListener, OnBreakListener)
    
    SBAnimationSprite *image;
    
    struct HpNode {
        cocos2d::Node *bg;
        cocos2d::Sprite *gage;
        cocos2d::Sprite *gageEffect;
        cocos2d::Label *label;
    };
    
    HpNode hpNode;
};

#endif /* Brick_hpp */
