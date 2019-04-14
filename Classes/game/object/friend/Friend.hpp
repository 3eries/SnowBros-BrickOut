//
//  Friend.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef Friend_hpp
#define Friend_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../../content/data/model/FriendData.h"
#include "FriendBall.hpp"

class TileLayer;

struct FriendDef {
    FriendData data;
    int damage;
    TileLayer *tileLayer;
    
    FriendDef(const FriendData &_data) : data(_data), damage(1), tileLayer(nullptr) {}
};

class Friend : public cocos2d::Node {
public:
    enum class ImageType {
        NONE,
        IDLE,
        MOVE,
        DIE,
        SHOOT_LOW,
        SHOOT_HIGH,
        STAGE_START,
        STAGE_CLEAR,
    };
    
public:
    static Friend* create(const FriendDef &def);
    virtual ~Friend();
    
protected:
    Friend();
    
    virtual bool init(const FriendDef &def);
    
    virtual void initImage();
    virtual void initBall();
    
    virtual FriendBall* createBall();
    virtual void        addBall(int count = 1);
    
    int                 getBallCount();
    
private:
    virtual void onContactBrick(FriendBall *ball, Game::Tile *brick);
    virtual void onContactFloor(FriendBall *ball);
    
public:
    virtual void          shoot(float delay);
    virtual void          shootStop();
    virtual void          onShootFinished();
    
    cocos2d::Vec2         getShootingPosition();
    virtual cocos2d::Vec2 getShootingTargetPosition();
    
    virtual void          withdrawBall();
    
    bool                  isFallFinished();
    
    virtual void          setImage(ImageType type, SBCallback onAnimationFinished = nullptr);
    virtual void          setImageFlippedX(bool flippedX);
    
    virtual void          setDamage(int damage);
    virtual void          setDamageVisible(bool isVisible);
    
    float                 getBallMaxVelocity();
    
protected:
    CC_SYNTHESIZE(std::function<void(Friend*)>, onFallFinishedListener,
                  OnFallFinishedListener);
    CC_SYNTHESIZE(TileLayer*, tileLayer, TileLayer);
    
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    CC_SYNTHESIZE_READONLY(int, damage, Damage);
    
    int shootIndex;       // 발사된 볼 인덱스
    int fallenBallCount;  // 떨어진 볼 개수
    
    spine::SkeletonAnimation *image;
    std::vector<FriendBall*> balls;
    cocos2d::Label *damageLabel;
};

#endif /* Friend_hpp */
