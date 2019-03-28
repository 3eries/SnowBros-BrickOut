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

struct FriendDef {
    FriendData data;
    int power;
    
    FriendDef(const FriendData &_data, int _power) : data(_data), power(_power) {}
    FriendDef(const FriendData &_data) : data(_data), power(1) {}
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
    virtual void initPhysicsListener();
    
    virtual FriendBall* createBall();
    virtual void        addBall(int count = 1);
    
    int                 getBallCount();
    
private:
//    virtual void onContactBrick(Ball *ball, Game::Tile *brickTile, cocos2d::Vec2 contactPoint);
//    virtual void onContactWall(Ball *ball);
    virtual void onContactFloor(FriendBall *ball);
    
public:
    virtual void    shoot(cocos2d::Vec2 endPosition);
    virtual void    shootStop();
    virtual void    onShootFinished();
    
    virtual void    withdrawBall();
    
    bool            isFallFinished();
    cocos2d::Vec2   getShootingPosition();
    
    virtual void    setImage(ImageType type, SBCallback onAnimationFinished = nullptr);
    virtual void    setImageFlippedX(bool flippedX);
    
    virtual void    setPower(int power);
    virtual void    setPowerVisible(bool isVisible);
    
protected:
    CC_SYNTHESIZE(std::function<void(Friend*)>, onFallFinishedListener,
                  OnFallFinishedListener);
    
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    CC_SYNTHESIZE_READONLY(int, power, Power);
    
    int shootIndex;       // 발사된 볼 인덱스
    int fallenBallCount;  // 떨어진 볼 개수
    
    spine::SkeletonAnimation *image;
    std::vector<FriendBall*> balls;
    cocos2d::Label *powerLabel;
};

#endif /* Friend_hpp */
