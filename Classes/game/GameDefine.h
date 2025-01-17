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

#define                                       TILE_MOVE_DURATION                 0.18f // 타일 이동 시간

#define                                       NEXT_FLOOR_DELAY                   0.5f  // 모든 볼 추락 -> 다음 층 전환 지연 시간
#define                                       MOVE_NEXT_STAGE_DURATION           1.4f  // 다음 스테이지 이동 시간
#define                                       STAGE_CLEAR_DELAY                  2.0f  // 스테이지 클리어 지연 시간
#define                                       GAME_RESULT_DELAY                  1.5f  // 게임 오버 -> 결과 노출까지의 지연 시간

// Map
#define                                       MAP_CONTENT_WIDTH                  720
#define                                       MAP_CONTENT_HEIGHT                 1156
#define                                       MAP_CONTENT_SIZE                   cocos2d::Size(MAP_CONTENT_WIDTH, MAP_CONTENT_HEIGHT)

#define                                       MAP_ORIGIN                         (MAP_POSITION - (MAP_CONTENT_SIZE*0.5f))
#define                                       MAP_POSITION                       Vec2MC(0, -14)
#define                                       MAP_BOUNDING_BOX                   cocos2d::Rect(MAP_ORIGIN, MAP_CONTENT_SIZE)

#define                                       MAP_DIAGONAL                       MAP_ORIGIN.distance(cocos2d::Vec2(MAP_CONTENT_SIZE)) // 대각선

// Friends
#define                                       GAME_FRIENDS_POS_Y                 Vec2MC(0,-529).y
#define                                       GAME_FRIENDS_SLOT_COUNT            5

// Ball
#define                                       SHOOTING_MAX_ANGLE                 80                // 발사 최대 각도
#define                                       SHOOTING_INTERVAL                  (PHYSICS_FPS * 4) // 발사 시간 간격

static const float                            BALL_RADIUS                      = 20;
static const cocos2d::Size                    BALL_SIZE                        = cocos2d::Size(BALL_RADIUS*2, BALL_RADIUS*2);
static const float                            BALL_MAX_VELOCITY                = 28;
#define                                       BALL_ANIM_HIT_INTERVAL             0.03f

#define                                       BALL_DAMAGE_LABEL_POSITION_Y       25

#define                                       SHOOTING_POSITION_Y                64
#define                                       FIRST_SHOOTING_POSITION            Vec2BC(0, SHOOTING_POSITION_Y)

#define                                       BALL_JOIN_MOVE_DURATION            0.2f
#define                                       BALL_WITHDRAW_MOVE_DURATION        0.25f

// Wall
static const float                            WALL_BORDER                      = BALL_SIZE.height * 1.3f; // 벽 두께

// Tile
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
#define                                       PHYSICS_FPS                        (1 / 60.0f)

// velocityIterations : 바디들을 정상적으로 이동시키기 위해서 필요한 충돌들을 반복적으로 계산
// positionIterations : 조인트 분리와, 겹침현상을 줄이기 위해서 바디의 위치를 반복적으로 적용
// 값이 클수록 정확한 연산이 가능하지만 성능이 떨어진다.
// 매뉴얼상의 권장값 (VELOCITY_ITERATIONS=8, POSITION_ITERATIONS=3)
#define                                       VELOCITY_ITERATIONS                8
#define                                       POSITION_ITERATIONS                6

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

