//
//  GameScene.hpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseScene.hpp"
#include "Define.h"

#include "GameManager.hpp"

class GameView;

class GameScene : public BaseScene {
public:
    static GameScene* create();
    ~GameScene();
    
private:
    enum Tag {
        BANNER_LOADING           = 100,
        
        BTN_START,
    };
    
private:
    GameScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    bool onApplicationEnterBackground() override;
    bool onBackKeyReleased() override;
    
    void initBg();
    void initBanner();
    void initGameView();
    void initMenu();
    
    void initGameListener();
    void initIAPListener();
    void initGiftListener();
    
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
    
    void onStageClear();
    void onNextFloor(const FloorData &floor);
    
public:
    void replaceScene(SceneType type, const GiftRewardItems &items = GiftRewardItems()) override;
    
    void showPausePopup();
    void showStageClearPopup();
    void showContinuePopup();
    void showResultPopup();

    void checkReview();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    CC_SYNTHESIZE(GameView*, gameView, GameView);
};

#endif /* GameScene_hpp */
