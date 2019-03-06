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
using namespace cocos2d::ui;
using namespace std;

Brick* Brick::create(const BrickDef &def) {
    
    auto brick = new Brick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick::Brick(const BrickDef &def) : Game::Tile(def.data.width, def.data.height, def.floorData),
data(def.data),
originalHp(def.hp),
hp(def.hp),
elite(false),
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
    bg = Sprite::create(ContentResourceHelper::getBrickBackgroundFile(getRows(), getColumns(), 1));
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
    
    return true;
}

void Brick::onEnter() {
    
    Game::Tile::onEnter();
    
    initHpGage();
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
    listener->onContactBrick = CC_CALLBACK_3(Brick::onContactBrick, this);
    GameManager::getPhysicsManager()->addListener(listener);
}

/**
 * HP 게이지 초기화
 */
void Brick::initHpGage() {

    // 1x1
    // 2x2
    // 6x1
    // 4x2
    // 1x2
    // 3x2
    // 3x1
    int idx = (getRows() == 1 && getColumns() == 1) ? 1 : 2;
    
    auto addGage = [=]() -> Sprite* {

        string file = STR_FORMAT("brick_gage_normal_%02d.png", idx);
        
        if( data.isBoss() )               file = STR_FORMAT("brick_gage_boss_%02d.png", idx);
        else if( elite )                  file = STR_FORMAT("brick_gage_elite_%02d.png", idx);
        else if( this->isInfinityHp() )   file = STR_FORMAT("brick_gage_infinity_%02d.png", idx);
        
        auto hpBgSize = hpGage.bg->getContentSize();

        hpGage.gage = Sprite::create(DIR_BRICK + file);
        hpGage.gage->setAnchorPoint(ANCHOR_ML);
        hpGage.gage->setPosition(Vec2ML(hpBgSize, (hpBgSize.width-hpGage.gage->getContentSize().width)*0.5f, 0));
        hpGage.gage->setScaleX(1);
        hpGage.bg->addChild(hpGage.gage);

        return hpGage.gage;
    };

    auto addLabel = [=](int fontSize) -> Label* {

        hpGage.label = Label::createWithTTF(TO_STRING(hp), FONT_COMMODORE, fontSize,
                                            Size::ZERO, TextHAlignment::CENTER, TextVAlignment::TOP);
        hpGage.label->setAnchorPoint(ANCHOR_MT);
        hpGage.label->setTextColor(Color4B::WHITE);
        hpGage.label->enableOutline(Color4B::BLACK, 3);
        this->addChild(hpGage.label, 1);

        return hpGage.label;
    };

    // 1
    if( idx == 1 ) {
        addLabel(21);
    }
    // 2
    else {
        addLabel(28);
    }
    
    if( isInfinityHp() ) {
        hpGage.label->setTextColor(Color4B(254,239,12,255));
        hpGage.label->setString("MAX");
    }
    
    // 1. HP Label 좌표 설정
    hpGage.label->setPosition(Vec2TC(getContentSize(), 0, 4));
    
    // 2. HP Label 상대 좌표로 게이지 설정
    hpGage.bg = Sprite::create(DIR_BRICK + STR_FORMAT("brick_gage_bg_%02d.png", idx));
    hpGage.bg->setAnchorPoint(ANCHOR_MT);
    hpGage.bg->setPosition(Vec2(getContentSize().width*0.5f,
                                hpGage.label->getPositionY() - (hpGage.label->getContentSize().height*0.5f) + 3));
    addChild(hpGage.bg);
    
    addGage();
    
//    auto addGage = [=](string file) -> Sprite* {
//
//        auto hpBgSize = hpGage.bg->getContentSize();
//
//        hpGage.gage = Sprite::create(DIR_IMG_GAME + file);
//        hpGage.gage->setAnchorPoint(ANCHOR_ML);
//        hpGage.gage->setPosition(Vec2ML(hpBgSize, (hpBgSize.width-hpGage.gage->getContentSize().width)*0.5f, 0));
//        hpGage.gage->setScaleX(1);
//        hpGage.bg->addChild(hpGage.gage);
//
//        return hpGage.gage;
//    };
//
//    auto addLabel = [=](int fontSize) -> Label* {
//
//        hpGage.label = Label::createWithTTF("", FONT_COMMODORE, fontSize,
//                                            Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
//        hpGage.label->setAnchorPoint(ANCHOR_M);
//        hpGage.label->setTextColor(!this->isInfinityHp() ? Color4B::WHITE : Color4B(254,239,12,255));
//        hpGage.label->enableOutline(Color4B::BLACK, 3);
//        this->addChild(hpGage.label);
//
//        return hpGage.label;
//    };
//
//    // Normal
//    if( !data.isBoss() ) {
//        hpGage.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_normal_bg.png");
//        hpGage.bg->setAnchorPoint(ANCHOR_M);
//        hpGage.bg->setPosition(Vec2MC(getContentSize(), 0, 36));
//        addChild(hpGage.bg);
//
//        // HP gage
//        string gageFile = "";
//
//        if( isInfinityHp() ) {
//            gageFile = "game_gage_brick_boss_blue.png";
//        } else {
//            gageFile = !elite ? "game_gage_brick_normal.png" : "game_gage_brick_elite.png";
//        }
//
//        addGage(gageFile);
//
//        // HP Label
//        auto label = addLabel(21);
//        label->setPosition(Vec2MC(getContentSize(), 0, 46));
//    }
//    // Boss
//    else {
//        hpGage.bg = Sprite::create(DIR_IMG_GAME + "game_gage_brick_boss_bg.png");
//        hpGage.bg->setAnchorPoint(ANCHOR_M);
//        hpGage.bg->setPosition(Vec2MC(getContentSize(), 0, 92));
//        addChild(hpGage.bg);
//
//        // HP gage
//        addGage("game_gage_brick_boss.png");
//
//        // HP Label
//        auto label = addLabel(28);
//        label->setPosition(Vec2MC(getContentSize(), 0, 101));
//    }
//
    // gage effect
    hpGage.gageEffect = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    hpGage.gageEffect->setVisible(false);
    hpGage.gageEffect->setAnchorPoint(hpGage.gage->getAnchorPoint());
    hpGage.gageEffect->setPosition(hpGage.gage->getPosition());
    hpGage.gageEffect->setScaleY(hpGage.gage->getContentSize().height / hpGage.gageEffect->getContentSize().height);
    hpGage.bg->addChild(hpGage.gageEffect);
}

/**
 * 벽돌 이미지 설정
 */
void Brick::setImage(ImageType type, bool isRunAnimation) {
    
    switch( type ) {
        case ImageType::IDLE: {
            auto anim = SBNodeUtils::createAnimation(data.idleAnims, data.idleAnimInterval);
            image->setAnimation(anim);
        } break;
            
        case ImageType::DAMAGE: {
            auto anim = SBNodeUtils::createAnimation(data.damageAnims, data.damageAnimInterval);
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
void Brick::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
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
    runAction(ScaleTo::create(TILE_ENTER_DURATION, 1));
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
    // particle->setScale(0.2f);
    getParent()->addChild(particle, SBZOrder::BOTTOM);
    
    if( data.isBoss() ) {
        particle->setScale(particle->getScale() * 1.5f);
    }
}

/**
 * 벽돌이 깨졌습니다
 */
void Brick::onBreak() {
    
    setHpVisible(false);
    
    if( onBreakListener ) {
        onBreakListener(this);
    }
}

/**
 * 데미지를 입습니다
 */
void Brick::sufferDamage(int damage) {
    
    setHp(hp - damage);
    
    // 브릭 애니메이션 변경
    setImage(ImageType::DAMAGE, false);
    
    image->runAnimation([=](Node*) {
        this->setImage(ImageType::IDLE, true);
    });
    
    // 흰색 브릭 반짝 연출
    auto whiteEffect = createWhiteBrickEffect();
    addChild(whiteEffect);
    
    SBDirector::postDelayed(this, [=]() {
        whiteEffect->removeFromParent();
    }, 0.03f);
    
    // 게이지 반짝 연출
    if( hpGage.gageEffect->getNumberOfRunningActions() == 0 ) {
        float scaleX = hpGage.gage->getContentSize().width / hpGage.gageEffect->getContentSize().width;
        scaleX *= getHpRatio();
        
        hpGage.gageEffect->setVisible(true);
        hpGage.gageEffect->setScaleX(scaleX);
        
        auto delay = DelayTime::create(0.04f);
        auto hide = Hide::create();
        hpGage.gageEffect->runAction(Sequence::create(delay, hide, nullptr));
    }
}

/**
 * HP 설정
 */
void Brick::setHp(int hp, bool updateUI) {
    
    if( isInfinityHp() ) {
        return;
    }
    
    this->hp = MAX(0, hp);
    
    if( updateUI ) {
        updateHpUI();
    }
    
    if( isBroken() ) {
        onBreak();
    }
}

void Brick::updateHpUI() {
    
    if( isInfinityHp() ) {
        return;
    }
    
    const float hpRatio = getHpRatio();
    
    // 벽돌 배경
    int bgStep = 0;
    
    if( hpRatio >= 0.5f )        bgStep = 1;
    else if( hpRatio >= 0.25f )  bgStep = 2;
    else                         bgStep = 3;
    
    bg->setTexture(ContentResourceHelper::getBrickBackgroundFile(getRows(), getColumns(), bgStep));
    
    // 게이지
    hpGage.gage->setScaleX(hpRatio);
    hpGage.label->setString(TO_STRING(hp));
}

void Brick::setBgVisible(bool isVisible) {
    
    bg->setVisible(isVisible);
}

void Brick::setHpVisible(bool isVisible) {
    
    hpGage.bg->setVisible(isVisible);
    hpGage.label->setVisible(isVisible);
}

bool Brick::isBoss() {
 
    return data.isBoss();
}

bool Brick::isInfinityHp() {
    
    return hp == BRICK_INFINITY_HP;
}

bool Brick::isBroken() {
    
    return hp == 0;
}

float Brick::getHpRatio() {
    
    return (float)hp / originalHp;
}

Node* Brick::createWhiteBrickEffect() {
    
    auto size = getContentSize();
    
    auto stencil = Sprite::createWithSpriteFrame(image->getSpriteFrame());
    stencil->setAnchorPoint(ANCHOR_M);
    stencil->setPosition(Vec2MC(size, 0,0));
    stencil->setFlippedX(image->isFlippedX());
    stencil->setFlippedY(image->isFlippedY());
    
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setAnchorPoint(ANCHOR_M);
    clippingNode->setPosition(Vec2MC(size, 0,0));
    clippingNode->setContentSize(size);
    clippingNode->setAlphaThreshold(0);

    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2::ZERO, stencil->getContentSize(), Color4F::WHITE);
    clippingNode->addChild(drawNode);
    
    return clippingNode;
}
