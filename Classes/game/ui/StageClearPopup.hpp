//
//  StageClearPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#ifndef StageClearPopup_hpp
#define StageClearPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "ContentManager.hpp"

class StageClearPopup : public BasePopup {
public:
    static StageClearPopup* create(const StageData &stage);
    ~StageClearPopup();
    
private:
    StageClearPopup(const StageData &stage);
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    StageData stage;
    CC_SYNTHESIZE(SBCallback, onNextListener, OnNextListener);
};

#endif /* StageClearPopup_hpp */
