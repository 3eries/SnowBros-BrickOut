//
//  FriendCell.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/04/2019.
//

#ifndef FriendCell_hpp
#define FriendCell_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../content/data/model/FriendData.h"

class FriendCell : public cocos2d::ui::Widget {
public:
    static FriendCell* create(const std::string &selectedImageFile);
    ~FriendCell();
    
private:
    FriendCell();
    
    bool init(const std::string &selectedImageFile);
    
public:
    void setFriend(const FriendData &data);
    void setSelected(bool isSelected);
    
private:
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    CC_SYNTHESIZE(std::function<void(FriendCell*)>, onClickListener, OnClickListener);
    bool isSelected;
    
    spine::SkeletonAnimation *image; // 프렌드 이미지
    cocos2d::Sprite *selectedImage;  // 선택 상태 이미지
    CC_SYNTHESIZE(int, friendPaddingY, FriendPaddingY);
};

#endif /* FriendCell_hpp */
