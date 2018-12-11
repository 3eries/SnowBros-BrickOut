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

USING_NS_CC;
using namespace std;

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
    
    return true;
}

b2Body* Ball::createBody(SBPhysicsObject *userData) {
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.userData = userData;
    
    b2CircleShape circle;
    circle.m_radius = PTM(BALL_RADIUS);
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::BALL;
    filter.maskBits = PhysicsCategory::WALL | PhysicsCategory::FLOOR | PhysicsCategory::BRICK;
    
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
    
    setFall(false);
    setOpacity(255);
    
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
