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
    
    // HP UI 초기화
    hpNode.bg = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_bg.png");
    hpNode.bg->setAnchorPoint(ANCHOR_MT);
    hpNode.bg->setPosition(Vec2TC(getContentSize(), 0, -10));
    hpNode.bg->setScale((getContentSize().width*0.95f) / hpNode.bg->getContentSize().width);
    addChild(hpNode.bg);
    
    auto hpBgSize = hpNode.bg->getContentSize();
    
    // HP gage
    hpNode.gage = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
    hpNode.gage->setAnchorPoint(ANCHOR_ML);
    hpNode.gage->setPosition(Vec2ML(hpBgSize, (hpBgSize.width-hpNode.gage->getContentSize().width)*0.5f, 0));
    hpNode.gage->setScaleX(1);
    hpNode.bg->addChild(hpNode.gage);
    
    // HP gage effect
    hpNode.gageEffect = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    hpNode.gageEffect->setVisible(false);
    hpNode.gageEffect->setAnchorPoint(hpNode.gage->getAnchorPoint());
    hpNode.gageEffect->setPosition(hpNode.gage->getPosition());
    hpNode.bg->addChild(hpNode.gageEffect);
    
    // HP Label
    hpNode.label = Label::createWithTTF("", FONT_COMMODORE, 35,
                                        Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    hpNode.label->setAnchorPoint(ANCHOR_M);
    hpNode.label->setPosition(Vec2MC(hpBgSize, 0, 10));
    hpNode.label->setTextColor(Color4B::WHITE);
    hpNode.label->enableOutline(Color4B::BLACK, 4);
    hpNode.bg->addChild(hpNode.label);
    
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
    
    hpNode.bg->setVisible(false);
    removeWithAction();
}

/**
 * 데미지를 입습니다
 */
void Brick::sufferDamage(int damage) {
    
    setHp(hp - damage);
    
    // 게이지 연출
    if( hpNode.gageEffect->getNumberOfRunningActions() == 0 ) {
        hpNode.gageEffect->setVisible(true);
        hpNode.gageEffect->setScaleX(getHpGageRatio());
        
        auto delay = DelayTime::create(0.04f);
        auto hide = Hide::create();
        hpNode.gageEffect->runAction(Sequence::create(delay, hide, nullptr));
    }
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
    
    hpNode.gage->setScaleX(getHpGageRatio());
    hpNode.label->setString(TO_STRING(hp));
}

bool Brick::isBroken() {
    
    return hp == 0;
}

float Brick::getHpGageRatio() {
    
    return (float)hp / originalHp;
}
