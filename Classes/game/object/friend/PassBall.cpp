//
//  PassBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 02/04/2019.
//

#include "PassBall.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

#include "../tile/Tile.hpp"
#include "../tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

PassBall* PassBall::create(const FriendBallDef &def) {
    
    auto ball = new PassBall(def);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

PassBall::PassBall(const FriendBallDef &def) : FriendBall(def) {
}

PassBall::~PassBall() {
    
}

bool PassBall::init() {
    
    if( !FriendBall::init() ) {
        return false;
    }
    
    return true;
}

/**
 * 이미지 초기화
 */
void PassBall::initImage() {
    
    FriendBall::initImage();
}

bool PassBall::afterStep() {
    
    if( !FriendBall::afterStep() ) {
        return false;
    }
    
    setRotation(0);
    
    return true;
}

/**
 * 발사
 */
void PassBall::shoot(b2Vec2 velocity) {
    
    contactBricks.clear();
    
    FriendBall::shoot(velocity);
}

/**
 * 추락
 */
void PassBall::fallToFloor() {
    
    FriendBall::fallToFloor();
}

void PassBall::onBeginContact(b2Contact *contact) {
    
    FriendBall::onBeginContact(contact);

    // contact->SetEnabled(false);
    
//    auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
//
//    if( objs.obj1 && objs.obj2 ) {
//        contact->SetEnabled(false);
//    }
}

void PassBall::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    FriendBall::onPreSolve(contact, oldManifold);

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
    
    // 이미 깨진 브릭 예외 처리
    if( brick->isBroken() ) {
        return;
    }
    
    // 볼이 통과할 수 있게 해당 충돌 비활성화
    contact->SetEnabled(false);
    
    // 강제로 충돌, 이번 턴에서 이미 충돌한 브릭은 예외
    if( !SBCollection::contains(contactBricks, brick) ) {
        auto contactPoint = SBPhysics::getContactPoint(contact);
        
        onContactBrick(this, brick, contactPoint);
        brick->onContactBrick(this, brick, contactPoint);
        
        contactBricks.push_back(brick);
    }
}

void PassBall::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    FriendBall::onPostSolve(contact, impulse);
}

/**
 * 볼 & 브릭 충돌
 */
bool PassBall::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    return FriendBall::onContactBrick(ball, brick, contactPoint);
}

/**
 * 볼 & 바닥 충돌
 */
void PassBall::onContactFloor(Ball *ball) {
    
    FriendBall::onContactFloor(ball);
}
