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
    
    updateOpacity();
}

/**
 * 층 변경
 */
void GhostBrick::onFloorChanged(const FloorData &floor) {
    
    Brick::onFloorChanged(floor);

    updateOpacity();
}

/**
 * 다음 층
 */
void GhostBrick::onNextFloor(const FloorData &floor) {
    
    Brick::onNextFloor(floor);
}

void GhostBrick::updateOpacity() {
    
    // 투명화
    if( getFloorChangedCount() % 2 == 0 ) {
        ghostState = true;
        setCollisionLocked(true);
        setBodyActive(false);
        
        auto fadeOut = FadeTo::create(TILE_ENTER_DURATION, 255*0.3f);
        auto callFunc = CallFunc::create([=]() {
            this->setImage(BrickImageType::HIDE, true);
        });
        runAction(Sequence::create(fadeOut, callFunc, nullptr));
    }
    // 반투명화
    else {
        ghostState = false;
        setCollisionLocked(false);
        setBodyActive(true);

        auto fadeIn = FadeIn::create(TILE_ENTER_DURATION);
        auto callFunc = CallFunc::create([=]() {
            this->setImage(BrickImageType::IDLE, true);
        });
        runAction(Sequence::create(fadeIn, callFunc, nullptr));
    }
}

bool GhostBrick::canDamage() {
    
    if( !Brick::canDamage() ) {
        return false;
    }
    
    return !isGhostState();
}
