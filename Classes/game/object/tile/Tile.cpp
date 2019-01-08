//
//  Tile.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "Tile.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GamePhysics.hpp"

USING_NS_CC;
using namespace std;

namespace Game {

const float Tile::ENTER_DURATION         = 0.15f;
const float Tile::MOVE_DURATION          = 0.18f;

Tile::Tile(int rows, int columns) : SBPhysicsObject(this),
rows(rows),
columns(columns) {
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

/**
 * 물리 객체 초기화
 */
void Tile::initPhysics() {
    
}

/**
 * 등장
 */
void Tile::enterWithAction() {
}

/**
 * 제거
 */
void Tile::removeWithAction() {
    
    sleep(false);
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
    setTilePosition(TilePosition(tilePos.x, tilePos.y-1));
}

/**
 * 타일 좌표 설정
 */
void Tile::setTilePosition(const TilePosition &tilePos, bool action) {
    
    this->tilePos = tilePos;
    
    Vec2 p = convertToTilePosition(tilePos, rows, columns);
    
    if( action ) {
        auto move = MoveTo::create(MOVE_DURATION, p);
        auto callFunc = CallFunc::create([=]() {
            this->syncNodeToBody();
        });
        runAction(Sequence::create(move, callFunc, nullptr));
        
    } else {
        setPosition(p);
        syncNodeToBody();
    }
}
    
/**
 * 좌표가 타일에 포함됐는지 반환합니다
 */
bool Tile::isContainsPosition(const TilePosition &p) {

    return p.x >= tilePos.x && p.y >= tilePos.y &&
           p.x <= tilePos.x + (rows-1) && p.y <= tilePos.y + (columns-1);
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
    
} // namespace Game
