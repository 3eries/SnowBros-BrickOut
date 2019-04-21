//
//  FriendCell.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/04/2019.
//

#include "FriendCell.hpp"

#include "Define.h"
#include "ContentManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

FriendCell* FriendCell::create(const string &selectedImageFile) {
    
    auto cell = new FriendCell();
    
    if( cell && cell->init(selectedImageFile) ) {
        cell->autorelease();
        return cell;
    }
    
    CC_SAFE_DELETE(cell);
    return nullptr;
}

FriendCell::FriendCell() :
onClickListener(nullptr),
isSelected(false),
friendPaddingY(0) {
}

FriendCell::~FriendCell() {
}

bool FriendCell::init(const string &selectedImageFile) {
    
    if( !Widget::init() ) {
        return false;
    }
    
    image = nullptr;
    
    selectedImage = Sprite::create(selectedImageFile);
    selectedImage->setAnchorPoint(ANCHOR_M);
    selectedImage->setPosition(Vec2MC(selectedImage->getContentSize(), 0, 0));
    selectedImage->setVisible(false);
    addChild(selectedImage, 1);
    
    setContentSize(selectedImage->getContentSize());
    
    // 터치 이벤트 등록
    setTouchEnabled(true);
    
    addClickEventListener([=](Ref*) {
        
        if( data.friendId == "" ) {
            return;
        }
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        if( onClickListener ) {
            onClickListener(this);
        }
    });
    
    return true;
}

void FriendCell::setFriend(const FriendData &data) {
    
    this->data = data;
    
    setSelected(false);
    
    if( image ) {
        image->removeFromParent();
        image = nullptr;
    }
    
    if( data.friendId == "" ) {
        return;
    }
    
    string file = ContentResourceHelper::getFriendAnimationFile(data.friendId);
    
    image = spine::SkeletonAnimation::createWithJsonFile(file);
    image->setAnchorPoint(Vec2::ZERO);
    image->setPosition(Vec2MC(getContentSize(), 0, friendPaddingY));
    addChild(image);
}

void FriendCell::setSelected(bool isSelected) {
    
    if( this->isSelected == isSelected ) {
        return;
    }
    
    this->isSelected = isSelected;
    selectedImage->setVisible(isSelected);
    
    if( image ) {
        if( isSelected ) {
            image->clearTracks();
            image->setAnimation(0, FRIEND_ANIM_NAME_IDLE, true);
        } else {
            image->clearTracks();
        }
    }
}
