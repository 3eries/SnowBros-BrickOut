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

class UserCoinView;

class StageClearPopup : public BasePopup {
public:
    static StageClearPopup* create(const StageData &stage, BrickDataList bricks,
                                   int rewardCoin);
    ~StageClearPopup();
    
private:
    StageClearPopup();
    
    bool init(const StageData &stage, BrickDataList bricks, int rewardCoin);
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initBg();
    void initBrick();
    void initRewardCoin();
    void initTopMenu();
    void initBottomMenu();
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    StageData stage;
    BrickDataList bossBrickDatas;
    BrickDataList normalBrickDatas;
    int rewardCoin;
    
    CC_SYNTHESIZE(SBCallback, onFriendsListener, OnFriendsListener);
    CC_SYNTHESIZE(SBCallback, onNextListener, OnNextListener);
    
    UserCoinView *userCoinView;
};

#endif /* StageClearPopup_hpp */
