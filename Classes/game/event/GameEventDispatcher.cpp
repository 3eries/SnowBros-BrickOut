//
//  GameEventDispatcher.cpp
//
//  Created by seongmin hwang on 29/11/2018.
//

#include "GameEventDispatcher.hpp"

USING_NS_CC;
using namespace std;

GameEventDispatcher::GameEventDispatcher() {
}

GameEventDispatcher::~GameEventDispatcher() {
    
    listeners.clear();
}

/**
 * 이벤트 리스너 추가
 */
void GameEventDispatcher::addListener(GameEventListener *listener) {
    
    if( listener ) {
        if( listeners.contains(listener) ) {
            CCASSERT(false, "GameEventDispatcher::addListener error: already exists.");
            return;
        }
        
        this->listeners.pushBack(listener);
    }
}

/**
 * 이벤트 리스너 삭제
 */
void GameEventDispatcher::removeListener(GameEventListener *listener) {
    
    if( listener && listeners.contains(listener) ) {
        this->listeners.eraseObject(listener);
    }
}

void GameEventDispatcher::removeListener(Ref *target) {
    
    if( !target ) {
        return;
    }
    
    SBCollection::remove(this->listeners, [=](GameEventListener *listener) {
        return listener->getTarget() == target;
    });
}

void GameEventDispatcher::removeListeners(GameEventListeners listeners) {
    
    for( auto listener : listeners ) {
        removeListener(listener);
    }
}

/**
 * onGameEnter 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameEnter() {
    
    for( auto listener : listeners ) {
        if( listener->onGameEnter ) {
            listener->onGameEnter();
        }
    }
}

/**
 * onGameExit 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameExit() {
    
    for( auto listener : listeners ) {
        if( listener->onGameExit ) {
            listener->onGameExit();
        }
    }
}

/**
 * onGameReset 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameReset() {
    
    for( auto listener : listeners ) {
        if( listener->onGameReset ) {
            listener->onGameReset();
        }
    }
}

/**
 * onGameStart 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameStart() {
    
    for( auto listener : listeners ) {
        if( listener->onGameStart ) {
            listener->onGameStart();
        }
    }
}

/**
 * onGameRestart 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameRestart() {
    
    for( auto listener : listeners ) {
        if( listener->onGameRestart ) {
            listener->onGameRestart();
        }
    }
}

/**
 * onGamePause 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGamePause() {
    
    for( auto listener : listeners ) {
        if( listener->onGamePause ) {
            listener->onGamePause();
        }
    }
}

/**
 * onGameResume 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameResume() {
    
    for( auto listener : listeners ) {
        if( listener->onGameResume ) {
            listener->onGameResume();
        }
    }
}

/**
 * onGameOver 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameOver() {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onGameOver ) {
            listener->onGameOver();
        }
    }
}

/**
 * onGameContinue 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameContinue() {
    
    for( auto listener : listeners ) {
        if( listener->onGameContinue ) {
            listener->onGameContinue();
        }
    }
}

/**
 * onGameRestore 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameRestore(const RestoreData &restoreData) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onGameRestore ) {
            listener->onGameRestore(restoreData);
        }
    }
}

/**
 * onGameResult 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameResult() {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onGameResult ) {
            listener->onGameResult();
        }
    }
}

/**
 * onBoostStart 이벤트 전달
 */
void GameEventDispatcher::dispatchOnBoostStart() {
    
    for( auto listener : listeners ) {
        if( listener->onBoostStart ) {
            listener->onBoostStart();
        }
    }
}

/**
 * onBoosting 이벤트 전달
 */
void GameEventDispatcher::dispatchOnBoosting() {
    
    for( auto listener : listeners ) {
        if( listener->onBoosting ) {
            listener->onBoosting();
        }
    }
}

/**
 * onBoostEnd 이벤트 전달
 */
void GameEventDispatcher::dispatchOnBoostEnd() {
    
    for( auto listener : listeners ) {
        if( listener->onBoostEnd ) {
            listener->onBoostEnd();
        }
    }
}

/**
 * onWorldChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnWorldChanged(const WorldData &world) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onWorldChanged ) {
            listener->onWorldChanged(world);
        }
    }
}

/**
 * onWorldClear 이벤트 전달
 */
void GameEventDispatcher::dispatchOnWorldClear() {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onWorldClear ) {
            listener->onWorldClear();
        }
    }
}

/**
 * onStageChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnStageChanged(const StageData &stage) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onStageChanged ) {
            listener->onStageChanged(stage);
        }
    }
}

/**
 * onStageClear 이벤트 전달
 */
void GameEventDispatcher::dispatchOnStageClear() {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onStageClear ) {
            listener->onStageClear();
        }
    }
}

/**
 * onMoveNextStage 이벤트 전달
 */
void GameEventDispatcher::dispatchOnMoveNextStage(const StageData &stage) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onMoveNextStage ) {
            listener->onMoveNextStage(stage);
        }
    }
}

/**
 * onMoveNextStageFinished 이벤트 전달
 */
void GameEventDispatcher::dispatchOnMoveNextStageFinished(const StageData &stage) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onMoveNextStageFinished ) {
            listener->onMoveNextStageFinished(stage);
        }
    }
}

/**
 * onFloorChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnFloorChanged(const FloorData &floor) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onFloorChanged ) {
            listener->onFloorChanged(floor);
        }
    }
}

/**
 * onNextFloor 이벤트 전달
 */
void GameEventDispatcher::dispatchOnNextFloor(const FloorData &floor) {
    
    auto copyListeners = listeners;
    
    for( auto listener : copyListeners ) {
        if( listener->onNextFloor ) {
            listener->onNextFloor(floor);
        }
    }
}

/**
 * onScoreChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnScoreChanged(int score) {
    
    for( auto listener : listeners ) {
        if( listener->onScoreChanged ) {
            listener->onScoreChanged(score);
        }
    }
}
