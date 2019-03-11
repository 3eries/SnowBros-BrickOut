//
//  DemoBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#include "DemoBall.hpp"

#include "Define.h"
#include "../../game/GameDefine.h"

USING_NS_CC;
using namespace std;

DemoBall* DemoBall::create(b2World *world) {
    
    auto ball = new DemoBall();
    
    if( ball && ball->init(world) ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

DemoBall::DemoBall() : SBPhysicsObject(this),
fall(false) {
}

DemoBall::~DemoBall() {
}

bool DemoBall::init(b2World *world) {
    
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
    
    // physics
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.fixedRotation = true;
    bodyDef.awake = false;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2CircleShape circle;
    circle.m_radius = PTM(BALL_RADIUS);
    
    auto body = world->CreateBody(&bodyDef);
    setBody(body);
    
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
    
    return true;
}

bool DemoBall::afterStep() {
    
    if( !SBPhysicsObject::afterStep() ) {
        return false;
    }
    
    if( !body->IsActive() || !body->IsAwake() ) {
        return true;
    }
    
    // 바디 값 동기화
    syncBodyToNode();
    
    if( !isSyncLocked() ) {
        image->setRotation(getBodyVelocityAngle());
    }
    
    return true;
}

void DemoBall::shoot(b2Vec2 velocity) {
    
    // 활성화
    setFall(false);
    awake(false);
    setCollisionLocked(false);
    setOpacity(255);
    image->setRotation(0);
    
    // 발사
    getBody()->SetLinearVelocity(velocity);
}

/**
 * 볼 & 벽돌 충돌
 */
void DemoBall::onContactBrick(Vec2 contactPoint) {
    
    // 히트 연출
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_IMG_GAME + STR_FORMAT("game_hit_%02d.png", i));
    }
    
    auto effect = SBAnimationSprite::create(anims, BALL_ANIM_HIT_INTERVAL, 1);
    effect->setAnchorPoint(ANCHOR_M);
    effect->setPosition(contactPoint);
    getParent()->addChild(effect, SBZOrder::MIDDLE);
    
    effect->runAnimation([=](Node*) {
        effect->removeFromParent();
    });
}

/**
 * 볼 & 바닥 충돌
 */
void DemoBall::onContactFloor() {
    
    // 비활성화
    setFall(true);
    sleep(false);
    setCollisionLocked(true);
    image->runAction(RotateTo::create(0.05f, 0));
}
