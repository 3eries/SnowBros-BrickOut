//
//  TornadoBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/04/2019.
//

#include "TornadoBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"

#include "../Ball.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define ANIM_SLOW_INTERVAL                       0.12f
#define ANIM_FAST_INTERVAL                       0.06f

#define ANIM_SLOW_DURATION(__frames__)           ((__frames__) * ANIM_SLOW_INTERVAL)
#define ANIM_FAST_DURATION(__frames__)           ((__frames__) * ANIM_FAST_INTERVAL)

static const string SCHEDULER_CHECK_COLLISION = "SCHEDULER_CHECK_COLLISION";

TornadoBrick* TornadoBrick::create(const BrickDef &def) {
    
    auto brick = new TornadoBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

TornadoBrick::TornadoBrick(const BrickDef &def) : SpecialBrick(def) {
}

TornadoBrick::~TornadoBrick() {
    
}

bool TornadoBrick::init() {
    
    if( !SpecialBrick::init() ) {
        return false;
    }
    
    return true;
}

void TornadoBrick::onEnter() {
    
    SpecialBrick::onEnter();
    
    hpGage.bg->setCascadeOpacityEnabled(true);
    
    updateSpecialState();
}

/**
 * 층 변경
 */
void TornadoBrick::onFloorChanged(const FloorData &floor) {
    
    SpecialBrick::onFloorChanged(floor);
}

/**
 * 다음 층
 */
void TornadoBrick::onNextFloor(const FloorData &floor) {
    
    SpecialBrick::onNextFloor(floor);
}

void TornadoBrick::onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    
    SpecialBrick::onPreSolve(contact, oldManifold);
    
    if( !isSpecialState() ) {
        return;
    }
    
    // 브릭 충돌 체크
    auto objs = SBPhysics::findCollisionObjects(PhysicsCategory::BALL, PhysicsCategory::BRICK, contact);
    
    if( !objs.obj1 || !objs.obj2 ) {
        return;
    }
    
    if( objs.obj2 != this ) {
        CCASSERT(false, "TornadoBrick::onPreSolve error.");
    }
    
    // 볼이 통과할 수 있게 해당 충돌 비활성화
    contact->SetEnabled(false);
}

/**
 * 충돌 체크
 */
void TornadoBrick::checkCollision(float dt) {
    
    auto world = GameManager::getPhysicsManager()->getWorld();
    auto brickOriginBox = SBNodeUtils::getBoundingBoxInWorld(this);
    auto brickCollisionBox = SBNodeUtils::getBoundingBoxInWorld(this, 0.9f, 0.5f);
    
    SBPhysics::loopObjects(world, [=](SBPhysicsObject *obj) {
        
        auto ball = dynamic_cast<Ball*>(obj);
        
        if( !ball || !ball->isBodyActive() || !ball->isBodyAwake() || ball->isCollisionLocked() ) {
            return;
        }
        
        auto ballBox = SB_BOUNDING_BOX_IN_WORLD(ball);
        auto ballCenter = Vec2(ballBox.getMidX(), ballBox.getMidY());
        
        // 충돌 체크
        if( !brickCollisionBox.intersectsCircle(ballCenter, BALL_RADIUS) ) {
            // 이미 접촉한 볼이 영역을 벗어나면 리스트에서 제거
            if( !brickOriginBox.intersectsRect(ballBox) ) {
                auto it = std::find(contactBalls.begin(), contactBalls.end(), ball);
                if( it != contactBalls.end() ) {
                    contactBalls.erase(it);
                }
            }
            
            return;
        }
        
        // 이미 접촉한 볼은 영역을 벗어날 때까지 무시
        if( SBCollection::contains(contactBalls, ball) ) {
            return;
        }
        
        // 난반사
        float angle = SBMath::random(0,90) * (arc4random() % 2 == 0 ? 1 : -1); // 0~90도 사이의 랜덤 각도
        Vec2  endPos = SBMath::getEndPosition(ballCenter, angle, MAP_DIAGONAL);
        
        b2Vec2 velocity = Ball::getShootingVelocity(ballCenter, endPos, ball->getMaxVelocity());
        ball->getBody()->SetLinearVelocity(velocity);
        
        contactBalls.push_back(ball);
    });
}

/**
 * 상태 변경
 */
void TornadoBrick::setSpecialState(bool specialState) {
    
    SpecialBrick::setSpecialState(specialState);
    
    auto originSpinAnimFiles = ContentResourceHelper::getBrickAnimationFiles(data.image, "spin");
    auto originTornadoAnimFiles = ContentResourceHelper::getBrickAnimationFiles(data.image, "tornado");
    
    auto runAnimation = [=](StringList animFiles, float duration, int loops, SBCallbackNode onFinished) {
        auto anim = SBNodeUtils::createAnimation(animFiles, duration);
        
        image->setAnimation(anim, loops);
        image->runAnimation(onFinished);
    };
    
    // 토네이도
    if( specialState ) {
    // if( getFloorChangedCount() % 2 == 0 ) {
        setCollisionLocked(true);
        setBodyActive(false);
        
        contactBalls.clear();
        schedule(CC_CALLBACK_1(TornadoBrick::checkCollision, this), SCHEDULER_CHECK_COLLISION);
        
        // 연출
        const float SPIN_SLOW_DURATION = ANIM_SLOW_DURATION(originSpinAnimFiles.size()) - ANIM_FAST_INTERVAL;
        
        const int   SPIN_FAST_LOOPS = 2;
        const float SPIN_FAST_DURATION = ANIM_FAST_DURATION(originSpinAnimFiles.size()) * SPIN_FAST_LOOPS;
        
        const float TOTAL_DURATION = SPIN_SLOW_DURATION + SPIN_FAST_DURATION;
        
        // Step 1. spin slow
        runAnimation(originSpinAnimFiles, ANIM_SLOW_INTERVAL, 1, nullptr);
        
        // Step 2. spin fast
        SBDirector::postDelayed(this, [=]() {
            
            runAnimation(originSpinAnimFiles, ANIM_FAST_INTERVAL, SPIN_FAST_LOOPS, [=](Node*) {
                
                // Step 3. tornado
                runAnimation(originTornadoAnimFiles, ANIM_FAST_INTERVAL, SBAnimationSprite::LOOP_FOREVER,
                             nullptr);
            });
            
        }, SPIN_SLOW_DURATION);
        
        // fade out
        auto fadeOut = FadeOut::create(TOTAL_DURATION);
        bg->runAction(fadeOut->clone());
        hpGage.bg->runAction(fadeOut->clone());
        hpGage.label->runAction(fadeOut->clone());
    }
    // 일반 브릭
    else {
        setCollisionLocked(false);
        setBodyActive(true);
        unschedule(SCHEDULER_CHECK_COLLISION);
        
        // 연출
        StringList spinFastAnimFiles({
            originSpinAnimFiles[2], originSpinAnimFiles[3], originSpinAnimFiles[0], originSpinAnimFiles[1],
        });
        
        StringList spinSlowAnimFiles({
            originSpinAnimFiles[2], originSpinAnimFiles[3], originSpinAnimFiles[0],
        });
        
        const int   SPIN_FAST_LOOPS = 2;
        const float SPIN_FAST_DURATION = (ANIM_FAST_DURATION(spinFastAnimFiles.size()) * SPIN_FAST_LOOPS) + ANIM_FAST_INTERVAL;
        const float SPIN_SLOW_DURATION = ANIM_SLOW_DURATION(spinSlowAnimFiles.size());
        
        const float TOTAL_DURATION = SPIN_SLOW_DURATION + SPIN_FAST_DURATION;
        
        // Step 1. spin fast
        runAnimation(spinFastAnimFiles, ANIM_FAST_INTERVAL, SPIN_FAST_LOOPS, nullptr);
        
        // Step 2. spin slow
        SBDirector::postDelayed(this, [=]() {
            
            runAnimation(spinSlowAnimFiles, ANIM_SLOW_INTERVAL, 1, [=](Node*) {
                
                // Step 3. idle
                this->setImage(BrickImageType::IDLE, true);
            });
            
        }, SPIN_FAST_DURATION);
        
        // fade in
        auto fadeIn = FadeIn::create(TOTAL_DURATION);
        bg->runAction(fadeIn->clone());
        hpGage.bg->runAction(fadeIn->clone());
        hpGage.label->runAction(fadeIn->clone());
    }
}

void TornadoBrick::updateSpecialState() {
    
    SpecialBrick::updateSpecialState();
    
    const int POS_Y = (int)getTilePosition().y;
    setSpecialState(POS_Y % 2 != 0);
}

void TornadoBrick::prepareRemove() {
    
    unschedule(SCHEDULER_CHECK_COLLISION);
    
    SpecialBrick::prepareRemove();
}
