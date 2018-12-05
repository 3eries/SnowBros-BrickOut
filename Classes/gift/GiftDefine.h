//
//  GiftDefine.h
//
//  Created by seongmin hwang on 05/11/2018.
//

#ifndef GiftDefine_h
#define GiftDefine_h

#include "cocos2d.h"
#include "superbomb.h"

enum class GiftType {
    NONE = -1,
    BOOST,
};

typedef std::vector<int> GiftParams;

struct GiftRewardItem {
    GiftType type;
    int amount;
    GiftParams params;
    
    GiftRewardItem() : type(GiftType::NONE), amount(0) {}
};

typedef std::vector<GiftRewardItem> GiftRewardItems;

static inline std::string giftToString(GiftType type) {
    
    switch( type ) {
        case GiftType::BOOST:       return "boost";
        default:
            return "none";
    }
}

#endif /* GiftDefine_h */
