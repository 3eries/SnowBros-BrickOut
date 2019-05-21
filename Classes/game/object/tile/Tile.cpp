//
//  Tile.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "Tile.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

USING_NS_CC;
using namespace std;

NS_GAME_BEGIN

Tile::Tile(int rows, int columns, const FloorData &floorData) : SBPhysicsObject(this),
rows(rows),
columns(columns),
floorData(floorData),
available(true),
floorChangedCount(0) {
}

Tile::~Tile() {
    
}

bool Tile::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setContentSize(MEASURE_TILE_SIZE(rows, columns));
    
    return true;
}

void Tile::onEnter() {
    
    initGameListener();
    
    Node::onEnter();
}
    
void Tile::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}
    
/**
 * 물리 객체 초기화
 */
void Tile::initPhysics() {
    
}

/**
 * 게임 리스너 초기화
 */
void Tile::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onStageClear   = CC_CALLBACK_0(Tile::onStageClear, this);
    listener->onFloorChanged = CC_CALLBACK_1(Tile::onFloorChanged, this);
    listener->onNextFloor    = CC_CALLBACK_1(Tile::onNextFloor, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

void Tile::onStageClear() {
}

void Tile::onFloorChanged(const FloorData &floor) {
    
    this->floorData = floor;
}

void Tile::onNextFloor(const FloorData &floor) {
    
    floorChangedCount++;
}
    
/**
 * 타일 제거 준비
 */
void Tile::prepareRemove() {
    
    removeListeners(this);
    
    available = false;
    
    setBodyAwake(false);
    setCollisionLocked(true);
    
    b2Filter filter;
    filter.categoryBits = 0; // 0x0001;
    filter.maskBits = 0; // 0xFFFF;
    
    for( auto f = getBody()->GetFixtureList(); f; f = f->GetNext() ) {
        f->SetFilterData(filter);
    }
}

/**
 * 1칸 아래로 이동
 */
void Tile::down() {
    
    CCASSERT(tilePos.y > 0, "Tile::down error.");
    moveWithAction(TilePosition(tilePos.x, tilePos.y-1), TILE_MOVE_DURATION, CC_CALLBACK_0(Tile::onDownFinished, this));
}

void Tile::onDownFinished() {
}

/**
 * 등장 액션
 */
void Tile::enterWithAction() {
    
//    setScale(0);
//    runAction(ScaleTo::create(TILE_ENTER_DURATION, 1));
}

/**
 * 제거 액션
 */
void Tile::removeWithAction() {
    
    prepareRemove();
}

/**
 * 타일 좌표 설정
 */
/*
void Tile::setTilePosition(const TilePosition &tilePos, bool action, SBCallback onActionFinished) {
    
    CCASSERT(tilePos != INVALID_TILE_POSITION, "Tile::setTilePosition error.");
    
    this->tilePos = tilePos;
    
    Vec2 p = convertToTilePosition(tilePos, rows, columns);
    
    if( action ) {
        auto move = MoveTo::create(TILE_MOVE_DURATION, p);
        auto callFunc = CallFunc::create([=]() {
            this->syncNodeToBody();
            
            if( onActionFinished ) {
                onActionFinished();
            }
        });
        runAction(Sequence::create(move, callFunc, nullptr));
        
    } else {
        setPosition(p);
        syncNodeToBody();
    }
}
*/

void Tile::setTilePosition(const TilePosition &tilePos) {
    
    CCASSERT(tilePos != INVALID_TILE_POSITION, "Tile::setTilePosition error.");
    
    this->tilePos = tilePos;
    
    setPosition(convertToTilePosition(tilePos, rows, columns));
    syncNodeToBody();
}
 
void Tile::moveWithAction(const TilePosition &tilePos, float duration, SBCallback onActionFinished) {
    
    CCASSERT(tilePos != INVALID_TILE_POSITION, "Tile::moveWithAction error.");
    
    this->tilePos = tilePos;
    
    auto move = MoveTo::create(duration, convertToTilePosition(tilePos, rows, columns));
    auto callFunc = CallFunc::create([=]() {
        this->syncNodeToBody();
        
        if( onActionFinished ) {
            onActionFinished();
        }
    });
    runAction(Sequence::create(move, callFunc, nullptr));
}
    
/**
 * 좌표가 타일에 포함됐는지 반환합니다
 */
bool Tile::isContainsPosition(const TilePosition &p) {
    
    return p.x >= tilePos.x && p.x <= tilePos.x + (rows-1) &&
           p.y >= tilePos.y && p.y <= tilePos.y + (columns-1);
}

Vec2 Tile::convertToTilePosition(int x, int y, int w, int h) {
    
    Vec2 pos(TILE_ORIGIN);
    
    // content size
    pos.x += x * TILE_CONTENT_WIDTH;
    pos.y += y * TILE_CONTENT_HEIGHT;
    // padding
    pos.x += x * TILE_PADDING_X;
    pos.y += y * TILE_PADDING_Y;
    // anchor middle
    Size SIZE = MEASURE_TILE_SIZE(w,h);
    pos.x += SIZE.width * 0.5f;
    pos.y += SIZE.height * 0.5f;
    
    return pos;
}

Vec2 Tile::convertToTilePosition(const TilePosition &p, int w, int h) {
    return convertToTilePosition((int)p.x, (int)p.y, w, h);
}
    
NS_GAME_END
