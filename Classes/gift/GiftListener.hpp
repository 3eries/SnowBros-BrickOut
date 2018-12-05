//
//  GiftListener.hpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#ifndef GiftListener_hpp
#define GiftListener_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "GiftDefine.h"

typedef std::function<void(GiftType)>               OnGiftListener;
typedef std::function<void(const GiftRewardItem&)>  OnGiftRewardedListener;

class GiftListener : public cocos2d::Ref {
public:
    SB_REF_CREATE_FUNC(GiftListener);
    GiftListener();
    ~GiftListener();
    
    OnGiftListener          onClickBox;     // 선물상자 클릭
    OnGiftRewardedListener  onRewarded;     // 보상
    
private:
    CC_SYNTHESIZE(cocos2d::Ref*, target, Target);
};

#endif /* GiftListener_hpp */
