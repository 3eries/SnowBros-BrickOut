//
//  PreviewBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 17/05/2019.
//

#include "PreviewBrick.hpp"

#include "Define.h"

#include "PreviewBall.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

PreviewBrick* PreviewBrick::create(const BrickDef &def) {
    
    auto brick = new PreviewBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

PreviewBrick::PreviewBrick(const BrickDef &def) : Brick(def) {
}

PreviewBrick::~PreviewBrick() {
    
}

bool PreviewBrick::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    return true;
}

void PreviewBrick::onEnter() {
    
    Brick::onEnter();
}

void PreviewBrick::initGameListener() {
    
    // 게임 리스너 없음
}

void PreviewBrick::initPhysicsListener() {
    
    // 물리 리스너 없음
}

bool PreviewBrick::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    return Brick::onContactBrick(ball, brick, contactPoint);
}
