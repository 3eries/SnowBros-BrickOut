//
//  SplashBall.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 02/04/2019.
//

#include "SplashBall.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

#include "../TileLayer.hpp"
#include "../tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

SplashBall* SplashBall::create(const FriendBallDef &def) {
    
    auto ball = new SplashBall(def);
    
    if( ball && ball->init() ) {
        ball->autorelease();
        return ball;
    }
    
    CC_SAFE_DELETE(ball);
    return nullptr;
}

SplashBall::SplashBall(const FriendBallDef &def) : FriendBall(def) {
}

SplashBall::~SplashBall() {
    
}

bool SplashBall::init() {
    
    if( !FriendBall::init() ) {
        return false;
    }
    
    return true;
}

/**
 * 이미지 초기화
 */
void SplashBall::initImage() {
    
    FriendBall::initImage();
}

/**
 * 발사
 */
void SplashBall::shoot(b2Vec2 velocity) {
    
    FriendBall::shoot(velocity);
}

/**
 * 추락
 */
void SplashBall::fallToFloor() {
    
    FriendBall::fallToFloor();
}

/**
 * 브릭 데미지 애니메이션을 생성합니다
 */
spine::SkeletonAnimation* SplashBall::createBrickDamageAnimation(Brick *brick, Vec2 contactPoint) {
    
    auto anim = FriendBall::createBrickDamageAnimation(brick, contactPoint);
    if( !anim ) {
        return nullptr;
    }
    
    auto brickBox = SB_BOUNDING_BOX_IN_WORLD(brick);
    float diffX = brickBox.getMidX() - contactPoint.x;
    
    anim->setScaleX(diffX < 0 ? -1 : 1);
    
    return anim;
}

void SplashBall::onBeginContact(b2Contact *contact) {
    
    FriendBall::onBeginContact(contact);
}

void SplashBall::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    FriendBall::onPreSolve(contact, oldManifold);
}

void SplashBall::onPostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    FriendBall::onPostSolve(contact, impulse);
}

/**
 * 볼 & 브릭 충돌
 */
bool SplashBall::onContactBrick(Ball *ball, Game::Tile *tile, Vec2 contactPoint) {
    
    auto brick = (Brick*)tile;
    
    if( !FriendBall::onContactBrick(ball, brick, contactPoint) ) {
        return false;
    }
    
    if( !brick->canDamage() ) {
        return false;
    }
    
    vector<Brick*> contactBricks;
    contactBricks.push_back((Brick*)brick);
    
    // 충돌 지점의 타일 좌표를 구한다
    TilePosition tilePos;
    float minDist = INT_MAX;
    
    for( int row = 0; row < brick->getRows(); ++row ) {
        for( int column = 0; column < brick->getColumns(); ++column ) {
            TilePosition pos = brick->getTilePosition() + TilePosition(row, column);
            float dist = Game::Tile::convertToTilePosition(pos).getDistance(contactPoint);
            
            if( dist < minDist ) {
                minDist = dist;
                tilePos = pos;
            }
        }
    }
    
    // 스플래쉬 공격, 범위:3x2
    Vec2 offsets[] = {
        Vec2(-1,0), Vec2(1,0),
        Vec2(-1,1), Vec2(0,1), Vec2(1,1),
    };
    
    for( auto offset : offsets ) {
        auto p = tilePos + offset;
        auto nearBrick = dynamic_cast<Brick*>(tileLayer->getTile(p));
        
        if( !nearBrick ) {
            continue;
        }
        
        // 데미지 중복 방지
        if( SBCollection::contains(contactBricks, nearBrick) ) {
            continue;
        }
        
        contactBricks.push_back(nearBrick);
        
        if( nearBrick->canDamage() ) {
            nearBrick->sufferDamage(this, contactPoint, false);
            
            SBDirector::postDelayed(nearBrick, [=]() {
                if( !nearBrick->isBroken() ) {
                    nearBrick->runFriendBallDamageAction(this, contactPoint);
                }
            }, 0.1f);
        }
    }
    
    return true;
}

/**
 * 볼 & 바닥 충돌
 */
void SplashBall::onContactFloor(Ball *ball) {
    
    FriendBall::onContactFloor(ball);
}
