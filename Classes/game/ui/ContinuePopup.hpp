//
//  ContinuePopup.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef ContinuePopup_hpp
#define ContinuePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ContinuePopup : public BasePopup {
private:
    enum Tag {
        BTN_CONTINUE = 100,
    };
    
public:
    static ContinuePopup* create(bool isAdsLoaded);
    ~ContinuePopup();
    
private:
    ContinuePopup();
    
    bool init(bool isAdsLoaded);
    void onEnter() override;
    void onExit() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    void initMenu(bool isAdsLoaded);
    
    void countdown();
    void timeOut();
    void onClickContinue();
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    bool isAdsLoaded;
    bool isTimeOut;
    bool isContinue;
    
    CC_SYNTHESIZE(SBCallback, onContinueListener, OnContinueListener);
    CC_SYNTHESIZE(SBCallback, onTimeOutListener, OnTimeOutListener);
    
    int count;
    spine::SkeletonAnimation *anim;
};

#endif /* ContinuePopup_hpp */
