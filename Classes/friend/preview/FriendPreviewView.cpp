//
//  FriendPreviewView.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/05/2019.
//

#include "FriendPreviewView.hpp"

#include "Define.h"
#include "ContentManager.hpp"
#include "GameConfiguration.hpp"

#include "PreviewBackground.hpp"
#include "PreviewBall.hpp"
#include "PreviewBrick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define             PREVIEW_FRIEND_POS_Y               100
static const string SCHEDULER_SHOOT                 = "SCHEDULER_SHOOT";
static const string SCHEDULER_SHOOT_START_DELAY     = "SCHEDULER_SHOOT_START_DELAY";

#define DEBUG_DRAW_PHYSICS      0

FriendPreviewView* FriendPreviewView::create(const FriendData &data) {
    
    auto view = new FriendPreviewView();
    
    if( view && view->init(data) ) {
        view->autorelease();
        return view;
    }
    
    CC_SAFE_DELETE(view);
    return nullptr;
}

FriendPreviewView::FriendPreviewView() :
world(nullptr) {
}

FriendPreviewView::~FriendPreviewView() {
    
    CC_SAFE_DELETE(world);
}

bool FriendPreviewView::init(const FriendData &data) {
    
    if( !Node::init() ) {
        return false;
    }
    
    this->data = data;
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    initPhysics();
    initBg();
    initFriend();
    initBall();
    initBrick();
    
    return true;
}

void FriendPreviewView::onEnter() {
    
    Node::onEnter();
    
    scheduleUpdate();
    
    // 볼 발사!
    scheduleOnce([=](float dt) {
        this->shoot();
    }, 0.5f, SCHEDULER_SHOOT_START_DELAY);
}

void FriendPreviewView::onExit() {
    
    SBPhysics::loopObjects(world, [=](SBPhysicsObject *obj) {
        obj->removeBody();
    });
    SBPhysics::removeBodies(world);
    
    Node::onExit();
}

/**
 * 볼 발사
 */
void FriendPreviewView::shoot() {
    
    shootIndex = 0;
    fallenBallCount = 0;
    
    // 발사 각도 설정
    /*
    float shootingAngle = ((arc4random() % 6) + 2) * 10; // 20 ~ 70
    shootingAngle *= (arc4random() % 2 == 0 ? 1 : -1);
    shootingAngle = MIN(SHOOTING_MAX_ANGLE, shootingAngle);
    shootingAngle = MAX(-SHOOTING_MAX_ANGLE, shootingAngle);
    */
    float shootingAngle = 0;
    
    // 속도 설정
    Vec2 endPosition = shootingPosition;
    endPosition += (Vec2::forAngle(-CC_DEGREES_TO_RADIANS(shootingAngle-90)) * MAP_DIAGONAL);

    b2Vec2 velocity = Ball::getShootingVelocity(shootingPosition, endPosition,
                                                balls[0]->getMaxVelocity());
    
    // 볼 업데이트
    for( auto ball : balls ) {
        // 발사 직후 바닥 충돌 방지를 위해 좌표 보정
        ball->setSyncLocked(true);
        ball->setBodyPosition(ball->getPosition() + Vec2(0,4));
        
        // 상태 변경
        ball->setBodyAwake(true);
        ball->setVisible(false);
    }
    
    balls[0]->setVisible(true);
    
    // 하나씩 발사
    schedule([=](float dt) {
        
        auto ball = balls[shootIndex];
        
        // 발사
        ball->setSyncLocked(false);
        ball->shoot(velocity);
        
        ++shootIndex;
        
        // 슈팅 진행중
        if( shootIndex < balls.size() ) {
            ballCountLabel->setString("X" + TO_STRING(balls.size() - shootIndex));
            // 다음볼 show
            balls[shootIndex]->setVisible(true);
        }
        // 슈팅 완료
        else {
            ballCountLabel->setVisible(false);
            this->shootStop();
        }
    }, SHOOTING_INTERVAL, SCHEDULER_SHOOT);
    
    // 프렌드 슛 연출
    auto track = friendImage->setAnimation(0, FRIEND_ANIM_NAME_SHOOT_HIGH, false);
    friendImage->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
        friendImage->setAnimation(0, FRIEND_ANIM_NAME_IDLE, true);
    });
}

void FriendPreviewView::shootStop() {
    
    unschedule(SCHEDULER_SHOOT);
    unschedule(SCHEDULER_SHOOT_START_DELAY);
}

void FriendPreviewView::onFallFinished() {
    
    ballCountLabel->setVisible(false);
    // ballCountLabel->setVisible(true);
    ballCountLabel->setPosition(Vec2(shootingPosition.x, ballCountLabel->getPositionY()));
    ballCountLabel->setString("X" + TO_STRING(balls.size()));
    
    if( !isScheduled(SCHEDULER_SHOOT_START_DELAY) ) {
        scheduleOnce([=](float dt) {
            this->shoot();
        }, 2.0f, SCHEDULER_SHOOT_START_DELAY);
    }
}

/**
 * 볼 & 바닥 충돌
 */
void FriendPreviewView::onContactFloor(Ball *ball) {
    
    ++fallenBallCount;
    
//    if( isFallFinished() && onFallFinishedListener ) {
//        onFallFinishedListener(this);
//    }
    
    ball->stopAllActions();
    ball->setPositionY(shootingPosition.y);
    
    auto fadeOut = FadeOut::create(0.2f);
    auto callFunc = CallFunc::create([=]() {
        ball->setPosition(shootingPosition);
        ball->setRotation(0);
        ball->setVisible(false);
    });
    ball->runAction(Sequence::create(fadeOut, callFunc, nullptr));
    
    // 모든 볼 추락
    if( fallenBallCount == balls.size() ) {
        SBDirector::postDelayed(this, CC_CALLBACK_0(FriendPreviewView::onFallFinished, this), BALL_JOIN_MOVE_DURATION*0.5f);
    }
}

void FriendPreviewView::onBrickBreak(Brick *brick) {
    
    brick->removeWithAction();
    shootStop();
    
    SBDirector::postDelayed(this, [=]() {
        this->initBrick();
    }, 2.0f);
    
    scheduleOnce([=](float dt) {
        this->shoot();
    }, 3.0f, SCHEDULER_SHOOT_START_DELAY);
}

void FriendPreviewView::update(float dt) {
    
    SBPhysics::loopObjects(world, [](SBPhysicsObject *obj) { obj->beforeStep(); });
    world->Step(PHYSICS_FPS, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    SBPhysics::loopObjects(world, [](SBPhysicsObject *obj) { obj->afterStep(); });
}

/**
 * 충돌 여부 설정
 */
bool FriendPreviewView::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !b2ContactFilter::ShouldCollide(fixtureA, fixtureB) ) {
        return false;
    }
    
    auto userDataA = fixtureA->GetBody()->GetUserData();
    auto userDataB = fixtureB->GetBody()->GetUserData();
    
    if( !userDataA || !userDataB ) {
        return false;
    }
    
    auto objA = (SBPhysicsObject*)userDataA;
    auto objB = (SBPhysicsObject*)userDataB;
    
    if( objA->isCollisionLocked() || objB->isCollisionLocked() ) {
        return false;
    }
    
    // 벽돌 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, fixtureA, fixtureB);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
            // 벽돌이 깨짐
            if( brick->isBroken() ) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * 충돌 시작
 */
void FriendPreviewView::BeginContact(b2Contact *contact) {
    
    // 벽 체크
    {
        PhysicsCategory wallCategorys[] = {
            PhysicsCategory::WALL_LEFT,
            PhysicsCategory::WALL_RIGHT,
            PhysicsCategory::WALL_TOP,
        };
        
        for( auto wallCategory : wallCategorys ) {
            auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, wallCategory, contact);
            
            if( objs.obj1 && objs.obj2 ) {
                auto ball = (Ball*)objs.obj1;
                ball->onContactWall(ball);
                
                return;
            }
        }
    }
    
    // 바닥 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::FLOOR, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            contact->SetEnabled(false);

            auto ball = (Ball*)objs.obj1;
            ball->onContactFloor(ball);
            
            return;
        }
    }
}

/**
 * 충돌 종료
 */
void FriendPreviewView::EndContact(b2Contact *contact) {
}

/**
 * 두 형태가 접촉을 계속하는 동안 호출됩니다.
 */
void FriendPreviewView::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    auto objA = (SBPhysicsObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    auto objB = (SBPhysicsObject*)contact->GetFixtureB()->GetBody()->GetUserData();
    
    if( objA && objA->isCollisionLocked() ) return;
    if( objB && objB->isCollisionLocked() ) return;

    for( auto ball : balls ) {
        if( ball == objA || ball == objB ) {
            ball->onPreSolve(contact, oldManifold);
        }
    }
    
    // 벽돌 체크
    /*
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            auto brick = (Brick*)objs.obj2;
            
            // 벽돌이 이미 깨진 경우, 충돌 비활성화
            if( brick->isBroken() ) {
                contact->SetEnabled(false);
            }
            
            return;
        }
    }
     */
}

/**
 * 두 형태가 접촉 후, 충돌을 통한 물리 계산을 이미 처리했을 때 호출됩니다.
 */
void FriendPreviewView::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
    
    // 벽돌 체크
    {
        auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
        
        if( objs.obj1 && objs.obj2 ) {
            auto ball = (Ball*)objs.obj1;
            auto brick = (Brick*)objs.obj2;
            
            if( !brick->isBroken() ) {
                auto p = SBPhysics::getContactPoint(contact);
                ball->onContactBrick(ball, brick, p);
                brick->onContactBrick(ball, brick, p);
            }

            return;
        }
    }
}

/**
 * 물리 세계 초기화
 */
void FriendPreviewView::initPhysics() {
    
    world = new b2World(b2Vec2(0, 0));
    world->SetContactFilter(this);
    world->SetContactListener(this);
    
    // DebugDraw
#if DEBUG_DRAW_PHYSICS
    {
        auto view = DebugDrawView::create(world);
        addChild(view, SBZOrder::MIDDLE);
        
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit;
        
        view->getDebugDraw()->SetFlags(flags);
    }
#endif
}

/**
 * 배경 초기화
 */
void FriendPreviewView::initBg() {
    
    auto mask = Sprite::create(DIR_IMG_FRIEND + "friends_preview_mask.png");
    mask->setAnchorPoint(ANCHOR_M);
    mask->setPosition(Vec2TL(216, -330));
    
    clippingNode = ClippingNode::create(mask);
    clippingNode->setAlphaThreshold(0);
    clippingNode->setAnchorPoint(Vec2::ZERO);
    clippingNode->setPosition(Vec2::ZERO);
    clippingNode->setContentSize(SB_WIN_SIZE);
    addChild(clippingNode);
    
    auto bg = PreviewBackground::create(mask->getContentSize(), world);
    bg->setAnchorPoint(mask->getAnchorPoint());
    bg->setPosition(mask->getPosition());
    clippingNode->addChild(bg);
    
    this->bg = bg;
    
    auto cover = Sprite::create(DIR_IMG_FRIEND + "friends_preview_cover.png");
    cover->setAnchorPoint(mask->getAnchorPoint());
    cover->setPosition(mask->getPosition());
    addChild(cover);
    
    /*
    // 배경
    auto bg = Sprite::create(ContentResourceHelper::getStageBackgroundFile(1));
    bg->setAnchorPoint(ANCHOR_MB);
    bg->setPosition(Vec2BC(previewLayer->getContentSize(), 0, 0));
    bg->setScale(3);
    previewLayer->addChild(bg);
    
    // 바닥
    auto bottom = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
    bottom->setAnchorPoint(ANCHOR_MB);
    bottom->setPosition(Vec2BC(previewLayer->getContentSize(), 0,0));
    bottom->setScale(0.75f);
    previewLayer->addChild(bottom);
    */
}

/**
 * 프렌드 초기화
 */
void FriendPreviewView::initFriend() {
    
    string file = ContentResourceHelper::getFriendAnimationFile(data.friendId);
    
    friendImage = spine::SkeletonAnimation::createWithJsonFile(file);
    friendImage->setAnchorPoint(Vec2::ZERO);
    friendImage->setPosition(Vec2BC(bg->getContentSize(), 0, PREVIEW_FRIEND_POS_Y));
    friendImage->setAnimation(0, FRIEND_ANIM_NAME_IDLE, true);
    bg->addChild(friendImage);
}

/**
 * 벽돌 초기화
 */
void FriendPreviewView::initBrick() {
    
    auto bgBox = SB_BOUNDING_BOX_IN_WORLD(bg);
    
    BrickDef def(Database::getFirstBrick());
    def.hp = 5;
    def.world = world;
    
    auto brick = PreviewBrick::create(def);
    brick->setOnBreakListener([=](Node*) {
        this->onBrickBreak(brick);
    });
    brick->setPosition(Vec2(bgBox.getMidX(), bgBox.getMaxY() - 70));
    brick->syncNodeToBody();
    brick->setSyncLocked(true);
    clippingNode->addChild(brick);
}

/**
 * 볼 초기화
 */
void FriendPreviewView::initBall() {
    
    auto bgBox = SB_BOUNDING_BOX_IN_WORLD(bg);
    shootingPosition = Vec2(bgBox.getMidX(), bgBox.getMinY() + 70);
    
    for( int i = 0; i < data.damage; ++i ) {
        FriendBallDef def(data);
        def.index = i;
        def.world = world;
        
        auto ball = PreviewBall::create(def);
        ball->setOnContactFloorListener(CC_CALLBACK_1(FriendPreviewView::onContactFloor, this));
        ball->setPosition(shootingPosition);
        ball->setVisible(false);
        ball->setBodyAwake(false);
        ball->setCollisionLocked(true);
        clippingNode->addChild(ball, 1);
        
        balls.push_back(ball);
    }
    
    ballCountLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    ballCountLabel->setVisible(false);
    ballCountLabel->setAnchorPoint(ANCHOR_M);
    ballCountLabel->setPosition(Vec2BC(bg->getContentSize(), 0, 25));
    ballCountLabel->setTextColor(Color4B::WHITE);
    ballCountLabel->enableOutline(Color4B::BLACK, 3);
    bg->addChild(ballCountLabel);
}
