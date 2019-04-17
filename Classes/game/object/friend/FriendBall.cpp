//
//  FriendBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/03/2019.
//

#include "FriendBall.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"
#include "../../GameView.hpp"

#include "../tile/Tile.hpp"
#include "../tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

FriendBall* FriendBall::create(const FriendBallDef &def) {
    
    auto ball = new FriendBall(def);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

FriendBall::FriendBall(const FriendBallDef &def) : Ball(),
data(def.data),
ballIndex(def.index),
tileLayer(nullptr),
onContactBrickListener(nullptr),
onContactFloorListener(nullptr) {
}

FriendBall::~FriendBall() {
    
}

bool FriendBall::init() {
    
    if( !Ball::init() ) {
        return false;
    }
    
    return true;
}

/**
 * 이미지 초기화
 */
void FriendBall::initImage() {
    
    auto animFiles = ContentResourceHelper::getFriendBallAnimationFiles(data.friendId);
    
    auto anim = SBAnimationSprite::create(animFiles, 0.06f);
    anim->setAnchorPoint(ANCHOR_M);
    anim->setPosition(Vec2MC(BALL_SIZE, 0, 0));
    anim->runAnimation();
    addChild(anim);
    
    this->image = anim;
}

/**
 * 발사
 */
void FriendBall::shoot(b2Vec2 velocity) {
    
    Ball::shoot(velocity);
}

float FriendBall::getMaxVelocity() {
    
    return BALL_MAX_VELOCITY * data.ballSpeed;
}

/**
 * 추락
 */
void FriendBall::fallToFloor() {
    
    Ball::fallToFloor();
}

/**
 * 볼 히트 애니메이션을 생성합니다
 */
SBAnimationSprite* FriendBall::createHitAnimation() {
    
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_CONTENT_FRIEND + STR_FORMAT("friend_ball_hit_%02d.png", i));
    }
    
    return SBAnimationSprite::create(anims, BALL_ANIM_HIT_INTERVAL, 1);
}

/**
 * 브릭 데미지 애니메이션을 생성합니다
 */
spine::SkeletonAnimation* FriendBall::createBrickDamageAnimation(Brick *brick, Vec2 contactPoint) {
    
    return ContentResourceHelper::createFriendBrickDamageAnimation(data.friendId, brick->getData());
}

void FriendBall::onBeginContact(b2Contact *contact) {
    
    Ball::onBeginContact(contact);
}

void FriendBall::onEndContact(b2Contact *contact) {
    
    Ball::onEndContact(contact);
}

void FriendBall::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    Ball::onPreSolve(contact, oldManifold);
}

void FriendBall::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    Ball::onPostSolve(contact, impulse);
}

/**
 * 볼 & 브릭 충돌
 */
bool FriendBall::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    if( !Ball::onContactBrick(ball, brick, contactPoint) ) {
        return false;
    }
    
    if( onContactBrickListener ) {
        onContactBrickListener(this, brick);
    }
    
    // 브릭 충돌 후 제거
    if( data.isRemoveAfterBrickContact ) {
        onContactFloor(this);
        setVisible(false);
    }
    
    return true;
}

/**
 * 볼 & 아이템 충돌
 */
void FriendBall::onContactItem(Ball *ball, Game::Tile *item) {
    
    Ball::onContactItem(ball, item);
}

/**
 * 볼 & 벽 충돌
 */
void FriendBall::onContactWall(Ball *ball) {
    
    Ball::onContactWall(ball);
}

/**
 * 볼 & 바닥 충돌
 */
void FriendBall::onContactFloor(Ball *ball) {
    
    Ball::onContactFloor(ball);
    
    if( onContactFloorListener ) {
        onContactFloorListener(this);
    }
}
