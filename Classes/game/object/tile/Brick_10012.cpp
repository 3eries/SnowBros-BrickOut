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

Brick_10012* Brick_10012::create(const BrickData &data, int hp) {
    
    auto brick = new Brick_10012(data, hp);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick_10012::Brick_10012(const BrickData &data, int hp) : Brick(data, hp),
floorChangeCount(0) {
}

Brick_10012::~Brick_10012() {
    
}

bool Brick_10012::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    // 게임 리스너 등록
    auto listener = GameEventListener::create(this);
    listener->onFloorChanged            = CC_CALLBACK_1(Brick_10012::onFloorChanged, this);
    listener->onNextFloor               = CC_CALLBACK_1(Brick_10012::onNextFloor, this);
    GameManager::getEventDispatcher()->addListener(listener);
    
    return true;
}

void Brick_10012::onEnter() {
    
    Brick::onEnter();
}

void Brick_10012::cleanup() {
    
    removeListeners(this);
    
    Brick::cleanup();
}

/**
 * 층 변경
 */
void Brick_10012::onFloorChanged(const FloorData &floor) {
    
    ++floorChangeCount;
}

/**
 * 다음 층
 */
void Brick_10012::onNextFloor(const FloorData &floor) {

}

void Brick_10012::setTilePosition(const TilePosition &tilePos, bool action,
                                  SBCallback onActionFinished) {
    
    Brick::setTilePosition(tilePos, action, onActionFinished);
    
    // 파츠 좌표 업데이트
    /*
    auto facePart = parts[0];
    
    // 합체
    if( floorChangeCount % 2 == 0 ) {
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
