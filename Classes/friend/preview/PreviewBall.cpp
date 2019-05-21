//
//  PreviewBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 17/05/2019.
//

#include "PreviewBall.hpp"

#include "Define.h"

#include "../../game/object/tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

PreviewBall* PreviewBall::create(const FriendBallDef &def) {
    
    auto ball = new PreviewBall(def);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

PreviewBall::PreviewBall(const FriendBallDef &def) : FriendBall(def) {
}

PreviewBall::~PreviewBall() {
    
}

bool PreviewBall::init() {
    
    if( !FriendBall::init() ) {
        return false;
    }
    
    return true;
}

/**
 * 이미지 초기화
 */
void PreviewBall::initImage() {
    
    FriendBall::initImage();
}

void PreviewBall::initPhysicsListener() {
    
    // 물리 리스너 없음
}

/**
 * 발사
 */
void PreviewBall::shoot(b2Vec2 velocity) {
    
    contactBricks.clear();
    
    FriendBall::shoot(velocity);
}

float PreviewBall::getMaxVelocity() {
    
    return FriendBall::getMaxVelocity()*0.6f;
}

/**
 * 추락
 */
void PreviewBall::fallToFloor() {
    
    FriendBall::fallToFloor();
}

void PreviewBall::onBeginContact(b2Contact *contact) {
    
    FriendBall::onBeginContact(contact);
}

void PreviewBall::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    FriendBall::onPreSolve(contact, oldManifold);
    
    if( data.type == FriendType::PASS ) {
        onPassTypePreSolve(contact);
    }
}

void PreviewBall::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    FriendBall::onPostSolve(contact, impulse);
}

/**
 * 볼 & 브릭 충돌
 */
bool PreviewBall::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    return FriendBall::onContactBrick(ball, brick, contactPoint);
}

/**
 * 볼 & 바닥 충돌
 */
void PreviewBall::onContactFloor(Ball *ball) {
    
    FriendBall::onContactFloor(ball);
}

void PreviewBall::onPassTypePreSolve(b2Contact *contact) {
    
    // 천장 충돌 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::WALL_TOP, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            // 볼 추락 처리
            contact->SetEnabled(false);
            
            SBDirector::getInstance()->postDelayed(this, [=]() {
                this->onContactFloor(this);
                this->setVisible(false);
            }, 0.05f);
            
            return;
        }
    }
    
    // 브릭 충돌 체크
    auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
    
    if( !objs.obj1 || !objs.obj2 ) {
        return;
    }
    
    auto brick = (Brick*)objs.obj2;
    
    // 볼이 통과할 수 있게 해당 충돌 비활성화
    contact->SetEnabled(false);
    
    // 이미 깨진 브릭 예외 처리
    if( brick->isBroken() ) {
        return;
    }
    
    // 강제로 충돌, 이번 턴에서 이미 충돌한 브릭은 예외
    if( !SBCollection::contains(contactBricks, brick) ) {
        auto contactPoint = SBPhysics::getContactPoint(contact);
        
        onContactBrick(this, brick, contactPoint);
        brick->onContactBrick(this, brick, contactPoint);
        
        contactBricks.push_back(brick);
    }
}
