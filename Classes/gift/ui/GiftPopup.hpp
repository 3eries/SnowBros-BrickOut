//
//  GiftPopup.hpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#ifndef GiftPopup_hpp
#define GiftPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "GiftDefine.h"

/** @class GiftPopup
 * @brief 선물 알림 팝업
 */
class GiftPopup : public BasePopup {
public:
    static GiftPopup* create(GiftRewardItem item);
    ~GiftPopup();
    
private:
    GiftPopup(GiftRewardItem item);
    
    bool init() override;
    void onEnter() override;
    bool onBackKeyReleased() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE_READONLY(GiftRewardItem, giftRewardItem, GiftRewardItem);
    CC_SYNTHESIZE(SBCallbackNode, onGetListener, OnGetListener);
    
    cocos2d::Node *stoneBg;
};

#endif /* GiftPopup_hpp */
