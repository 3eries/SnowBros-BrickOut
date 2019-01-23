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
    
    for( auto listener : listeners ) {
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
 * onGameResult 이벤트 전달
 */
void GameEventDispatcher::dispatchOnGameResult() {
    
    for( auto listener : listeners ) {
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
 * onStageChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnStageChanged(const StageData &stage) {
    
    for( auto listener : listeners ) {
        if( listener->onStageChanged ) {
            listener->onStageChanged(stage);
        }
    }
}

/**
 * onStageClear 이벤트 전달
 */
void GameEventDispatcher::dispatchOnStageClear() {
    
    for( auto listener : listeners ) {
        if( listener->onStageClear ) {
            listener->onStageClear();
        }
    }
}

/**
 * onNextStage 이벤트 전달
 */
void GameEventDispatcher::dispatchOnNextStage(const StageData &stage) {
    
    for( auto listener : listeners ) {
        if( listener->onNextStage ) {
            listener->onNextStage(stage);
        }
    }
}

/**
 * onFloorChanged 이벤트 전달
 */
void GameEventDispatcher::dispatchOnFloorChanged(const FloorData &floor) {
    
    for( auto listener : listeners ) {
        if( listener->onFloorChanged ) {
            listener->onFloorChanged(floor);
        }
    }
}

/**
 * onNextFloor 이벤트 전달
 */
void GameEventDispatcher::dispatchOnNextFloor(const FloorData &floor) {
    
    for( auto listener : listeners ) {
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
