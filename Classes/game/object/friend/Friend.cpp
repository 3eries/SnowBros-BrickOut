//
//  Friend.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Friend.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "ContentResourceHelper.hpp"

#include "../../GameManager.hpp"
#include "../../GameDefine.h"
#include "../TileLayer.hpp"

#include "CriticalBall.hpp"
#include "SplashBall.hpp"
#include "PassBall.hpp"

USING_NS_CC;
using namespace std;

#define             BALL_POSITION             Vec2BC(0, SHOOTING_POSITION_Y)

static const string SCHEDULER_SHOOT         = "SCHEDULER_SHOOT";
static const string SCHEDULER_SHOOT_DELAY   = "SCHEDULER_SHOOT_DELAY";

Friend* Friend::create(const FriendDef &def) {
    
    auto friendNode = new Friend();
    
    if( friendNode && friendNode->init(def) ) {
        friendNode->autorelease();
        return friendNode;
    }
    
    CC_SAFE_DELETE(friendNode);
    return nullptr;
}

Friend::Friend() :
onFallFinishedListener(nullptr),
tileLayer(nullptr),
shootIndex(0),
fallenBallCount(0) {
}

Friend::~Friend() {
    
}

bool Friend::init(const FriendDef &def) {
    
    if( !Node::init() ) {
        return false;
    }
    
    this->data = def.data;
    this->tileLayer = def.tileLayer;
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2(0, GAME_FRIENDS_POS_Y));
    setContentSize(SB_WIN_SIZE);
    setCascadeOpacityEnabled(true);
    
    initImage();
    initBall();
    
    setDamage(def.damage);
    
    return true;
}

void Friend::cleanup() {
    
    removeBalls();
    
    Node::cleanup();
}

void Friend::initImage() {
    
    string file = ContentResourceHelper::getFriendAnimationFile(data.friendId);
    
    image = spine::SkeletonAnimation::createWithJsonFile(file);
    image->setAnchorPoint(Vec2::ZERO);
    image->setPosition(Vec2MC(0, 0));
    addChild(image, 1);
    
    SBSpineHelper::clearAnimation(image, ANIM_NAME_CLEAR);
}

void Friend::initBall() {
    
    addBall(1);
    
    damageLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    damageLabel->setAnchorPoint(ANCHOR_M);
    damageLabel->setPosition(Vec2(Vec2BC(0,0).x,
                                  convertToNodeSpace(Vec2(0, BALL_DAMAGE_LABEL_POSITION_Y)).y));
    damageLabel->setTextColor(Color4B::WHITE);
    damageLabel->enableOutline(Color4B::BLACK, 3);
    addChild(damageLabel);
}

/**
 * 볼을 생성합니다
 */
FriendBall* Friend::createBall() {
    
    FriendBallDef def(data);
    def.index = getBallCount();
    def.world = GameManager::getPhysicsManager()->getWorld();
    
    if( data.type == FriendType::CRITICAL )     return CriticalBall::create(def);
    if( data.type == FriendType::SPLASH )       return SplashBall::create(def);
    if( data.type == FriendType::PASS )         return PassBall::create(def);
    
    return FriendBall::create(def);
}

void Friend::addBall(int count) {
    
    const int MAX_BALL_COUNT = GAME_CONFIG->getMaxBallCount();
    
    for( int i = 0; i < count && getBallCount() <= MAX_BALL_COUNT; ++i ) {
        auto ball = createBall();
        ball->setTileLayer(tileLayer);
        ball->setOnContactBrickListener(CC_CALLBACK_2(Friend::onContactBrick, this));
        ball->setOnContactFloorListener(CC_CALLBACK_1(Friend::onContactFloor, this));
        ball->setPosition(BALL_POSITION);
        ball->setVisible(false);
        ball->setBodyAwake(false);
        ball->setCollisionLocked(true);
        addChild(ball);
        
        balls.push_back(ball);
    }
}

void Friend::removeBalls() {
    
    for( auto ball : balls ) {
        ball->removeBody();
        ball->removeFromParent();
    }
    
    balls.clear();
}

int Friend::getBallCount() {
    
    return (int)balls.size();
}

/**
 * 볼 & 브릭 충돌
 */
void Friend::onContactBrick(FriendBall *ball, Game::Tile *brick) {
}

/**
 * 볼 & 바닥 충돌
 */
void Friend::onContactFloor(FriendBall *ball) {
    
    ++fallenBallCount;
    
    if( isFallFinished() && onFallFinishedListener ) {
        onFallFinishedListener(this);
    }
    
    ball->stopAllActions();
    ball->setPositionY(BALL_POSITION.y);
    
    auto fadeOut = FadeOut::create(0.2f);
    auto callFunc = CallFunc::create([=]() {
        ball->setPosition(BALL_POSITION);
        ball->setRotation(0);
        ball->setVisible(false);
    });
    ball->runAction(Sequence::create(fadeOut, callFunc, nullptr));
}

/**
 * 볼 발사
 */
void Friend::shoot(float delay) {
    
    shootIndex = 0;
    fallenBallCount = 0;
    
    if( delay > 0 ) {
        scheduleOnce([=](float dt) {
            this->shoot(0);
        }, delay, SCHEDULER_SHOOT_DELAY);
        
        return;
    }
    
    // 타겟 브릭이 없으면 발사하지 않음
    if( tileLayer->getCanDamageBricks().size() == 0 ) {
        fallenBallCount = getBallCount();
        
        if( onFallFinishedListener ) {
            onFallFinishedListener(this);
        }
        
        return;
    }
    
    // 좌표 보정
    Vec2 shootingPosition = getShootingPosition();
    Vec2 targetPosition = getShootingTargetPosition();
    
    float angle = SBMath::getDegree(shootingPosition, targetPosition);
    angle = MIN(SHOOTING_MAX_ANGLE, angle);
    angle = MAX(-SHOOTING_MAX_ANGLE, angle);
    
    targetPosition = SBMath::getEndPosition(shootingPosition, angle, shootingPosition.getDistance(targetPosition));
    
    // 속도 설정
    b2Vec2 velocity = Ball::getShootingVelocity(shootingPosition, targetPosition,
                                                getBallMaxVelocity());
    Vec2 diff = targetPosition - shootingPosition;
    
    // 볼 업데이트
    for( auto ball : balls ) {
        // 발사 직후 바닥 충돌 방지를 위해 좌표 보정
        ball->setSyncLocked(true);
        ball->setBodyPosition(shootingPosition + Vec2(0,4));
        
        // 상태 변경
        ball->setBodyAwake(true);
        ball->setVisible(false);
    }
    
    // 하나씩 발사
    schedule([=](float dt) {
        
        // 볼 회수됨
        /*
        if( isWithdraw ) {
            this->shootStop();
            return;
        }
        */
        
        auto ball = balls[shootIndex];
        
        // 발사
        ball->setSyncLocked(false);
        ball->setVisible(true);
        ball->shoot(velocity);
        
        ++shootIndex;
        
        // 슈팅 진행중
        if( shootIndex < getBallCount() ) {
            damageLabel->setString(STR_FORMAT("X%d", getBallCount() - shootIndex));
        }
        // 슈팅 완료
        else {
            this->setDamageVisible(false);
            
            this->shootStop();
            this->onShootFinished();
        }
    }, SHOOTING_INTERVAL * data.shootingInterval, SCHEDULER_SHOOT);
    
    // 이미지 변경
    ImageType shootType = fabsf(diff.x) > 150 ? ImageType::SHOOT_LOW : ImageType::SHOOT_HIGH;
    
    setImage(shootType, [=]() {
        this->setImage(Friend::ImageType::IDLE);
    });
    
    // 좌우 반전, 원본은 오른쪽을 본다
    setImageFlippedX(diff.x < 0);
}

void Friend::shootStop() {
    
    unschedule(SCHEDULER_SHOOT_DELAY);
    unschedule(SCHEDULER_SHOOT);
}

/**
 * 발사 완료
 */
void Friend::onShootFinished() {
    
}

/**
 * 발사 좌표를 반환합니다
 */
Vec2 Friend::getShootingPosition() {
    
    return Vec2(getPositionX(), SHOOTING_POSITION_Y);
}

/**
 * 발사 대상 좌표를 반환합니다
 */
Vec2 Friend::getShootingTargetPosition() {
    
    // 패스 타입은 최저 각도로 발사
//    if( data.type == FriendType::PASS ) {
//        float angle = arc4random() % 2 == 0 ? SHOOTING_MAX_ANGLE : -SHOOTING_MAX_ANGLE;
//        auto pos = SBMath::getEndPosition(getShootingPosition(), angle, MAP_CONTENT_WIDTH);
//
//        return pos;
//    }
    
    Brick *target = nullptr;
    
    for( int y = 1; y < GAME_CONFIG->getTileColumns(); ++y ) {
        auto bricks = tileLayer->getTiles<Brick*>(y);
        
        SBCollection::remove(bricks, [](Brick *b) -> bool {
            
            // 무한 HP 예외처리
            if( b->isInfinityHp() ) {
                return true;
            }
            
            return false;
        });
        
        if( bricks.size() > 0 ) {
            target = bricks[arc4random() % bricks.size()];
            break;
        }
    }
    
    // 타겟 없음, 발생할 수 없는 상황이지만 중앙에 쏩시다
    if( !target ) {
        return Vec2MC(0,0);
    }
    
    auto box = SB_BOUNDING_BOX_IN_WORLD(target);
    return Vec2(box.getMidX(), box.getMinY());
}

/**
 * 볼 회수
 */
void Friend::withdrawBall() {
    
    shootStop();
    
    for( int i = 0; i < getBallCount(); ++i ) {
        auto ball = balls[i];
        
        if( ball->isFall() ) {
            continue;
        }
        
        ball->fallToFloor();
        ball->stopAllActions();
        
        auto fadeOut = FadeOut::create(0.2f);
        auto callFunc = CallFunc::create([=]() {
            ball->setPosition(BALL_POSITION);
            ball->setRotation(0);
            ball->setVisible(false);
        });
        ball->runAction(Sequence::create(fadeOut, callFunc, nullptr));
    }
}

/**
 * 모든 볼이 추락했는지를 반환합니다
 */
bool Friend::isFallFinished() {
    
    return fallenBallCount == getBallCount();
}

/**
 * 이미지를 변경합니다
 */
void Friend::setImage(ImageType type, SBCallback onAnimationFinished) {
    
    // SBSpineHelper::clearAnimation(image, ANIM_NAME_CLEAR);
//    image->clearTracks();
//    image->update(0);

    auto setAnimation = [=]() ->spTrackEntry* {
        switch( type ) {
            case ImageType::IDLE:           return image->setAnimation(0, FRIEND_ANIM_NAME_IDLE, true);
            case ImageType::MOVE:           return image->setAnimation(0, FRIEND_ANIM_NAME_MOVE, true);
            case ImageType::DIE:            return image->setAnimation(0, FRIEND_ANIM_NAME_DIE, false);
            case ImageType::SHOOT_LOW:      return image->setAnimation(0, FRIEND_ANIM_NAME_SHOOT_LOW, false);
            case ImageType::SHOOT_HIGH:     return image->setAnimation(0, FRIEND_ANIM_NAME_SHOOT_HIGH, false);
            case ImageType::STAGE_START:    return image->setAnimation(0, FRIEND_ANIM_NAME_STAGE_START, false);
            case ImageType::STAGE_CLEAR:    return image->setAnimation(0, FRIEND_ANIM_NAME_STAGE_CLEAR, false);
            default:
                CCASSERT(false, "Friend::setImage error: invalid image type.");
                break;
        }
    };
    
    auto track = setAnimation();
    
    if( track && onAnimationFinished ) {
        image->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
            onAnimationFinished();
        });
    }
}

void Friend::setImageFlippedX(bool flippedX) {
    
    image->setScaleX(flippedX ? -1 : 1);
}

void Friend::setDamage(int damage) {
    
    damage += data.damage;
    damage -= 1;
    
    this->damage = damage;
    damageLabel->setString(STR_FORMAT("X%d", damage));
    
    // 단일 볼, 데미지 증가
    if( data.isSingleBall ) {
        balls[0]->setDamage(damage);
    }
    // 개수 증가
    else {
        addBall(damage - getBallCount());
    }
}

void Friend::setDamageVisible(bool isVisible) {
    
    damageLabel->setVisible(isVisible);
}

float Friend::getBallMaxVelocity() {
    
    return balls[0]->getMaxVelocity();
}

/**
 * 슬롯 인덱스에 해당하는 프렌즈 좌표를 반환합니다
 */
Vec2 Friend::getSlotPosition(int slotIndex) {
 
    int w = SB_WIN_SIZE.width / GAME_FRIENDS_SLOT_COUNT;
    return Vec2((w*slotIndex) + (w*0.5f), GAME_FRIENDS_POS_Y);
}
