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
    
Vec2 Tile::convertToTilePosition(int x, int y) {
    
    Vec2 pos(MAP_ORIGIN);
    // content size
    pos.x += x * TILE_CONTENT_WIDTH;
    pos.y += y * TILE_CONTENT_HEIGHT;
    // margin & padding
    pos.x += TILE_MARGIN_X + (x * TILE_PADDING_X);
    pos.y += TILE_MARGIN_BOTTOM + (y * TILE_PADDING_Y);
    // anchor middle
    pos.x += TILE_CONTENT_WIDTH*0.5f;
    pos.y += TILE_CONTENT_HEIGHT*0.5f;
    
    return pos;
}

Vec2 Tile::convertToTilePosition(Position tilePos) {
    return convertToTilePosition(tilePos.x, tilePos.y);
}

const float Tile::ENTER_DURATION         = 0.2f;
const float Tile::MOVE_DURATION          = 0.2f;

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
    
    initPhysics();
    
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
    
    setScale(0);
    runAction(ScaleTo::create(ENTER_DURATION, 1));
}

/**
 * 제거
 */
void Tile::removeWithAction() {
    
    // PhysicsManager::getWorld()->DestroyBody(getBody());
    // setActive(false, false);
    sleep(false);
    
    auto scale = ScaleTo::create(0.2f, 0);
    auto callFunc = CallFunc::create([=]() {
        // this->removeFromParent();
        this->setVisible(false);
        this->setNeedRemove(true);
    });
    runAction(Sequence::create(scale, callFunc, nullptr));
}

/**
 * 1칸 아래로 이동
 */
void Tile::down() {
    
    CCASSERT(tilePos.y > 0, "Tile::down error.");
    setTilePosition(Position(tilePos.x, tilePos.y-1));
}

/**
 * 타일 좌표 설정
 */
void Tile::setTilePosition(Position tilePos, bool action) {
    
    this->tilePos = tilePos;
    
    if( action ) {
        auto move = MoveTo::create(MOVE_DURATION, convertToTilePosition(tilePos));
        auto callFunc = CallFunc::create([=]() {
            this->syncNodeToBody();
        });
        runAction(Sequence::create(move, callFunc, nullptr));
        
    } else {
        setPosition(convertToTilePosition(tilePos));
        syncNodeToBody();
    }
}
    
} // namespace Game
