//
//  GameDefine.h
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameDefine_h
#define GameDefine_h

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

/*
enum class GameState {
    NONE,
    IDLE,            // 대기
    PLAY,            // 게임 진행중, 일시정지 여부와 관계없음
    BOOSTING,        // 부스트 진행중
    GAME_OVER,       // 게임 오버
    RESULT,          // 결과 화면
};
*/

enum GameState {
    NONE            = (1 << 0),
    ENTERED         = (1 << 1),     // 게임 진입
    EXITED          = (1 << 2),     // 게임 퇴장
    STARTED         = (1 << 3),     // 게임 시작됨
    PAUSED          = (1 << 4),     // 일시정지
    IDLE            = (1 << 5),     // 대기
    BOOSTING        = (1 << 6),     // 부스트 진행중
    GAME_OVER       = (1 << 7),     // 게임 오버
    CONTINUE        = (1 << 8),     // 이어하기
    RESULT          = (1 << 9),     // 결과 화면
};

static const float          GAME_RESULT_DELAY                  = 1.0f;    // 게임 오버 -> 결과 노출까지의 지연 시간

#endif /* GameDefine_h */

