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

#include "../Ball.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static const float IDLE_ANIM_INTERVAL               = 0.6f;

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
    
    // 배경 초기화
    bg = Sprite::create();
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(getContentSize(), 0, 0));
    // bg->setColor(Color3B(0,0,0));
    // bg->setOpacity(255*0.3f);
    addChild(bg);
    
    // 이미지 초기화
    image = SBAnimationSprite::create();
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(image);
    
    setImage(ImageType::IDLE, true);
    
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
    
    // Normal
    if( !data.isBoss() ) {
        // game_gage_brick_elite_bg.png Vec2MC(0, 36) , Size(104, 16)
        hpNode.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_normal_bg.png");
        hpNode.bg->setAnchorPoint(ANCHOR_M);
        hpNode.bg->setPosition(Vec2MC(getContentSize(), 0, 36));
        addChild(hpNode.bg);
        
        // HP gage
        string gageFile = !data.isElite() ? "game_gage_brick_normal.png" : "game_gage_brick_elite.png";
        addHpGage(gageFile);
        
        // HP Label
        // 2850 size:21 stroke:3px Vec2MC(0, 46) , Size(71, 24)
        auto label = addHpLabel(21);
        label->setPosition(Vec2MC(getContentSize(), 0, 46));
    }
    // Boss
    else {
        // game_gage_brick_boss_bg.png Vec2MC(0, 92) , Size(204, 28)
        hpNode.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_boss_bg.png");
        hpNode.bg->setAnchorPoint(ANCHOR_M);
        hpNode.bg->setPosition(Vec2MC(getContentSize(), 0, 92));
        addChild(hpNode.bg);
        
        // HP gage
        addHpGage("game_gage_brick_boss.png");
        
        // HP Label
        // 16850 size:28 stroke:3 Vec2MC(0, 101) , Size(112, 29)
        auto label = addHpLabel(28);
        label->setPosition(Vec2MC(getContentSize(), 0, 101));
    }
    
    // HP gage effect
    hpNode.gageEffect = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    hpNode.gageEffect->setVisible(false);
    hpNode.gageEffect->setAnchorPoint(hpNode.gage->getAnchorPoint());
    hpNode.gageEffect->setPosition(hpNode.gage->getPosition());
    hpNode.gageEffect->setScaleY(hpNode.gage->getContentSize().height / hpNode.gageEffect->getContentSize().height);
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
    
    // 리스너 등록
    auto listener = GamePhysicsListener::create();
    listener->setTarget(this);
    listener->setContactTarget(this);
    listener->onContactBrick = CC_CALLBACK_2(Brick::onContactBrick, this);
    GameManager::getPhysicsManager()->addListener(listener);
}

/**
 * 벽돌 이미지 설정
 */
void Brick::setImage(ImageType type, bool isRunAnimation) {
    
    switch( type ) {
        case ImageType::IDLE: {
            auto anim = SBNodeUtils::createAnimation(data.idleAnims, IDLE_ANIM_INTERVAL);
            image->setAnimation(anim);
        } break;
            
        case ImageType::DAMAGE: {
            auto anim = SBNodeUtils::createAnimation(data.damageAnims, 0.1f);
            image->setAnimation(anim, 1);
        } break;
            
        default:
            CCASSERT(false, "Brick::setImage error.");
            break;
    }
    
    // image->setColor(Color3B(255, 255*0.5f, 255*0.5f));
    
    if( isRunAnimation ) {
        image->runAnimation();
    }
}

/**
 * 볼 & 벽돌 충돌
 */
void Brick::onContactBrick(Ball *ball, Game::Tile *brick) {
    
    if( isBroken() ) {
        Log::w("이미 깨진 벽돌 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    sufferDamage(ball->getDamage());
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
    
    setVisible(false);
    setNeedRemove(true);
    
    // 연출
    auto particle = ParticleSystemQuad::create(DIR_IMG_GAME + "particle_brick.plist");
    particle->setAnchorPoint(ANCHOR_M);
    particle->setPosition(getPosition());
    particle->setStartColor(Color4F(data.color));
    particle->setEndColor(Color4F(data.color));
    particle->setAutoRemoveOnFinish(true);
    getParent()->addChild(particle, SBZOrder::BOTTOM);
    
    if( data.isBoss() ) {
        particle->setScale(1.5f);
    }
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
    
    // 벽돌 데미지 연출
    setImage(ImageType::DAMAGE, false);
    
    image->runAnimation([=](Node*) {
        this->setImage(ImageType::IDLE, true);
    });
    
    // 게이지 반짝 연출
    if( hpNode.gageEffect->getNumberOfRunningActions() == 0 ) {
        float scaleX = hpNode.gage->getContentSize().width / hpNode.gageEffect->getContentSize().width;
        scaleX *= getHpRatio();
        
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
    
    const float hpRatio = getHpRatio();
    
    // 벽돌 배경
    string bgFormat = (data.type != BrickType::BOSS) ? "game_brick_monster_%02d.png" : "game_brick_boss_%02d.png";
    int bgIndex = 0;
    
    if( hpRatio >= 0.5f )        bgIndex = 1;
    else if( hpRatio >= 0.25f )  bgIndex = 2;
    else                         bgIndex = 3;
    
    bg->setTexture(DIR_IMG_GAME + STR_FORMAT(bgFormat.c_str(), bgIndex));
    
    // 게이지
    hpNode.gage->setScaleX(hpRatio);
    hpNode.label->setString(TO_STRING(hp));
}

bool Brick::isBroken() {
    
    return hp == 0;
}

float Brick::getHpRatio() {
    
    return (float)hp / originalHp;
}
