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

class TileLayer;
class Ball;
class FriendBall;

struct BrickDef {
    BrickData data;
    TileData tile;
    int hp;
    bool elite;
    FloorData floorData;
    b2World *world;
    
    BrickDef(const BrickData &_data) : data(_data), hp(0), elite(false), world(nullptr) {}
    BrickDef(const BrickData &_data, const TileData &_tile,
             int _hp, bool _elite, const FloorData &_floorData, b2World *_world) :
    data(_data), tile(_tile), hp(_hp), elite(_elite), floorData(_floorData), world(_world) {}

};

class Brick : public Game::Tile {
public:
    static Brick* create(const BrickDef &def);
    virtual ~Brick();
    
public:
    enum class ZOrder {
        BG            = -1,
        IMAGE,
        
        DAMAGE_EFFECT = 5,
        
        HP_BAR        = 10,
        HP_LABEL,
    };
    
protected:
    Brick(const BrickDef &def);
    
    virtual bool init() override;
    virtual void onEnter() override;
    
    virtual void initPhysics() override;
    virtual void initPhysicsListener();
    virtual void initBg();
    virtual void initImage();
    virtual void initHpGage();
    
public:
    virtual void    setImage(BrickImageType type, bool isRunAnimation);
    virtual void    setImageFlippedX(bool flippedX);
    virtual void    setImageFlippedY(bool flippedY);
    
    virtual void    prepareRemove() override;
    
    virtual void    enterWithAction() override;
    virtual void    removeWithAction() override;
    
    virtual void    runBallHitAction(Ball *ball, cocos2d::Vec2 contactPoint);
    virtual void    runFriendBallDamageAction(FriendBall *ball, cocos2d::Vec2 contactPoint);
    
    virtual void    onPreSolve(b2Contact *contact, const b2Manifold *oldManifold);
    virtual bool    onContactBrick(Ball *ball, Game::Tile *brick, cocos2d::Vec2 contactPoint);
    
    virtual void    onBreak();
    
    virtual bool    sufferDamage(Ball *ball, cocos2d::Vec2 contactPoint,
                              bool withBallDamageAction);
    virtual bool    sufferDamage(Ball *ball, cocos2d::Vec2 contactPoint);
    virtual void    setHp(int hp, bool updateUI = true);
    
    virtual void    updateHpUI();
    
    virtual void    setBgVisible(bool isVisible);
    virtual void    setHpVisible(bool isVisible);
    
    virtual bool    canDamage();
    virtual bool    canDamagePoint(cocos2d::Vec2 p);
    virtual float   getEnterDuration();
    
    bool         isElite();
    bool         isBoss();
    bool         isInfinityHp();
    bool         isBroken();
    float        getHpRatio();
    
protected:
    BrickDef def;
    CC_SYNTHESIZE_READONLY(BrickData, data, Data);
    CC_SYNTHESIZE_READONLY(int, originalHp, OriginalHp);
    CC_SYNTHESIZE_READONLY(int, hp, Hp);
    int prevHp;
    
    CC_SYNTHESIZE(TileLayer*, tileLayer, TileLayer);
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
    
    typedef cocos2d::Vector<SBAnimationSprite*> BallHitAnimations;
    BallHitAnimations ballHitAnims;
    BallHitAnimations friendsBallHitAnims;
};

typedef std::function<void(Brick*)> OnBrickListener;
typedef std::vector<Brick*> BrickList;

#endif /* Brick_hpp */
