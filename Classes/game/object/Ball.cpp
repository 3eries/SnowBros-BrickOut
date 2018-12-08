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
damage(1) {
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
    image = Sprite::create("images/common/circle_white.png");
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(BALL_SIZE, 0, 0));
    SBNodeUtils::scale(image, BALL_SIZE);
    addChild(image);
    
    // 물리 객체 초기화
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2CircleShape circle;
    circle.m_radius = PTM(BALL_SIZE.height * 0.5f);
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
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
    
    return true;
}

/**
 * 발사
 */
void Ball::shoot(b2Vec2 velocity) {
    
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