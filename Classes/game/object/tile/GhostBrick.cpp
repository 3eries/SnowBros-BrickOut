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

GhostBrick::GhostBrick(const BrickDef &def) : SpecialBrick(def) {
}

GhostBrick::~GhostBrick() {
    
}

bool GhostBrick::init() {
    
    if( !SpecialBrick::init() ) {
        return false;
    }
    
    return true;
}

void GhostBrick::onEnter() {
    
    SpecialBrick::onEnter();
    
    setCascadeOpacityEnabled(true);
    hpGage.bg->setCascadeOpacityEnabled(true);
    
    updateSpecialState();
}

/**
 * 층 변경
 */
void GhostBrick::onFloorChanged(const FloorData &floor) {
    
    SpecialBrick::onFloorChanged(floor);
}

/**
 * 다음 층
 */
void GhostBrick::onNextFloor(const FloorData &floor) {
    
    SpecialBrick::onNextFloor(floor);
}

/**
 * 상태 변경
 */
void GhostBrick::setSpecialState(bool specialState) {
    
    SpecialBrick::setSpecialState(specialState);
    
    // 투명화
    // if( getFloorChangedCount() % 2 == 0 ) {
    if( specialState ) {
        setCollisionLocked(true);
        setBodyActive(false);
        
        auto fadeOut = FadeTo::create(data.enterDuration, 255*0.3f);
        auto callFunc = CallFunc::create([=]() {
            this->setImage(BrickImageType::HIDE, true);
        });
        runAction(Sequence::create(fadeOut, callFunc, nullptr));
    }
    // 반투명화
    else {
        setCollisionLocked(false);
        setBodyActive(true);
        
        auto fadeIn = FadeIn::create(data.enterDuration);
        auto callFunc = CallFunc::create([=]() {
            this->setImage(BrickImageType::IDLE, true);
        });
        runAction(Sequence::create(fadeIn, callFunc, nullptr));
    }
}

void GhostBrick::updateSpecialState() {
    
    SpecialBrick::updateSpecialState();
    
    const int POS_Y = (int)getTilePosition().y;
    setSpecialState(POS_Y % 2 != 0);
}
