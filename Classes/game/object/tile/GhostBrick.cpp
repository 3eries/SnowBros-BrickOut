//
//  GhostBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 28/02/2019.
//

#include "GhostBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

GhostBrick* GhostBrick::create(const BrickDef &def) {
    
    auto brick = new GhostBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

GhostBrick::GhostBrick(const BrickDef &def) : Brick(def) {
}

GhostBrick::~GhostBrick() {
    
}

bool GhostBrick::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    return true;
}

void GhostBrick::onEnter() {
    
    Brick::onEnter();
    
    setCascadeOpacityEnabled(true);
    hpGage.bg->setCascadeOpacityEnabled(true);
}

/**
 * 층 변경
 */
void GhostBrick::onFloorChanged(const FloorData &floor) {
    
    Brick::onFloorChanged(floor);
    
    // 투명화
    if( getFloorChangedCount() % 2 == 1 ) {
        setCollisionLocked(true);
        setActive(false);
        runAction(FadeOut::create(TILE_ENTER_DURATION));
    }
    // 반투명화
    else {
        setCollisionLocked(false);
        setActive(true);
        runAction(FadeIn::create(TILE_ENTER_DURATION));
    }
}

/**
 * 다음 층
 */
void GhostBrick::onNextFloor(const FloorData &floor) {
    
    Brick::onNextFloor(floor);
}
