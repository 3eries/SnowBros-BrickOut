//
//  GameView.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "Define.h"
#include "GameManager.hpp"

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    enum Tag {
        BG                 = 1,
        CLOUD,
    };
    
    enum class ZOrder {
        BG = -2,
        CLOUD = -1,
    };
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    
    void initBg();
    void initHero();
    void initGameListener();
    void initIAPListener();
    
// Game Event
public:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameContinue();
    void onGameResult();
    
    void onBoostStart();
    void onBoostEnd();
    
    void onScoreChanged(int score);
    void onLevelChanged(const LevelInfo &level);
};

#endif /* GameView_hpp */
