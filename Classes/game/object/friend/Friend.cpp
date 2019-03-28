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

USING_NS_CC;
using namespace std;

#define FRIEND_SIZE                     Size(63, 75)
#define BALL_POSITION                   Vec2BC(0, SHOOTING_POSITION_Y)

#define ANIM_NAME_IDLE                  "idle"
#define ANIM_NAME_STAGE_START           "stage_start"
#define ANIM_NAME_STAGE_CLEAR           "stage_clear"
#define ANIM_NAME_MOVE                  "move"
#define ANIM_NAME_DIE                   "die"
#define ANIM_NAME_SHOOT_LOW             "shoot_low"
#define ANIM_NAME_SHOOT_HIGH            "shoot_high"

static const string SCHEDULER_SHOOT   = "SCHEDULER_SHOOT";

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
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    setCascadeOpacityEnabled(true);
    
    initImage();
    initBall();
    initPhysicsListener();
    
    setPower(def.power);
    
    return true;
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
    
    addBall(data.power);
    
    // power label
    powerLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    powerLabel->setAnchorPoint(ANCHOR_M);
    powerLabel->setPosition(Vec2BC(0, 25));
    powerLabel->setTextColor(Color4B::WHITE);
    powerLabel->enableOutline(Color4B::BLACK, 3);
    addChild(powerLabel);
}

/**
 * 물리 리스너 초기화
 */
void Friend::initPhysicsListener() {
    
//    auto listener = GamePhysicsListener::create();
//    listener->setTarget(this);
//    listener->onContactBrick        = CC_CALLBACK_3(Friend::onContactBrick, this);
//    listener->onContactWall         = CC_CALLBACK_1(Friend::onContactWall, this);
//    listener->onContactFloor        = CC_CALLBACK_1(Friend::onContactFloor, this);
//    GameManager::getPhysicsManager()->addListener(listener);
}

/**
 * 볼을 생성합니다
 */
FriendBall* Friend::createBall() {
    
    return FriendBall::create(data);
}

void Friend::addBall(int count) {
    
    const int MAX_BALL_COUNT = GAME_CONFIG->getMaxBallCount();
    
    for( int i = 0; i < count && getBallCount() <= MAX_BALL_COUNT; ++i ) {
        auto ball = createBall();
        ball->setOnContactFloorListener(CC_CALLBACK_1(Friend::onContactFloor, this));
        ball->setPosition(BALL_POSITION);
        ball->setVisible(false);
        ball->setBodyAwake(false);
        ball->setCollisionLocked(true);
        addChild(ball);
        
        balls.push_back(ball);
    }
    
//    tileLayer->setBallCount((int)balls.size());
//
//    if( updateUI ) {
//        updateBallCountUI();
//    }
}

int Friend::getBallCount() {
    
    return (int)balls.size();
}

///**
// * 볼 & 벽돌 충돌
// */
//void Friend::onContactBrick(Ball *ball, Game::Tile *brickTile, Vec2 contactPoint) {
//}
//
///**
// * 볼 & 벽 충돌
// */
//void Friend::onContactWall(Ball *ball) {
//}

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
void Friend::shoot(Vec2 endPosition) {
    
    shootIndex = 0;
    fallenBallCount = 0;
    
    // 좌표 보정
    Vec2 shootingPosition = getShootingPosition();
    
    float angle = SBMath::getDegree(shootingPosition, endPosition);
    angle = MIN(SHOOTING_MAX_ANGLE, angle);
    angle = MAX(-SHOOTING_MAX_ANGLE, angle);
    
    endPosition = SBMath::getEndPosition(shootingPosition, angle, shootingPosition.getDistance(endPosition));
    
    // 속도 설정
    Vec2 diff = endPosition - shootingPosition;
    
    b2Vec2 velocity = PTM(diff);
    velocity.Normalize();
    velocity.x *= BALL_MAX_VELOCITY;
    velocity.y *= BALL_MAX_VELOCITY;
    
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
            powerLabel->setString(STR_FORMAT("X%d", getBallCount() - shootIndex));
        }
        // 슈팅 완료
        else {
            this->setPowerVisible(false);
            
            this->shootStop();
            this->onShootFinished();
        }
    }, SHOOT_INTERVAL, SCHEDULER_SHOOT);
    
    // 이미지 변경
    // 원본은 오른쪽을 본다
    ImageType shootType = fabsf(diff.x) > 150 ? ImageType::SHOOT_LOW : ImageType::SHOOT_HIGH;
    
    setImage(shootType, [=]() {
        this->setImage(Friend::ImageType::IDLE);
    });
    setImageFlippedX(diff.x < 0);
}

void Friend::shootStop() {
    
    unschedule(SCHEDULER_SHOOT);
}

/**
 * 발사 완료
 */
void Friend::onShootFinished() {
    
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
 * 발사 좌표를 반환합니다
 */
Vec2 Friend::getShootingPosition() {
    
    return Vec2(getPositionX(), SHOOTING_POSITION_Y);
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
            case ImageType::IDLE:           return image->setAnimation(0, ANIM_NAME_IDLE, true);
            case ImageType::MOVE:           return image->setAnimation(0, ANIM_NAME_MOVE, true);
            case ImageType::DIE:            return image->setAnimation(0, ANIM_NAME_DIE, false);
            case ImageType::SHOOT_LOW:      return image->setAnimation(0, ANIM_NAME_SHOOT_LOW, false);
            case ImageType::SHOOT_HIGH:     return image->setAnimation(0, ANIM_NAME_SHOOT_HIGH, false);
            case ImageType::STAGE_START:    return image->setAnimation(0, ANIM_NAME_STAGE_START, false);
            case ImageType::STAGE_CLEAR:    return image->setAnimation(0, ANIM_NAME_STAGE_CLEAR, false);
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

void Friend::setPower(int power) {
 
    power += data.power;
    power -= 1;
    
    this->power = power;
    powerLabel->setString(STR_FORMAT("X%d", power));
    
    // 단일 볼 타입
    if( data.type == FriendType::ONE_SHOT ) {
        balls[0]->setPower(power);
    }
    // 개수 증가 타입
    else {
        addBall(power - getBallCount());
    }
}

void Friend::setPowerVisible(bool isVisible) {
    
    powerLabel->setVisible(isVisible);
}
