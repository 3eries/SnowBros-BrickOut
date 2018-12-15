//
//  Friend.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Friend.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

Friend* Friend::create() {
    
    auto friendNode = new Friend();
    
    if( friendNode && friendNode->init() ) {
        friendNode->autorelease();
        return friendNode;
    }
    
    CC_SAFE_DELETE(friendNode);
    return nullptr;
}

Friend::Friend() {
    
}

Friend::~Friend() {
    
}

bool Friend::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    
    // game_friends_02_idle1.png Vec2BL(46, 85) , Size(63, 75)
    // X5 size:26 stroke:3 Vec2BL(50, 29) , Size(45, 28)
    // game_ball_friends_02.png Vec2BL(55, 75) , Size(28, 28)
    StringList anims;
    anims.push_back(DIR_IMG_GAME + "game_friends_02_idle1.png");
    
    image = SBAnimationSprite::create(anims, 0.2f);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(image->getContentSize(), 0,0));
    addChild(image);
    
    image->runAnimation();
    
    setContentSize(image->getContentSize());
    
    return true;
}
