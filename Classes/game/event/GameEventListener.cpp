//
//  GameEventListener.cpp
//
//  Created by seongmin hwang on 29/11/2018.
//

#include "GameEventListener.hpp"

USING_NS_CC;
using namespace std;

GameEventListener* GameEventListener::create(Ref *target) {
    
    auto listener = new GameEventListener(target);
    
    if( listener ) {
        listener->autorelease();
        return listener;
    }
    
    CC_SAFE_DELETE(listener);
    return nullptr;
}

GameEventListener::GameEventListener(Ref *target) :
target(target),
onGameEnter(nullptr), onGameExit(nullptr),
onGameReset(nullptr),
onGameStart(nullptr), onGameRestart(nullptr),
onGamePause(nullptr), onGameResume(nullptr),
onGameOver(nullptr), onGameContinue(nullptr), onGameResult(nullptr),
onBoostStart(nullptr), onBoostEnd(nullptr),
onScoreChanged(nullptr), onLevelChanged(nullptr) {
}

GameEventListener::~GameEventListener() {
    
}
