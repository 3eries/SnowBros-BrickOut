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

// Map
static const float                            MAP_CONTENT_WIDTH                = 660;
static const float                            MAP_CONTENT_HEIGHT               = 1100;
static const cocos2d::Size                    MAP_CONTENT_SIZE                 = cocos2d::Size(MAP_CONTENT_WIDTH, MAP_CONTENT_HEIGHT);

#define                                       MAP_POSITION                       Vec2MC(0, 0)
#define                                       MAP_ORIGIN                         cocos2d::Vec2(MAP_POSITION.x - MAP_CONTENT_WIDTH*0.5f, MAP_POSITION.y - MAP_CONTENT_HEIGHT*0.5f)
#define                                       MAP_BOUNDING_BOX                   cocos2d::Rect(MAP_ORIGIN, MAP_CONTENT_SIZE)

static const float                            MAP_DIAGONAL                     = MAP_ORIGIN.distance(cocos2d::Vec2(MAP_CONTENT_SIZE));  // 대각선

// Ball
static const int                              FIRST_BALL_COUNT                 = 30;        // 최초 볼 갯수
static const int                              MAX_BALL_COUNT                   = 1000;      // 최대 볼 갯수
static const float                            SHOOT_INTERVAL                   = 0.08f;     // 발사 시간 간격

static const float                            BALL_RADIUS                      = 18;
static const cocos2d::Size                    BALL_SIZE                        = cocos2d::Size(BALL_RADIUS*2, BALL_RADIUS*2);
static const float                            BALL_MAX_VELOCITY                = 30;

#define                                       SHOOTING_POSITION_Y                (MAP_ORIGIN.y+BALL_RADIUS+2)
#define                                       FIRST_SHOOTING_POSITION            Vec2BC(0, SHOOTING_POSITION_Y)

// Wall
static const float                            WALL_BORDER                      = BALL_SIZE.height * 1.3f;   // 벽 두께

// Tile
static const int                              TILE_ROWS                        = 6;     // 맵의 가로줄에 표시되는 타일 갯수
static const int                              TILE_COLUMNS                     = 9;     // 맵의 세로줄에 표시되는 타일 갯수

static const float                            TILE_MARGIN_X                    = 2;
// static const float                            TILE_MARGIN_Y                    = 2;
static const float                            TILE_MARGIN_BOTTOM               = 120;
static const float                            TILE_MARGIN_TOP                  = 2;

static const float                            TILE_PADDING_X                   = 2;
static const float                            TILE_PADDING_Y                   = 2;

static const float                            TILE_CONTENT_WIDTH               = (MAP_CONTENT_WIDTH - (TILE_MARGIN_X*2) - (TILE_PADDING_X*(TILE_ROWS-1))) / TILE_ROWS;
// static const float                            TILE_CONTENT_HEIGHT              = (MAP_CONTENT_HEIGHT - (TILE_MARGIN_Y*2) - (TILE_PADDING_Y*(TILE_COLUMNS-1))) / TILE_COLUMNS;
static const float                            TILE_CONTENT_HEIGHT              = 106;
static const cocos2d::Size                    TILE_CONTENT_SIZE                = cocos2d::Size(TILE_CONTENT_WIDTH, TILE_CONTENT_HEIGHT);

#define MEASURE_TILE_SIZE(__rows__, __columns__)        cocos2d::Size(TILE_CONTENT_SIZE.width*__rows__, TILE_CONTENT_SIZE.height*__columns__)

// Physics
static const float                            PHYSICS_FPS                      = 1 / 40.0f;

enum PhysicsCategory {
    WALL        = (1 << 0),
    FLOOR       = (1 << 1),
    BALL        = (1 << 2),
    BRICK       = (1 << 3),
    ITEM        = (1 << 4),
};

// Game Value
static const float                            GAME_RESULT_DELAY                = 1.0f;    // 게임 오버 -> 결과 노출까지의 지연 시간

#endif /* GameDefine_h */

