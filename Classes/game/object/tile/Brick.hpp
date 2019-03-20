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
    TileData tile;
    int hp;
    bool elite;
    FloorData floorData;
    
    BrickDef(const BrickData &_data) : data(_data), hp(0), elite(false) {}
    BrickDef(const BrickData &_data, const TileData &_tile,
             int _hp, bool _elite, const FloorData &_floorData) :
    data(_data), tile(_tile), hp(_hp), elite(_elite), floorData(_floorData) {}

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
    
public:
    virtual void setImage(BrickImageType type, bool isRunAnimation);
    virtual void setImageFlippedX(bool flippedX);
    virtual void setImageFlippedY(bool flippedY);
    
    virtual void enterWithAction() override;
    virtual void removeWithAction() override;
    
    virtual void runRemoveAction();
    
    virtual void onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    virtual void onBreak();
    
    virtual void sufferDamage(int damage);
    virtual void setHp(int hp, bool updateUI = true);
    
    virtual void updateHpUI();
    
    virtual void setBgVisible(bool isVisible);
    virtual void setHpVisible(bool isVisible);
    
    bool    isElite();
    bool    isBoss();
    bool    isInfinityHp();
    
    bool    isBroken();
    float   getHpRatio();
    
    cocos2d::Node* createWhiteBrickEffect();
    
protected:
    BrickDef def;
    CC_SYNTHESIZE_READONLY(BrickData, data, Data);
    CC_SYNTHESIZE_READONLY(int, originalHp, OriginalHp);
    CC_SYNTHESIZE_READONLY(int, hp, Hp);
    
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

typedef std::function<void(Brick*)> OnBrickListener;
typedef std::vector<Brick*> BrickList;

#endif /* Brick_hpp */
