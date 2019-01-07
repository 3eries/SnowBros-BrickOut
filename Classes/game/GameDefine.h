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

static const float                            STAGE_CLEAR_DELAY                = 0.8f;
static const float                            GAME_RESULT_DELAY                = 0.4f;    // 게임 오버 -> 결과 노출까지의 지연 시간

// Map
#define                                       MAP_CONTENT_WIDTH                  720
#define                                       MAP_CONTENT_HEIGHT                 1156
#define                                       MAP_CONTENT_SIZE                   cocos2d::Size(MAP_CONTENT_WIDTH, MAP_CONTENT_HEIGHT)

#define                                       MAP_ORIGIN                         Vec2(0, SHOOTING_POSITION_Y-BALL_RADIUS-2)
#define                                       MAP_POSITION                       (MAP_ORIGIN + (MAP_CONTENT_SIZE*0.5f)) // Vec2MC(0, -11)
#define                                       MAP_BOUNDING_BOX                   cocos2d::Rect(MAP_ORIGIN, MAP_CONTENT_SIZE)

#define                                       MAP_DIAGONAL                       MAP_ORIGIN.distance(cocos2d::Vec2(MAP_CONTENT_SIZE))  // 대각선

// Ball
static const float                            SHOOT_INTERVAL                   = 0.08f;     // 발사 시간 간격

#define                                       BALL_IMAGE                         std::string(DIR_IMG_GAME + "game_ball.png")

static const float                            BALL_RADIUS                      = 20;
static const cocos2d::Size                    BALL_SIZE                        = cocos2d::Size(BALL_RADIUS*2, BALL_RADIUS*2);
static const float                            BALL_MAX_VELOCITY                = 32;

#define                                       SHOOTING_POSITION_Y                64
#define                                       FIRST_SHOOTING_POSITION            Vec2BC(0, SHOOTING_POSITION_Y)

// Wall
static const float                            WALL_BORDER                      = BALL_SIZE.height * 1.3f;   // 벽 두께

// Tile
#define                                       TILE_POSITION_MAX_Y                (GameManager::getConfig()->getTileColumns()-1)

#define                                       TILE_ORIGIN                        (Vec2(60, 104) - (TILE_CONTENT_SIZE*0.5f))

static const float                            TILE_PADDING_X                   = 4;
static const float                            TILE_PADDING_Y                   = 4;

static const float                            TILE_CONTENT_WIDTH               = 116;
static const float                            TILE_CONTENT_HEIGHT              = 112;
static const cocos2d::Size                    TILE_CONTENT_SIZE                = cocos2d::Size(TILE_CONTENT_WIDTH, TILE_CONTENT_HEIGHT);

#define MEASURE_TILE_SIZE(__rows__, __columns__) \
cocos2d::Size((TILE_CONTENT_SIZE.width*__rows__) + (TILE_PADDING_X*(__rows__-1)), (TILE_CONTENT_SIZE.height*__columns__) + (TILE_PADDING_Y*(__columns__-1)))

// Item
static const cocos2d::Size                    ITEM_CONTENT_SIZE                = cocos2d::Size(40, 48);

// Physics
static const float                            PHYSICS_FPS                      = 1 / 50.0f;

enum PhysicsCategory {
    WALL        = (1 << 0),
    FLOOR       = (1 << 1),
    BALL        = (1 << 2),
    BRICK       = (1 << 3),
    ITEM        = (1 << 4),
};

#endif /* GameDefine_h */

