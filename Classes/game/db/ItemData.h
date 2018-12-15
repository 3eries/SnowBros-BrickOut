//
//  ItemData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef ItemData_h
#define ItemData_h

#include "cocos2d.h"

enum class ItemType {
    NONE = 0,
    POWER_UP,                   // 볼 증가
    FRIENDS_POWER_UP,           // 프렌즈 볼 증가
};

struct ItemData {
    ItemType type;
    
    ItemData() : type(ItemType::NONE) {}
};

#endif /* ItemData_h */
