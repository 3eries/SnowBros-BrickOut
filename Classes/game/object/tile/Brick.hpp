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

class Ball;

struct BrickDef {
    BrickData data;
    int hp;
    FloorData floorData;
    
    BrickDef(BrickData _data) : data(_data) {}
};

class Brick : public Game::Tile {
public:
    static Brick* create(const BrickDef &def);
    virtual ~Brick();
    
protected:
    Brick(const BrickDef &def);
    
    virtual bool init() override;
    virtual void onEnter() override;
    
    virtual void initPhysics() override;
    virtual void initBg();
    virtual void initImage();
    virtual void initHpGage();
    
    virtual void setImage(BrickImageType type, bool isRunAnimation);
    
    virtual void onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    
public:
    virtual void enterWithAction() override;
    virtual void removeWithAction() override;
    
    virtual void onBreak();
    
    virtual void sufferDamage(int damage);
    virtual void setHp(int hp, bool updateUI = true);
    
    virtual void updateHpUI();
    
    virtual void setBgVisible(bool isVisible);
    virtual void setHpVisible(bool isVisible);
    
    bool    isBoss();
    bool    isInfinityHp();
    
    bool    isBroken();
    float   getHpRatio();
    
    cocos2d::Node* createWhiteBrickEffect();
    
protected:
    CC_SYNTHESIZE_READONLY(BrickData, data, Data);
    CC_SYNTHESIZE_READONLY(int, originalHp, OriginalHp);
    CC_SYNTHESIZE_READONLY(int, hp, Hp);
    SB_SYNTHESIZE_BOOL(elite, Elite);
    
    CC_SYNTHESIZE(SBCallbackNode, onBreakListener, OnBreakListener)
    
    cocos2d::Sprite *bg;
    CC_SYNTHESIZE_READONLY(SBAnimationSprite*, image, Image);
    BrickImageType imageType;
    
    struct HpGage {
        cocos2d::Node *bg;
        cocos2d::Sprite *gage;
        cocos2d::Sprite *gageEffect;
        cocos2d::Label *label;
        
        HpGage() : bg(nullptr), gage(nullptr), gageEffect(nullptr), label(nullptr) {}
    };
    
    HpGage hpGage;
    
    bool isRunningDamageWhiteEffect;
};

#endif /* Brick_hpp */
