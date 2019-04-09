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

#define NS_GAME_BEGIN         namespace Game {
#define NS_GAME_END           }
#define USING_NS_GAME         using namespace Game

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

#define                                       TILE_ENTER_DURATION                0.3f     // 타일 등장 시간
#define                                       TILE_MOVE_DURATION                 0.18f    // 타일 이동 시간

static const float                            MOVE_NEXT_STAGE_DURATION         = 1.4f;    // 다음 스테이지 이동 시간
static const float                            STAGE_CLEAR_DELAY                = 2.0f;    // 스테이지 클리어 지연 시간
static const float                            GAME_RESULT_DELAY                = 1.5f;    // 게임 오버 -> 결과 노출까지의 지연 시간

// Map
#define                                       MAP_CONTENT_WIDTH                  720
#define                                       MAP_CONTENT_HEIGHT                 1156
#define                                       MAP_CONTENT_SIZE                   cocos2d::Size(MAP_CONTENT_WIDTH, MAP_CONTENT_HEIGHT)

#define                                       MAP_ORIGIN                         (MAP_POSITION - (MAP_CONTENT_SIZE*0.5f))
#define                                       MAP_POSITION                       Vec2MC(0, -14)
#define                                       MAP_BOUNDING_BOX                   cocos2d::Rect(MAP_ORIGIN, MAP_CONTENT_SIZE)

#define                                       MAP_DIAGONAL                       MAP_ORIGIN.distance(cocos2d::Vec2(MAP_CONTENT_SIZE)) // 대각선

// Ball
#define                                       SHOOTING_MAX_ANGLE                 80                // 발사 최대 각도
#define                                       SHOOTING_INTERVAL                  (PHYSICS_FPS * 4) // 발사 시간 간격

static const float                            BALL_RADIUS                      = 20;
static const cocos2d::Size                    BALL_SIZE                        = cocos2d::Size(BALL_RADIUS*2, BALL_RADIUS*2);
static const float                            BALL_MAX_VELOCITY                = 28;
#define                                       BALL_ANIM_HIT_INTERVAL             0.03f

#define                                       SHOOTING_POSITION_Y                64
#define                                       FIRST_SHOOTING_POSITION            Vec2BC(0, SHOOTING_POSITION_Y)

#define                                       BALL_JOIN_MOVE_DURATION            0.2f
#define                                       BALL_WITHDRAW_MOVE_DURATION        0.25f

// Wall
static const float                            WALL_BORDER                      = BALL_SIZE.height * 1.3f; // 벽 두께

// Tile
typedef cocos2d::Vec2 TilePosition;
typedef std::vector<TilePosition> TilePositions;

#define                                       TILE_POSITION_MAX_Y                (GAME_CONFIG->getTileColumns()-1)

#define                                       TILE_ORIGIN                        (Vec2(60, 104) - (TILE_CONTENT_SIZE*0.5f))

static const float                            TILE_PADDING_X                   = 4;
static const float                            TILE_PADDING_Y                   = 4;

static const float                            TILE_CONTENT_WIDTH               = 116;
static const float                            TILE_CONTENT_HEIGHT              = 112;
static const cocos2d::Size                    TILE_CONTENT_SIZE                = cocos2d::Size(TILE_CONTENT_WIDTH, TILE_CONTENT_HEIGHT);

// 브릭의 충돌 영역을 눈에 보이는 것보다 넓히기 위해 별도의 PADDING 값을 정의한다
static const float                            BRICK_PHYSICS_PADDING_X          = 2;
static const float                            BRICK_PHYSICS_PADDING_Y          = 2;

#define MEASURE_TILE_SIZE(__rows__, __columns__) \
cocos2d::Size((TILE_CONTENT_SIZE.width*__rows__) + (TILE_PADDING_X*(__rows__-1)), (TILE_CONTENT_SIZE.height*__columns__) + (TILE_PADDING_Y*(__columns__-1)))

// Item
static const cocos2d::Size                    ITEM_CONTENT_SIZE                = cocos2d::Size(40, 48);

// Physics
static const float                            PHYSICS_FPS                      = 1 / 60.0f;

enum PhysicsCategory {
    WALL_LEFT   = (1 << 0),     // 왼쪽 벽
    WALL_RIGHT  = (1 << 1),     // 오른쪽 벽
    WALL_TOP    = (1 << 2),     // 천장
    FLOOR       = (1 << 3),
    BALL        = (1 << 4),
    BRICK       = (1 << 5),
    ITEM        = (1 << 6),
};

static const uint16 PHYSICS_MASK_BITS_WALL = (PhysicsCategory::BALL);
static const uint16 PHYSICS_MASK_BITS_BALL = (PhysicsCategory::WALL_LEFT | PhysicsCategory::WALL_RIGHT | PhysicsCategory::WALL_TOP |
                                              PhysicsCategory::FLOOR | PhysicsCategory::BRICK | PhysicsCategory::ITEM);

#endif /* GameDefine_h */

