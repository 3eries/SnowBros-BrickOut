//
//  SpecialBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#include "SpecialBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

SpecialBrick::SpecialBrick(const BrickDef &def) : Brick(def),
specialState(false) {
}

SpecialBrick::~SpecialBrick() {
    
}

bool SpecialBrick::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    return true;
}

void SpecialBrick::onEnter() {
    
    Brick::onEnter();
}

/**
 * 층 변경
 */
void SpecialBrick::onFloorChanged(const FloorData &floor) {
    
    Brick::onFloorChanged(floor);
    
    if( isAvailable() ) {
        updateSpecialState();
    }
}

/**
 * 다음 층
 */
void SpecialBrick::onNextFloor(const FloorData &floor) {
    
    Brick::onNextFloor(floor);
}

void SpecialBrick::setSpecialState(bool specialState) {
    
    this->specialState = specialState;
}

void SpecialBrick::updateSpecialState() {
}

bool SpecialBrick::canDamage() {
    
    if( !Brick::canDamage() ) {
        return false;
    }
    
    return !isSpecialState();
}

