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

class TopMenu : public cocos2d::Node {
public:
    static TopMenu* create();
    ~TopMenu();
    
private:
    enum Tag {
        STAGE = 10,
        SCORE,
    };
    
private:
    TopMenu();
    
    bool init() override;
    void cleanup() override;
    
    void initGameListener();
    
private:
    void updateFloorProgressUI(int floor, int floorLen);
    
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
    
    void onStageClear();
    void onNextStage(const StageData &stage);
    void onNextFloor(const FloorData &floor);
    
    void onScoreChanged(int score);
    
private:
    struct FloorProgressBar {
        cocos2d::Sprite *gage;
        cocos2d::Sprite *hero;
        cocos2d::Sprite *enemy;
        cocos2d::Label *label;
    };
    
    FloorProgressBar floorProgressBar;
};

#endif /* TopMenu_hpp */
