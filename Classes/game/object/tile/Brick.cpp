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

Brick* Brick::create(const BrickData &data, int hp) {
    
    auto brick = new Brick(data, hp);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick::Brick(const BrickData &data, int hp) : Game::Tile(data.width, data.height),
data(data),
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
    
    initPhysics();
    
    // 이미지 초기화
    image = SBAnimationSprite::create(data.idleAnims, 0.1f);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(image);
    
    image->runAnimation();
    
    // HP UI 초기화
    auto addHpGage = [=](string file) -> Sprite* {
        
        auto hpBgSize = hpNode.bg->getContentSize();
        
        hpNode.gage = Sprite::create(DIR_IMG_GAME + file);
        hpNode.gage->setAnchorPoint(ANCHOR_ML);
        hpNode.gage->setPosition(Vec2ML(hpBgSize, (hpBgSize.width-hpNode.gage->getContentSize().width)*0.5f, 0));
        hpNode.gage->setScaleX(1);
        hpNode.bg->addChild(hpNode.gage);
        
        return hpNode.gage;
    };
    
    auto addHpLabel = [=](int fontSize) -> Label* {
      
        hpNode.label = Label::createWithTTF("", FONT_COMMODORE, fontSize,
                                            Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        hpNode.label->setAnchorPoint(ANCHOR_M);
        hpNode.label->setTextColor(Color4B::WHITE);
        hpNode.label->enableOutline(Color4B::BLACK, 3);
        this->addChild(hpNode.label);
        
        return hpNode.label;
    };
    
    switch( data.type ) {
        case BrickType::NORMAL: {
            // game_gage_brick_elite_bg.png Vec2MC(0, 34) , Size(104, 16)
            // 2850 size:21 stroke:3px Vec2MC(0, 44) , Size(71, 24)
            hpNode.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_normal_bg.png");
            hpNode.bg->setAnchorPoint(ANCHOR_M);
            hpNode.bg->setPosition(Vec2MC(getContentSize(), 0, 34));
            addChild(hpNode.bg);
            
            // HP gage
            addHpGage("game_gage_brick_normal.png");
            
            // HP Label
            auto label = addHpLabel(21);
            label->setPosition(Vec2MC(getContentSize(), 0, 44));
        } break;
            
        case BrickType::ELITE: {
            hpNode.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_elite_bg.png");
            hpNode.bg->setAnchorPoint(ANCHOR_M);
            hpNode.bg->setPosition(Vec2MC(getContentSize(), 0, 34));
            addChild(hpNode.bg);
            
            // HP gage
            addHpGage("game_gage_brick_elite.png");
            
            // HP Label
            auto label = addHpLabel(21);
            label->setPosition(Vec2MC(getContentSize(), 0, 44));
        } break;
            
        case BrickType::BOSS: {
            // game_gage_brick_boss_bg.png Vec2MC(0, 93) , Size(204, 28)
            // 16850 size:28 stroke:3 Vec2MC(0, 102) , Size(112, 29)
            hpNode.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_boss_bg.png");
            hpNode.bg->setAnchorPoint(ANCHOR_M);
            hpNode.bg->setPosition(Vec2MC(getContentSize(), 0, 93));
            addChild(hpNode.bg);
            
            // HP gage
            addHpGage("game_gage_brick_boss.png");
            
            // HP Label
            auto label = addHpLabel(28);
            label->setPosition(Vec2MC(getContentSize(), 0, 102));
        } break;
            
        default:
            break;
    }
    
    // HP gage effect
    hpNode.gageEffect = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    hpNode.gageEffect->setVisible(false);
    hpNode.gageEffect->setAnchorPoint(hpNode.gage->getAnchorPoint());
    hpNode.gageEffect->setPosition(hpNode.gage->getPosition());
    hpNode.bg->addChild(hpNode.gageEffect);
    
    updateHpUI();
    
    return true;
}

/**
 * 물리 객체 초기화
 */
void Brick::initPhysics() {
    
    Size size(getPhysicsSize());
    
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
 * 등장
 */
void Brick::enterWithAction() {
    
    Game::Tile::enterWithAction();
    
    setScale(0);
    runAction(ScaleTo::create(ENTER_DURATION, 1));
}

/**
 * 제거
 */
void Brick::removeWithAction() {
    
    Game::Tile::removeWithAction();
 
    setOnBreakListener(nullptr);
    
    auto scale = ScaleTo::create(ENTER_DURATION, 0);
    auto callFunc = CallFunc::create([=]() {
        // this->removeFromParent();
        this->setVisible(false);
        this->setNeedRemove(true);
    });
    runAction(Sequence::create(scale, callFunc, nullptr));
}

/**
 * 벽돌이 깨졌습니다
 */
void Brick::onBreak() {
    
    hpNode.bg->setVisible(false);
    
    if( onBreakListener ) {
        onBreakListener(this);
    }
}

/**
 * 데미지를 입습니다
 */
void Brick::sufferDamage(int damage) {
    
    setHp(hp - damage);
    
    // 게이지 연출
    if( hpNode.gageEffect->getNumberOfRunningActions() == 0 ) {
        float scaleX = hpNode.gage->getContentSize().width / hpNode.gageEffect->getContentSize().width;
        scaleX *= getHpGageRatio();
        
        hpNode.gageEffect->setVisible(true);
        hpNode.gageEffect->setScaleX(scaleX);
        
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
