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

class Friend : public cocos2d::Node {
public:
    enum class ImageType {
        NONE,
        IDLE,
        MOVE,
        DIE,
        STAGE_START,
        STAGE_CLEAR,
    };
    
public:
    static Friend* create(const FriendData &data);
    ~Friend();
    
private:
    Friend(const FriendData &data);
    
    bool init() override;
    
public:
    void setImage(ImageType type, SBCallback onAnimationFinished = nullptr);
    void setImageFlippedX(bool flippedX);
    
private:
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    spine::SkeletonAnimation *image;
};

#endif /* Friend_hpp */
