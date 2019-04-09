//
//  CriticalBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/04/2019.
//

#include "CriticalBall.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

#include "../TileLayer.hpp"
#include "../tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

CriticalBall* CriticalBall::create(const FriendBallDef &def) {
    
    auto ball = new CriticalBall(def);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

CriticalBall::CriticalBall(const FriendBallDef &def) : FriendBall(def) {
}

CriticalBall::~CriticalBall() {
    
}

bool CriticalBall::init() {
    
    if( !FriendBall::init() ) {
        return false;
    }
    
    return true;
}

/**
 * 이미지 초기화
 */
void CriticalBall::initImage() {
    
    FriendBall::initImage();
}

/**
 * 발사
 */
void CriticalBall::shoot(b2Vec2 velocity) {
    
    FriendBall::shoot(velocity);
}

/**
 * 추락
 */
void CriticalBall::fallToFloor() {
    
    FriendBall::fallToFloor();
}

void CriticalBall::onBeginContact(b2Contact *contact) {
    
    FriendBall::onBeginContact(contact);
}

void CriticalBall::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    FriendBall::onPreSolve(contact, oldManifold);
}

void CriticalBall::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    FriendBall::onPostSolve(contact, impulse);
}

/**
 * 볼 & 브릭 충돌
 */
bool CriticalBall::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    return FriendBall::onContactBrick(ball, brick, contactPoint);
}

/**
 * 볼 & 바닥 충돌
 */
void CriticalBall::onContactFloor(Ball *ball) {
    
    FriendBall::onContactFloor(ball);
}
