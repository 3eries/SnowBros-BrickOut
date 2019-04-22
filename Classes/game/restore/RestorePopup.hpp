//
//  RestorePopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef RestorePopup_hpp
#define RestorePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class RestorePopup : public BasePopup {
public:
    CREATE_FUNC(RestorePopup);
    ~RestorePopup();
    
private:
    enum Tag {
        BANNER                = 10,
        
        BTN_CLOSE             = 100,
        BTN_RESTORE,
        BTN_REWARDED_VIDEO,
    };
    
private:
    RestorePopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initRestore();
    void initRewardedVideo();
    
    void onClick(cocos2d::Node *sender) override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(std::function<void(bool)>, onRestoreListener, OnRestoreListener);
};

#endif /* RestorePopup_hpp */
