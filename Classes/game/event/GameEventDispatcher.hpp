//
//  GameEventDispatcher.hpp
//
//  Created by seongmin hwang on 29/11/2018.
//

#ifndef GameEventDispatcher_hpp
#define GameEventDispatcher_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../GameDefine.h"
#include "GameEventListener.hpp"

/** @class GameEventDispatcher
 * @brief 이 클래스는 게임 이벤트 리스너를 관리합니다
 */
class GameEventDispatcher {
public:
    GameEventDispatcher();
    ~GameEventDispatcher();
    
public:
    typedef cocos2d::Vector<GameEventListener*> GameEventListeners;
    
public:
    void addListener(GameEventListener *listener);
    void removeListener(GameEventListener *listener);
    void removeListener(cocos2d::Ref *target);
    void removeListeners(GameEventListeners listeners);
    
public:
    void dispatchOnGameEnter();
    void dispatchOnGameExit();
    void dispatchOnGameReset();
    void dispatchOnGameStart();
    void dispatchOnGameRestart();
    void dispatchOnGamePause();
    void dispatchOnGameResume();
    void dispatchOnGameOver();
    void dispatchOnGameContinue();
    void dispatchOnGameRestore(const RestoreData &restoreData);
    void dispatchOnGameResult();
    
    void dispatchOnBoostStart();
    void dispatchOnBoosting();
    void dispatchOnBoostEnd();

    void dispatchOnWorldChanged(const WorldData &world);
    void dispatchOnWorldClear();
    
    void dispatchOnStageChanged(const StageData &stage);
    void dispatchOnStageClear();
    
    void dispatchOnMoveNextStage(const StageData &stage);
    void dispatchOnMoveNextStageFinished(const StageData &stage);
    
    void dispatchOnFloorChanged(const FloorData &floor);
    void dispatchOnNextFloor(const FloorData &floor);
    
    void dispatchOnScoreChanged(int score);
    
private:
    GameEventListeners listeners;
};

#endif /* GameEventDispatcher_hpp */
