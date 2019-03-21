//
//  DemoBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#include "DemoBall.hpp"

#include "Define.h"
#include "User.hpp"
#include "ContentResourceHelper.hpp"

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
    image = Sprite::create();
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(BALL_SIZE, 0, 0));
    addChild(image);
    
    auto updateImage = [=]() {
        image->setTexture(SELECTED_BALL_IMAGE);
    };
    
    updateImage();
    
    // 볼 스킨 선택 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_SELECTED_BALL, [=](EventCustom *event) {
        updateImage();
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
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
    
    // check movement
    float angle = getBodyVelocityAngle();
    
    const int RANGE = 5;
    angle = fabsf(angle);
    
    if( angle >= 90-RANGE && angle <= 90+RANGE ) {
        if( brickContactCount >= 5 || wallContactCount >= 2 ) {
            getBody()->ApplyForceToCenter(b2Vec2(0, angle < 90 ? 0.5f : -0.5f), false);
        }
    }
    
    return true;
}

void DemoBall::shoot(b2Vec2 velocity) {
    
    contactCount = 0;
    brickContactCount = 0;
    wallContactCount = 0;
    
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
    
    contactCount++;
    brickContactCount++;
    wallContactCount = 0;
    
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
 * 볼 & 벽 충돌
 */
void DemoBall::onContactWall() {
    
    contactCount++;
    brickContactCount = 0;
    wallContactCount++;
}

/**
 * 볼 & 바닥 충돌
 */
void DemoBall::onContactFloor() {
    
    contactCount = 0;
    brickContactCount = 0;
    wallContactCount = 0;
    
    // 비활성화
    setFall(true);
    sleep(false);
    setCollisionLocked(true);
    image->runAction(RotateTo::create(0.05f, 0));
}
