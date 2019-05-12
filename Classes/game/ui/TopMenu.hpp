//
//  TopMenu.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#ifndef TopMenu_hpp
#define TopMenu_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class TopMenu : public cocos2d::Node, public SBNodeListener {
public:
    static TopMenu* create();
    ~TopMenu();
    
private:
    enum Tag {
        WORLD = 10,
        STAGE,
        SCORE,
        
        BTN_PAUSE = 100,
    };
    
private:
    TopMenu();
    
    bool init() override;
    void onEnter() override;
    void cleanup() override;
    
    void initBg();
    void initStage();
    void initScore();
    void initMenu();
    void initGameListener();
    
private:
    void onClick(cocos2d::Node *sender) override;
    
    void updateStageUI(const StageData &stage);
    void updateFloorProgressUI(const StageData &stage, const FloorData &floor);
    
// Game Event
public:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGameOver();
    void onGameContinue();
    void onGameResult();
    
    void onBoostStart();
    void onBoostEnd();
    
    void onStageChanged(const StageData &stage);
    void onStageClear();
    
    void onMoveNextStage(const StageData &stage);
    void onMoveNextStageFinished(const StageData &stage);
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
    void onScoreChanged(int score);
    
private:
    struct FloorProgressBar {
        cocos2d::Node *layer;
        cocos2d::Sprite *gage;
        cocos2d::Sprite *hero;
        cocos2d::Sprite *enemy;
        cocos2d::Label *label;
    };
    
    FloorProgressBar floorProgressBar;
};

#endif /* TopMenu_hpp */
