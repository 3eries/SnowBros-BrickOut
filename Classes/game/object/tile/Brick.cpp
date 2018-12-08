//
//  Brick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "Brick.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

USING_NS_CC;
using namespace std;

Brick* Brick::create(int hp) {
    
    auto brick = new Brick(hp);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick::Brick(int hp) : Game::Tile(1,1),
originalHp(hp),
hp(hp),
onBreakListener(nullptr) {
}

Brick::~Brick() {
    
}

bool Brick::init() {
    
    if( !Game::Tile::init() ) {
        return false;
    }
    
    // 이미지 초기화
    auto n = LayerColor::create(Color4B(255, 211, 196, 255));
    n->setIgnoreAnchorPointForPosition(false);
    n->setAnchorPoint(Vec2::ZERO);
    n->setPosition(Vec2::ZERO);
    n->setContentSize(getContentSize());
    addChild(n);
    
    // n->setVisible(false);
    
    // HP 라벨 초기화
    hpLabel = Label::createWithTTF("", FONT_COMMODORE, 30);
    hpLabel->setAnchorPoint(ANCHOR_M);
    hpLabel->setPosition(Vec2MC(getContentSize(), 0, 0));
    hpLabel->setTextColor(Color4B::WHITE);
    addChild(hpLabel);
    
    updateHpUI();
    
    return true;
}

/**
 * 물리 객체 초기화
 */
void Brick::initPhysics() {
    
    Size size(getContentSize());
    
    b2BodyDef bodyDef;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2PolygonShape box;
    box.SetAsBox(PTM(size.width*0.5f), PTM(size.height*0.5f));
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::BRICK;
    filter.maskBits = PhysicsCategory::BALL;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.1f;
    fixtureDef.restitution = 1;
    fixtureDef.friction = 0;
    fixtureDef.filter = filter;
    body->CreateFixture(&fixtureDef);
}

/**
 * 벽돌이 깨졌습니다
 */
void Brick::onBreak() {
    
    if( onBreakListener ) {
        onBreakListener(this);
    }
    
    removeWithAction();
}

/**
 * 데미지를 입습니다
 */
void Brick::sufferDamage(int damage) {
    
    setHp(hp - damage);
}

/**
 * HP 설정
 */
void Brick::setHp(int hp, bool updateUI) {
    
    this->hp = MAX(0, hp);
    
    if( updateUI ) {
        updateHpUI();
    }
    
    if( isBroken() ) {
        onBreak();
    }
}

void Brick::updateHpUI() {
    
    hpLabel->setString(TO_STRING(hp));
}

bool Brick::isBroken() {
    
    return hp == 0;
}
