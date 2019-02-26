//
//  Ball.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 08/12/2018.
//

#include "Ball.hpp"

#include "Define.h"
#include "../GameDefine.h"
#include "../GameManager.hpp"
#include "../GameView.hpp"

#include "tile/Tile.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define ANIM_HIT_INTERVAL           0.03f

Ball::Ball() : SBPhysicsObject(this),
damage(1),
fall(false) {
}

Ball::~Ball() {
    
}

bool Ball::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(FIRST_SHOOTING_POSITION);
    setContentSize(BALL_SIZE);
    setCascadeOpacityEnabled(true);
    
    // 이미지 초기화
    image = Sprite::create(BALL_IMAGE);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(BALL_SIZE, 0, 0));
    SBNodeUtils::scale(image, BALL_SIZE);
    addChild(image);
    
    // 물리 객체 초기화
    setBody(createBody((SBPhysicsObject*)this));
    
    initPhysicsListener();
    
    return true;
}

void Ball::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 물리 리스너 초기화
 */
void Ball::initPhysicsListener() {
    
    auto listener = GamePhysicsListener::create();
    listener->setTarget(this);
    listener->setContactTarget(this);
    listener->onBeginContact        = CC_CALLBACK_1(Ball::onBeginContact, this);
    listener->onEndContact          = CC_CALLBACK_1(Ball::onEndContact, this);
    listener->onPreSolve            = CC_CALLBACK_2(Ball::onPreSolve, this);
    listener->onPostSolve           = CC_CALLBACK_2(Ball::onPostSolve, this);
    listener->onContactBrick        = CC_CALLBACK_3(Ball::onContactBrick, this);
    listener->onContactItem         = CC_CALLBACK_2(Ball::onContactItem, this);
    listener->onContactWall         = CC_CALLBACK_1(Ball::onContactWall, this);
    listener->onContactFloor        = CC_CALLBACK_1(Ball::onContactFloor, this);
    GameManager::getPhysicsManager()->addListener(listener);
}

b2Body* Ball::createBody(SBPhysicsObject *userData) {
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.fixedRotation = true;
    bodyDef.awake = false;
    bodyDef.userData = userData;
    
    b2CircleShape circle;
    circle.m_radius = PTM(BALL_RADIUS);
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::BALL;
    filter.maskBits = PhysicsCategory::WALL | PhysicsCategory::FLOOR | PhysicsCategory::BRICK | PhysicsCategory::ITEM;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 0.1f;      // 밀도
    fixtureDef.restitution = 1;     // 반발력 - 물체가 다른 물체에 닿았을때 팅기는 값
    fixtureDef.friction = 0;        // 마찰력
    fixtureDef.filter = filter;
    body->CreateFixture(&fixtureDef);
    
    return body;
}

bool Ball::beforeStep() {
    
//    if( isShoot ) {
//        isShoot = false;
//        getBody()->SetLinearVelocity(shootVelocity);
//    }
    
    return true;
}

bool Ball::afterStep() {
    
    if( !SBPhysicsObject::afterStep() ) {
        return false;
    }
    
    if( !body->IsActive() || !body->IsAwake() ) {
        return true;
    }
    
    // 바디 값 동기화
    syncBodyToNode();
    
    // Velocity 보정
    auto v = body->GetLinearVelocity();
    
    if( abs(v.x) > 0 && abs(v.y) > 0 ) {
        v.Normalize();
//        CCLOG("velocity1: %f,%f (%f,%f)",
//              body->GetLinearVelocity().x, body->GetLinearVelocity().y,
//              v.x, v.y);

//        float max = MAX(abs(v.x), abs(v.y));
//        float offset = 30 / max;
//        body->SetLinearVelocity(b2Vec2(v.x * offset, v.y * offset));
//
//        CCLOG("velocity2: %f,%f", body->GetLinearVelocity().x, body->GetLinearVelocity().y);
    }
    
    return true;
}

/**
 * 발사
 */
void Ball::shoot(b2Vec2 velocity) {
    
    // 활성화
    setFall(false);
    awake(false);
    setCollisionLocked(false);
    startRotate();
    setOpacity(255);
    
    // 발사
    getBody()->SetLinearVelocity(velocity);
}

/**
 * 물리 세계에서 rest 상태로 진입합니다
 */
void Ball::sleepWithAction() {
    
    sleep(false);
    
    auto fadeOut = FadeOut::create(0.1f);
    auto hide = Hide::create();
    runAction(Sequence::create(fadeOut, hide, nullptr));
}

void Ball::startRotate() {
    
    // image->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
}

void Ball::stopRotate() {

    // image->stopAllActions();
    // image->setRotation(0);
}

void Ball::onBeginContact(b2Contact *contact) {
    
    CCLOG("Ball::onBeginContact");
}

void Ball::onEndContact(b2Contact *contact) {
}

void Ball::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
}

void Ball::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    /*
    // 바닥 충돌 무시
    if( contact->GetFixtureA()->GetFilterData().categoryBits == PhysicsCategory::FLOOR ||
        contact->GetFixtureB()->GetFilterData().categoryBits == PhysicsCategory::FLOOR ) {
        return;
    }

    // 벽 충돌 무시
    if( contact->GetFixtureA()->GetFilterData().categoryBits == PhysicsCategory::WALL ||
        contact->GetFixtureB()->GetFilterData().categoryBits == PhysicsCategory::WALL ) {
        return;
    }
    
    // 히트 연출
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_IMG_GAME + STR_FORMAT("game_hit_%02d.png", i));
    }
    
    auto effect = SBAnimationSprite::create(anims, ANIM_HIT_INTERVAL, 1);
    effect->setAnchorPoint(ANCHOR_M);
    effect->setPosition(SBPhysics::getContactPoint(contact));
    GameManager::getInstance()->getView()->addChild(effect, SBZOrder::TOP);
    
    effect->runAnimation([=](Node*) {
        effect->removeFromParent();
    });
    */
}

/**
 * 볼 & 벽돌 충돌
 */
void Ball::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    // 히트 연출
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_IMG_GAME + STR_FORMAT("game_hit_%02d.png", i));
    }
    
    auto effect = SBAnimationSprite::create(anims, ANIM_HIT_INTERVAL, 1);
    effect->setAnchorPoint(ANCHOR_M);
    effect->setPosition(contactPoint);
    GameManager::getInstance()->getView()->addChild(effect, SBZOrder::TOP);
    
    effect->runAnimation([=](Node*) {
        effect->removeFromParent();
    });
}

/**
 * 볼 & 아이템 충돌
 */
void Ball::onContactItem(Ball *ball, Game::Tile *item) {
}

/**
 * 볼 & 벽 충돌
 */
void Ball::onContactWall(Ball *ball) {
}

/**
 * 볼 & 바닥 충돌
 */
void Ball::onContactFloor(Ball *ball) {
    
    CCLOG("Ball::onContactFloor");
    
    // 비활성화
    setFall(true);
    sleep(false);
    setCollisionLocked(true);
    stopRotate();
}
