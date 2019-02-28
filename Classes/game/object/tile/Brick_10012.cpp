//
//  Brick_10012.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/02/2019.
//

#include "Brick_10012.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

Brick_10012* Brick_10012::create(const BrickDef &def) {
    
    auto brick = new Brick_10012(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick_10012::Brick_10012(const BrickDef &def) : Brick(def) {
}

Brick_10012::~Brick_10012() {
    
}

bool Brick_10012::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    return true;
}

void Brick_10012::onEnter() {
    
    Brick::onEnter();
}

/**
 * 층 변경
 */
void Brick_10012::onFloorChanged(const FloorData &floor) {
    
    Brick::onFloorChanged(floor);
}

/**
 * 다음 층
 */
void Brick_10012::onNextFloor(const FloorData &floor) {

    Brick::onNextFloor(floor);
}

void Brick_10012::setTilePosition(const TilePosition &tilePos, bool action,
                                  SBCallback onActionFinished) {
    
    Brick::setTilePosition(tilePos, action, onActionFinished);
    
    // 파츠 좌표 업데이트
    /*
    auto facePart = parts[0];
    
    // 합체
    if( getFloorChangedCount() % 2 == 0 ) {
        facePart->setBgVisible(false);
        facePart->setHpVisible(false);
        facePart->setTilePosition(tilePos, action);
    }
    // 앞으로!
    else {
        facePart->setTilePosition(tilePos, , [=]() {
            facePart->setBgVisible(true);
            facePart->setHpVisible(true);
        });
    }
     */
}
