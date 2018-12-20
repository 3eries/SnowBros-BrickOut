//
//  GameEventListener.hpp
//
//  Created by seongmin hwang on 29/11/2018.
//

#ifndef GameEventListener_hpp
#define GameEventListener_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../GameDefine.h"
#include "../db/StageData.h"

class GameEventListener : public cocos2d::Ref {
public:
    static GameEventListener* create(cocos2d::Ref *target = nullptr);
    ~GameEventListener();
    
public:
    std::function<void()>                   onGameEnter;                // 게임 진입
    std::function<void()>                   onGameExit;                 // 게임 퇴장
    
    // 게임 리셋, 호출 시점 : 게임 시작, 이어하기
    std::function<void()>                   onGameReset;
    
    std::function<void()>                   onGameStart;                // 게임 시작
    std::function<void()>                   onGameRestart;              // 게임 재시작
    std::function<void()>                   onGamePause;                // 게임 일시정지
    std::function<void()>                   onGameResume;               // 게임 재게
    std::function<void()>                   onGameOver;                 // 게임 오버
    std::function<void()>                   onGameContinue;             // 게임 이어하기
    std::function<void()>                   onGameResult;               // 게임 결과
    
    std::function<void()>                   onBoostStart;               // 부스트 시작
    std::function<void()>                   onBoostEnd;                 // 부스트 종료
    
    std::function<void()>                   onStageClear;               // 스테이지 클리어
    std::function<void(const StageData&)>   onNextStage;                // 다음 스테이지
    
    std::function<void(const FloorData&)>   onFloorChanged;             // 층 변경
    std::function<void(const FloorData&)>   onNextFloor;                // 다음 층
    
    std::function<void(int)>                onScoreChanged;             // 스코어 변경
    
private:
    GameEventListener(cocos2d::Ref *target);
    
    CC_SYNTHESIZE(cocos2d::Ref*, target, Target);
};

#endif /* GameEventListener_hpp */
