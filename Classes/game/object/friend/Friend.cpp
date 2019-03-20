//
//  Friend.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Friend.hpp"

#include "Define.h"
#include "ContentResourceHelper.hpp"

USING_NS_CC;
using namespace std;

#define FRIEND_SIZE                     Size(63, 75)

#define ANIM_NAME_IDLE                  "idle"
#define ANIM_NAME_STAGE_START           "stage_start"
#define ANIM_NAME_STAGE_CLEAR           "stage_clear"
#define ANIM_NAME_MOVE                  "move"
#define ANIM_NAME_DIE                   "die"

Friend* Friend::create(const FriendData &data) {
    
    auto friendNode = new Friend(data);
    
    if( friendNode && friendNode->init() ) {
        friendNode->autorelease();
        return friendNode;
    }
    
    CC_SAFE_DELETE(friendNode);
    return nullptr;
}

Friend::Friend(const FriendData &data) :
data(data) {
}

Friend::~Friend() {
    
}

bool Friend::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    setCascadeOpacityEnabled(true);
    
    string file = ContentResourceHelper::getFriendAnimationFile(data.friendId);
    
    image = spine::SkeletonAnimation::createWithJsonFile(file);
    image->setAnchorPoint(Vec2::ZERO);
    image->setPosition(Vec2MC(0, 0));
    addChild(image);
    
    SBSpineHelper::clearAnimation(image, ANIM_NAME_CLEAR);
    
    return true;
}

void Friend::setImage(ImageType type, SBCallback onAnimationFinished) {
    
    // SBSpineHelper::clearAnimation(image, ANIM_NAME_CLEAR);
//    image->clearTracks();
//    image->update(0);

    auto setAnimation = [=]() ->spTrackEntry* {
        switch( type ) {
            case ImageType::IDLE:           return image->setAnimation(0, ANIM_NAME_IDLE, true);
            case ImageType::MOVE:           return image->setAnimation(0, ANIM_NAME_MOVE, true);
            case ImageType::DIE:            return image->setAnimation(0, ANIM_NAME_DIE, false);
            case ImageType::STAGE_START:    return image->setAnimation(0, ANIM_NAME_STAGE_START, false);
            case ImageType::STAGE_CLEAR:    return image->setAnimation(0, ANIM_NAME_STAGE_CLEAR, false);
            default:
                CCASSERT(false, "Friend::setImage error: invalid image type.");
                break;
        }
    };
    
    auto track = setAnimation();
    
    if( track && onAnimationFinished ) {
        image->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
            onAnimationFinished();
        });
    }
}

void Friend::setImageFlippedX(bool flippedX) {
    
    image->setScaleX(flippedX ? -1 : 1);
}
