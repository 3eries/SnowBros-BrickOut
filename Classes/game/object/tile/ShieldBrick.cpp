//
//  ShieldBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 28/02/2019.
//

#include "ShieldBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"
#include "../../GameView.hpp"

#include "../Ball.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

ShieldBrick* ShieldBrick::create(const BrickDef &def) {
    
    auto brick = new ShieldBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

ShieldBrick::ShieldBrick(const BrickDef &def) : Brick(def) {
}

ShieldBrick::~ShieldBrick() {
    
}

bool ShieldBrick::init() {
    
    if( !Brick::init() ) {
        return false;
    }
    
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_IMG_GAME + STR_FORMAT("game_shield_%02d.png", i));
    }
    
    shieldAnim = SBAnimationSprite::create(anims, 0.2f);
    shieldAnim->setAnchorPoint(ANCHOR_MB);
    shieldAnim->setPosition(Vec2BC(getContentSize(), 0, 0));
    addChild(shieldAnim, 1);
    
    shieldAnim->runAnimation();
    
    return true;
}

void ShieldBrick::onEnter() {
    
    Brick::onEnter();
}

void ShieldBrick::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    if( isBroken() ) {
        Log::w("이미 깨진 벽돌 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    auto localContactPoint = convertToNodeSpace(contactPoint);
    
//    if( SB_BOUNDING_BOX_IN_WORLD(shieldAnim) ) {
//    }
    
    // 하단부 충돌, 방어
    if( localContactPoint.y <= 5 ) {
        int ranX = (arc4random() % 10) * (arc4random() % 2 == 0 ? 2 : -2);
        int ranY = (arc4random() % 10) * (arc4random() % 2 == 0 ? 2 : -2);
        
        auto effect = Label::createWithTTF("BLOCK", FONT_COMMODORE, 26, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
        effect->setAnchorPoint(ANCHOR_MB);
        // effect->setPosition(getPosition() + Vec2(ranX, 30 + ranY));
        effect->setPosition(Vec2(contactPoint.x, getPosition().y+30) + Vec2(ranX, ranY));
        effect->setTextColor(Color4B(255,255,255,255));
        effect->enableOutline(Color4B::BLACK, 3);
        GameManager::getInstance()->getView()->addChild(effect, SBZOrder::BOTTOM);
        
        auto delay = DelayTime::create(0.1f);
        auto remove = RemoveSelf::create();
        effect->runAction(Sequence::create(delay, remove, nullptr));
    }
    // 데미지 적용
    else {
        sufferDamage(ball->getDamage());
    }
}

