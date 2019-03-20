//
//  NeutralBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/03/2019.
//

#include "NeutralBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"
#include "../../GameView.hpp"

#include "../Ball.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

NeutralBrick* NeutralBrick::create(BrickDef def) {
    
    def.hp = BRICK_INFINITY_HP;
    
    auto brick = new NeutralBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

NeutralBrick::NeutralBrick(const BrickDef &def) : Brick(def) {
}

NeutralBrick::~NeutralBrick() {
    
}

bool NeutralBrick::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    return true;
}

void NeutralBrick::onEnter() {
    
    Brick::onEnter();
}

void NeutralBrick::initBg() {
    
    // 배경 없음
}

void NeutralBrick::initHpGage() {
    
    // HP 게이지 없음
}

void NeutralBrick::runRemoveAction() {
    
    // 제거 연출 없음
}

void NeutralBrick::sufferDamage(int damage) {
    
    // 데미지 무시
}
