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

USING_NS_CC;
USING_NS_SB;
using namespace std;

FriendBall* FriendBall::create(const FriendData &data) {
    
    auto ball = new FriendBall(data);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

FriendBall::FriendBall(const FriendData &data) : Ball(),
data(data),
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
    
    image = Sprite::create(ContentResourceHelper::getFriendBallImageFile(data.friendId));
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(BALL_SIZE, 0, 0));
    addChild(image);
}

/**
 * 발사
 */
void FriendBall::shoot(b2Vec2 velocity) {
    
    Ball::shoot(velocity);
}

/**
 * 추락
 */
void FriendBall::fallToFloor() {
    
    Ball::fallToFloor();
}

/**
 * 히트 연출
 */
void FriendBall::runHitAction(Vec2 contactPoint) {
    
    StringList anims;
    
    for( int i = 1; i <= 4; ++i ) {
        anims.push_back(DIR_IMG_GAME + STR_FORMAT("game_hit_%02d.png", i));
    }
    
    auto effect = SBAnimationSprite::create(anims, BALL_ANIM_HIT_INTERVAL, 1);
    effect->setAnchorPoint(ANCHOR_M);
    effect->setPosition(contactPoint);
    GameManager::getInstance()->getView()->addChild(effect, SBZOrder::MIDDLE);
    
    effect->runAnimation([=](Node*) {
        effect->removeFromParent();
    });
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
 * 볼 & 벽돌 충돌
 */
void FriendBall::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    Ball::onContactBrick(ball, brick, contactPoint);
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
