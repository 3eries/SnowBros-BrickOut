//
//  GameView.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include <spine/spine-cocos2dx.h>

#include "SceneManager.h"
#include "UserDefaultKey.h"
#include "User.hpp"
#include "UIHelper.hpp"
#include "TestHelper.hpp"
#include "RankingManager.hpp"

#include "object/GameMap.hpp"
#include "object/FriendsLayer.hpp"
#include "object/Ball.hpp"
#include "object/AimController.hpp"

#include "object/tile/TileFactory.hpp"
#include "object/tile/Brick_10012.hpp"
#include "object/tile/GhostBrick.hpp"
#include "object/tile/Item.hpp"

#include "ui/TopMenu.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const string SCHEDULER_SHOOT                 = "SCHEDULER_SHOOT";
static const string SCHEDULER_CHECK_AUTO_WITHDRAW   = "SCHEDULER_CHECK_AUTO_WITHDRAW";
static const string SCHEDULER_CHECK_WITHDRAW_GUIDE  = "SCHEDULER_CHECK_WITHDRAW_GUIDE";

static const float  DRAG_MIN_DIST                   = 200; // 드래그 판단 거리

#define DEBUG_DRAW_PHYSICS      1
#define DEBUG_DRAW_TILE         1

GameView::GameView() :
gameMgr(GameManager::getInstance()),
aimController(nullptr),
tileLayer(nullptr),
friendsLayer(nullptr),
tunnelingCount(0),
tunnelingCountLabel(nullptr),
bodyCountLabel(nullptr) {
}

GameView::~GameView() {
 
    removeListeners(this);
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    initPhysics();
    initBg();
    initMenu();
    initMap();
    initAimController();
    initTile();
    initFriends();
    initBall();
    initTouchListener();
    initGameListener();
    initPhysicsListener();
    initIAPListener();
    
    return true;
}

void GameView::onEnter() {
    
    Node::onEnter();
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    // 볼 회수 가이드라인 노출 기준선
    /*
    auto line = LayerColor::create(Color4B(255,0,0,255*0.8f));
    line->setIgnoreAnchorPointForPosition(false);
    line->setAnchorPoint(ANCHOR_ML);
    line->setPosition(Vec2(0, (MAP_BOUNDING_BOX.getMidY() + 200)));
    line->setContentSize(Size(SB_WIN_SIZE.width, 10));
    addChild(line, INT_MAX);
    */
    
    /*
    SBDirector::postDelayed(this, [=]() {
        GAME_MANAGER->addBrick(Database::getBrick("brick_00001"));
        GAME_MANAGER->addBrick(Database::getBrick("brick_00002"));
        GAME_MANAGER->addBrick(Database::getBrick("brick_00003"));
        GAME_MANAGER->addBrick(Database::getBrick("brick_10001"));
        GameManager::onStageClear();
    }, 4, true);
    */
}

void GameView::onExit() {
    
    Node::onExit();
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 게임 진입
 */
void GameView::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void GameView::onGameExit() {
    
    removeChildByTag(Tag::DEBUG_DRAW_VIEW);
}

/**
 * 게임 리셋
 */
void GameView::onGameReset() {
    
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    SBNodeUtils::recursivePause(this);
}

/**
 * 게임 재게
 */
void GameView::onGameResume() {
    
    SBNodeUtils::recursiveResume(this);
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
    
    isWithdrawEnabled = false;
    
    // 볼 비활성화
    for( auto ball : balls ) {
        ball->setSyncLocked(true);
        ball->stopAllActions();
        ball->setCollisionLocked(true);
        ball->sleepWithAction();
        
        /*
        if( ball->isFall() ) {
            ball->setVisible(false);
        } else {
            ball->sleepWithAction();
        }
        */
    }
    
    ballCountLabel->runAction(FadeOut::create(0.1f));
    
    // 연출
    /*
    const float DURATION = 1.0f;
    
    auto scale = ScaleTo::create(DURATION, 1.3f);
    auto moveBy = MoveBy::create(DURATION, Vec2(0, 200));
    runAction(Spawn::create(scale, moveBy, nullptr));
     */
}

/**
 * 게임 이어하기
 */
void GameView::onGameContinue() {
}

/**
 * 게임 결과
 */
void GameView::onGameResult() {
}

/**
 * 부스트 시작
 */
void GameView::onBoostStart() {
}

/**
 * 부스트 종료
 */
void GameView::onBoostEnd() {
}

/**
 * 스테이지 변경
 */
void GameView::onStageChanged(const StageData &stage) {
    
    // 볼 회수 비활성화
    isWithdrawEnabled = false;
    isWithdraw = false;
    
    // 슈팅 포지션 초기화 
    aimController->setStartPosition(FIRST_SHOOTING_POSITION);
    
    for( auto ball : balls ) {
        ball->setPosition(aimController->getStartPosition());
        ball->syncNodeToBody();
    }
    
    friendsLayer->updatePosition(aimController->getStartPosition(), false);
    
    // update ui
    showStageLabel(stage.stage);
    updateBallCountUI();
}

/**
 * 스테이지 클리어
 */
void GameView::onStageClear() {
}

/**
 * 다음 스테이지로 이동
 */
void GameView::onMoveNextStage(const StageData &stage) {
    
    Log::i("onMoveNextStage: %d", stage.stage);
    
    // hide balls
    for( auto ball : balls ) {
        ball->setVisible(false);
    }
    
    ballCountLabel->setVisible(false);
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameView::onMoveNextStageFinished(const StageData &stage) {
    
    // show ball
    balls[0]->setVisible(true);
    ballCountLabel->setVisible(true);
}

/**
 * 층 변경
 */
void GameView::onFloorChanged(const FloorData &floor) {
    
}

/**
 * 다음 층
 */
void GameView::onNextFloor(const FloorData &floor) {
    
    // 게임 오버 직전
    if( !tileLayer->isRowClear(1) ) {
        return;
    }
    
    // 마지막 칸에 있는 아이템 자동 획득
    auto items = tileLayer->getTiles<Item*>(1);
    
    for( auto item : items ) {
        eatItem(item, false);
    }
    
    // 획득한 아이템 반영
    addBallFromQueue();
}

/**
 * 타일 이동 완료
 */
void GameView::onTileDownFinished() {
    
    Log::i("onTileDownFinished");
    
    // 게임 오버 체크
    if( !tileLayer->isRowClear(0) ) {
        GameManager::onGameOver();
        return;
    }
    
    // 다음 턴
    onShootingReady();
}

/**
 * 발사 준비
 */
void GameView::onShootingReady() {
    
    Log::i("onShootingReady");
    
    // update aim controller
    aimController->setEnabled(true, tileLayer->getBricks());
    
    // update brick down button
    auto brickDownBtn = getChildByTag(Tag::BTN_BRICK_DOWN);
    brickDownBtn->stopAllActions();
    brickDownBtn->setOpacity(255);
    
    if( checkBrickDown() ) {
        brickDownBtn->setVisible(true);
        brickDownBtn->setOpacity(0);
        
        auto fadeIn = FadeIn::create(1.3f);
        auto fadeOut = FadeOut::create(2.0f);
        auto seq = Sequence::create(fadeIn, DelayTime::create(0.2f), fadeOut, nullptr);
        brickDownBtn->runAction(RepeatForever::create(seq));
        
    } else {
        brickDownBtn->setVisible(false);
    }
    
    // 아이템 비활성화
    auto items = tileLayer->getItems();
    
    for( auto item : items ) {
        item->setBodyActive(false);
    }
    
    // 볼 위치 설정
    for( auto ball : balls ) {
        ball->setPosition(aimController->getStartPosition());
        ball->setSyncLocked(false);
        ball->syncNodeToBody();
    }
}

/**
 * 조준 시작
 */
void GameView::onAimingStart() {
    
    getChildByTag(Tag::BTN_BRICK_DOWN)->setVisible(false);
}

/**
 * 조준 종료
 */
void GameView::onAimingEnd() {
    
    if( checkBrickDown() ) {
        getChildByTag(Tag::BTN_BRICK_DOWN)->setVisible(true);
    }
}

/**
 * 발사 완료
 */
void GameView::onShootFinished() {
    
}

/**
 * 프렌즈 볼을 포함한 모든 볼 추락 완료
 */
void GameView::onAllBallFallFinished() {
    
    Log::i("onAllBallFallFinished flag: %d", isAllBallFallFinished);
    
    if( isAllBallFallFinished ) {
        return;
    }
    
    isAllBallFallFinished = true;
    
    unschedule(SCHEDULER_CHECK_AUTO_WITHDRAW);
    updateBallCountUI();
    
    // 스테이지 클리어
    if( checkStageClear() ) {
        return;
    }
    
    // 게임 오버 상황이 아닐때만 프렌즈 좌표 업데이트
    if( tileLayer->isRowClear(1) ) {
        friendsLayer->updatePosition(aimController->getStartPosition(), true);
    }
    
    // 다음 층으로 전환
    GameManager::onNextFloor();
}

/**
 * 프렌즈 볼 추락 완료
 */
void GameView::onFriendsBallFallFinished() {
    
    Log::i("onFriendsBallFallFinished isUserBallFallFinished: %d", isUserBallFallFinished());
    
    // 모든 볼 추락
    if( isUserBallFallFinished() ) {
        stopWithdrawGuide();
        SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onAllBallFallFinished, this), 0.1f);
    }
}

/**
 * 벽돌이 깨졌습니다
 */
void GameView::onBrickBreak(Brick *brick) {
    
    const bool isBoss = brick->isBoss();
    
    // remove brick
    tileLayer->removeTile(brick);
    
    // 스테이지 클리어
    auto items = tileLayer->getItems();
    
    if( checkStageClear() ) {
        SBDirector::postDelayed(this, [=]() {
            GameManager::onStageClear();
        }, STAGE_CLEAR_DELAY, true);
        
        // 볼 회수
        withdrawBall(0.5f);
        
        // 남아있는 아이템 자동 획득
        for( auto item : items ) {
            eatItem((Item*)item, false);
        }
        
        addBallFromQueue();
        
        return;
    }
    
    auto bricks = tileLayer->getBricks();
    
    // 남은 브릭 없음
    if( bricks.size() == 0 ) {
        // 남은 아이템 없으면 볼 회수
        if( items.size() == 0 ) {
            // withdrawBalls(0.5f);
        }
    }
    // 남은 브릭 있음
    else {
        // 모든 보스 브릭이 깨진 경우 귀속된 브릭도 제거
        if( isBoss ) {
            vector<Brick*> bossBricks;
            vector<Brick*> infinityBricks;
            
            for( auto brick : bricks ) {
                if( brick->isBoss() ) {
                    bossBricks.push_back(brick);
                } else if( brick->isInfinityHp() ) {
                    infinityBricks.push_back(brick);
                }
            }
            
            if( bossBricks.size() == 0 ) {
                for( auto brick : infinityBricks ) {
                    brick->onBreak();
                }
            }
        }
    }
}

/**
 * 물리 세계 업데이트
 */
void GameView::onPhysicsUpdate() {
    
    // 터널링 체크
    if( !isWithdraw && tunnelingCountLabel ) {
        auto bricks = tileLayer->getBricks();
        
        for( auto brick : bricks ) {
            if( brick->isCollisionLocked() ) {
                continue;
            }
            
            auto brickBox = SBNodeUtils::getBoundingBoxInWorld(brick);
            vector<Ball*> needRemoveBalls;
            
            for( auto ball : balls ) {
                auto body = ball->getBody();
                
                if( !body || !ball->isBodyActive() || !ball->isBodyAwake() || ball->isCollisionLocked() ) {
                    continue;
                }
                
                Vec2 center = MTP(body->GetPosition());
                
                if( brickBox.containsPoint(center) ) {
                    Log::w("터널링 발생!!").showMessageBox();
                // if( brickBox.intersectsCircle(center, BALL_RADIUS) ) {
                    ++tunnelingCount;
                    needRemoveBalls.push_back(ball);
                }
            }
            
            for( auto ball : needRemoveBalls ) {
                removeBall(ball);
            }
        }
        
        tunnelingCountLabel->setString(STR_FORMAT("Tunneling: %d", tunnelingCount));
    }
    
    // 바디 수 체크
    if( bodyCountLabel ) {
        bodyCountLabel->setString(STR_FORMAT("Body Count: %d", world->GetBodyCount()));
    }
}

/**
 * 볼 & 벽돌 충돌
 */
void GameView::onContactBrick(Ball *ball, Game::Tile *brickTile, Vec2 contactPoint) {
}

/**
 * 볼 & 아이템 충돌
 */
void GameView::onContactItem(Ball *ball, Game::Tile *itemTile) {
    
    auto item = dynamic_cast<Item*>(itemTile);
    
    if( !item->isBodyAwake() ) {
        Log::w("아이템 sleep 상태에서 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    eatItem(item, true);
}

/**
 * 볼 & 벽 충돌
 */
void GameView::onContactWall(Ball *ball) {
}

/**
 * 볼 & 바닥 충돌
 */
void GameView::onContactFloor(Ball *ball) {
    
    if( isWithdraw ) {
        Log::w("볼 회수 중 바닥 충돌 이벤트 발생!!").showMessageBox();
        return;
    }

    // 프렌즈 볼은 처리하지 않음
    if( dynamic_cast<FriendBall*>(ball) ) {
        return;
    }
    
    ++fallenBallCount;
//    Log::i("onContactFloor fallenBallCount: %d", fallenBallCount);
    
    // 첫번째 볼 추락
    const bool isFallenFirstBall = (fallenBallCount == 1);
    
    if( isFallenFirstBall ) {
        aimController->setStartPosition(Vec2(ball->getPosition().x, SHOOTING_POSITION_Y));
    }
    
    // 시작 위치로 이동
    auto pos = aimController->getStartPosition();
    ball->setPosition(Vec2(ball->getPositionX(), pos.y));
    
    auto move = MoveTo::create(BALL_JOIN_MOVE_DURATION, pos);
    auto callFunc = CallFunc::create([=]() {
        
        // 두번째 볼부터 hide
        if( !isFallenFirstBall ) {
            ball->setVisible(false);
        }
    });
    ball->runAction(Sequence::create(move, callFunc, nullptr));
    
    // 모든 볼 추락
    if( isUserBallFallFinished() ) {
        Log::i("GameView::onContactFloor user ball fall finished.");
    
        if( isFriendsBallFallFinished() ) {
            stopWithdrawGuide();
            SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onAllBallFallFinished, this),
                                    BALL_JOIN_MOVE_DURATION*0.5f);
        }
    }
}

/**
 * 발사!
 */
void GameView::shoot(const Vec2 &endPosition) {
    
    Log::i("GameView::shoot");
    
    shootIndex = 0;
    fallenBallCount = 0;
    isAllBallFallFinished = false;
    
    auto brickDownBtn = getChildByTag(Tag::BTN_BRICK_DOWN);
    brickDownBtn->stopAllActions();
    brickDownBtn->setVisible(false);
    
    // 아이템 활성화
    auto items = tileLayer->getItems();
    
    for( auto item : items ) {
        item->setBodyActive(true);
    }
    
    // 속도 설정
    Vec2 diff = endPosition - aimController->getStartPosition();
    
    b2Vec2 velocity = PTM(diff);
    velocity.Normalize();
    velocity.x *= BALL_MAX_VELOCITY;
    velocity.y *= BALL_MAX_VELOCITY;
    
    // Log::i("shoot velocity: %f,%f", velocity.x, velocity.y);
    
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
        
        // 볼 회수됨
        if( isWithdraw ) {
            this->shootStop();
            return;
        }
        
        // Log::i("GameView::shoot ballIndex: %d", shootIndex);
        
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
            
            // 프렌즈 볼 슈팅
            if( shootIndex >= balls.size()*0.3f && !friendsLayer->isShot() ) {
                friendsLayer->shoot(this->tileLayer);
            }
        }
        // 슈팅 완료
        else {
            ballCountLabel->setVisible(false);
            
            this->shootStop();
            this->onShootFinished();
        }
    }, SHOOT_INTERVAL, SCHEDULER_SHOOT);
    
    // 볼 회수 기능 활성화
    isWithdrawEnabled = false;
    isWithdraw = false;
    schedule(CC_CALLBACK_1(GameView::checkWithdrawGuide, this), 1, SCHEDULER_CHECK_WITHDRAW_GUIDE);
    
    SBDirector::postDelayed(this, [=]() {
        isWithdrawEnabled = true;
        schedule(CC_CALLBACK_1(GameView::checkAutoWithdrawBall, this), 1, SCHEDULER_CHECK_AUTO_WITHDRAW);
    }, SHOOT_INTERVAL*2);
}

void GameView::shootStop() {
    
    friendsLayer->shootStop();
    unschedule(SCHEDULER_SHOOT);
}

/**
 * 모든 볼을 회수합니다
 */
void GameView::withdrawBall(float delay) {
    
    if( isWithdraw || !isWithdrawEnabled ) {
        return;
    }
    
    unschedule(SCHEDULER_CHECK_AUTO_WITHDRAW);
    
    if( delay > 0 ) {
        SBDirector::postDelayed(this, [=]() {
            this->withdrawBall();
        }, delay);
        
        return;
    }
    
    Log::i("GameView::withdrawBall shootIndex: %d", shootIndex);
    
    shootStop();
    
    isWithdrawEnabled = false;
    isWithdraw = true;
    fallenBallCount = (int)balls.size();
    ballCountLabel->setVisible(false);
    
    stopWithdrawGuide();
    
    // 프렌즈 볼 회수
    friendsLayer->withdrawBall();
    
    // 발사된 볼 회수
    Vec2 finalPosition = aimController->getStartPosition();
    
    for( int i = 0; i < shootIndex; ++i ) {
        auto ball = balls[i];
        
        if( ball->isFall() ) {
            continue;
        }
        
        ball->fallToFloor();
         
        // 시작 위치로 이동
        ball->stopAllActions();
        
        Vec2 spreadPos(ball->getPosition() + Vec2(random<int>(-80, 80), random<int>(50, 80)));
        spreadPos.x = MIN(MAP_BOUNDING_BOX.getMaxX()*0.9f, spreadPos.x);
        spreadPos.x = MAX(MAP_BOUNDING_BOX.getMinX()*1.1f, spreadPos.x);
        spreadPos.y = MIN(MAP_BOUNDING_BOX.getMaxY()*0.9f, spreadPos.y);
        spreadPos.y = MAX(MAP_BOUNDING_BOX.getMinY()*1.1f, spreadPos.y);
        
        auto spread = Spawn::create(MoveTo::create(0.07f, spreadPos),
                                    RotateTo::create(0.07f, SBMath::getDegree(spreadPos, finalPosition)),
                                    nullptr);
        auto delay = DelayTime::create(random<int>(0,3) * 0.1f);
        auto callFunc = CallFunc::create([=]() {
        
            auto move = MoveTo::create(BALL_WITHDRAW_MOVE_DURATION, finalPosition);
            auto callFunc = CallFunc::create([=]() {
                
                ball->setRotation(0);
                
                // 두번째 볼부터 hide
                if( i > 0 ) {
                    ball->setVisible(false);
                }
            });
            ball->runAction(Sequence::create(move, callFunc, nullptr));
        });
        ball->runAction(Sequence::create(spread, delay, callFunc, nullptr));
    }
    
    // 발사되지 않은 볼 회수
    for( int i = shootIndex; i < balls.size(); ++i ) {
        auto ball = balls[i];
        
        if( !ball->getPosition().equals(finalPosition) ) {
            auto move = MoveTo::create(BALL_JOIN_MOVE_DURATION, finalPosition);
            ball->runAction(move);
        }
    }
    
    // 모든 볼 추락 완료
    SBDirector::postDelayed(this, [=]() {
        this->onAllBallFallFinished();
    }, BALL_WITHDRAW_MOVE_DURATION+0.5f);
}

/**
 * 자동 볼 회수
 */
void GameView::checkAutoWithdrawBall(float dt) {
    
    if( !isWithdrawEnabled ) {
        unschedule(SCHEDULER_CHECK_AUTO_WITHDRAW);
        return;
    }
    
    // 남은 아이템 체크
    if( tileLayer->getItems().size() > 0 ) {
        return;
    }
    
    // 남은 브릭 체크, 유효하지 않은 브릭 빼고 체크
    auto bricks = tileLayer->getBricks();
    
    SBCollection::remove(bricks, [](Brick *brick) {
        // 중립 브릭 제외
        if( brick->getData().type == BrickType::SPECIAL_NEUTRAL ) {
            return true;
        }
        
        // 투명화된 고스트 브릭 제외
        auto ghostBrick = dynamic_cast<GhostBrick*>(brick);
        
        if( ghostBrick && ghostBrick->isGhostState() ) {
            return true;
        }
        
        return false;
    });
    
    if( bricks.size() == 0 ) {
        withdrawBall();
    }
}

/**
 * 볼 회수 가이드
 */
void GameView::checkWithdrawGuide(float dt) {
    
    auto diff = balls.size() - fallenBallCount;
    
    // 모든 볼 추락됨
    if( diff == 0 ) {
        stopWithdrawGuide();
        return;
    }
    
    // 전체 볼의 10% 이하만 활동중인지 체크
    if( diff > balls.size()*0.1f ) {
        return;
    }
    
    // 맵 상단에서 활동중인 볼이 있는지 체크
    const float POS_Y = MAP_BOUNDING_BOX.getMidY() + 200;
    bool found = false;
    
    for( auto ball : balls ) {
        if( ball->isFall() ) {
            continue;
        }
        
        if( SB_BOUNDING_BOX_IN_WORLD(ball).getMinY() > POS_Y ) {
            found = true;
            break;
        }
    }
    
    if( !found ) {
        return;
    }
    
    auto guide = spine::SkeletonAnimation::createWithJsonFile(ANIM_WITHDRAW_GUIDE);
    guide->setTag(Tag::WITHDRAW_GUIDE);
    guide->setAnchorPoint(Vec2::ZERO);
    guide->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    addChild(guide, (int)ZOrder::MENU);
    
    SBSpineHelper::clearAnimation(guide, ANIM_NAME_CLEAR);
    
    guide->setAnimation(0, "drag", false);
    guide->setCompleteListener([=](spTrackEntry *entry) {
        
//        string animName = entry->animation->name;
//
//        if( animName == "drag" ) {
//            SBDirector::postDelayed(guide, [=]() {
//                guide->setAnimation(0, "drag", false);
//            }, 2.0f);
//        }
        guide->removeFromParent();
        schedule(CC_CALLBACK_1(GameView::checkWithdrawGuide, this), 3, SCHEDULER_CHECK_WITHDRAW_GUIDE);
    });
    
    unschedule(SCHEDULER_CHECK_WITHDRAW_GUIDE);
}

void GameView::stopWithdrawGuide() {
    
    unschedule(SCHEDULER_CHECK_WITHDRAW_GUIDE);
    removeChildByTag(Tag::WITHDRAW_GUIDE);
}

/**
 * 벽돌 아래로 이동 버튼 클릭
 */
void GameView::onClickDownButton() {
    
    aimController->setEnabled(false);
    getChildByTag(Tag::BTN_BRICK_DOWN)->setVisible(false);
    
    GameManager::onNextFloor();
}

/**
 * 아이템 획득
 */
void GameView::eatItem(Item *item, bool isFallAction) {
    
    switch( item->getData().type ) {
        // 기본 볼 개수 증가
        case ItemType::POWER_UP: {
            auto ballItem = Sprite::create(SELECTED_BALL_IMAGE);
            ballItem->setAnchorPoint(ANCHOR_M);
            ballItem->setPosition(item->getPosition());
            addChild(ballItem, (int)ZOrder::BALL);
            
            addBallQueue.push_back(ballItem);
            
            // 아이템 추락 연출
            if( isFallAction ) {
                Vec2 pos = Vec2(ballItem->getPositionX(), SHOOTING_POSITION_Y);
                float dist = pos.getDistance(ballItem->getPosition());
                
                // CCLOG("item fall duration: %f", dist * 0.001f);
                // ballItem->runAction(MoveTo::create(0.8f, pos));
                
                auto move = MoveTo::create(dist * 0.001f, pos);
                auto callFunc = CallFunc::create([=]() {
                
                    // 추락 좌표에 이미 아이템이 있으면 hide
                    auto list = SBCollection::find(addBallQueue, [=](Sprite *other) -> bool {
                        return other != ballItem && other->getPosition().equals(ballItem->getPosition());
                    });
                    
                    if( list.size() > 0 ) {
                        ballItem->setVisible(false);
                        return;
                    }
                    
                    // blink
                    auto fade = Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f), nullptr);
                    // auto blink = Blink::create(1.0f, 2);
                    ballItem->runAction(RepeatForever::create(fade));
                });
                ballItem->runAction(Sequence::create(move, callFunc, nullptr));
            }
            
        } break;
            
        // 프렌즈 볼 개수 증가
        case ItemType::FRIENDS_POWER_UP: {
            friendsLayer->eatFriendsItem(item);
            tileLayer->setFriendsPower(friendsLayer->getFriendsPower());
        } break;
            
        default:
            CCASSERT(false, "GameView::eatItem error: invalid item.");
            break;
    }
    
    tileLayer->removeTile(item);
}

/**
 * 볼 추가
 */
void GameView::addBall(int count, bool updateUI) {
    
    const int MAX_BALL_COUNT = GAME_CONFIG->getMaxBallCount();
    
    for( int i = 0; i < count && balls.size() <= MAX_BALL_COUNT; ++i ) {
        auto ball = Ball::create();
        ball->setPosition(aimController->getStartPosition());
        ball->setVisible(false);
        ball->setBodyAwake(false);
        ball->setCollisionLocked(true);
        addChild(ball, (int)ZOrder::BALL);
    
        balls.push_back(ball);
    }

    tileLayer->setBallCount((int)balls.size());
    
    if( updateUI ) {
        updateBallCountUI();
    }
}

/**
 * 큐에 있는 볼 추가
 */
void GameView::addBallFromQueue() {
 
    const int ADD_BALL_COUNT = (int)addBallQueue.size();
    
    if( ADD_BALL_COUNT == 0 ) {
        return;
    }
    
    // Step 1. 볼 합체 연출
    auto pos = aimController->getStartPosition();
    auto move = MoveTo::create(BALL_JOIN_MOVE_DURATION, pos);
    auto moveCallback = CallFunc::create([=]() {
        
        // Step 2. 연출 후 실제로 추가
        // this->addBall(1, false);
    });
    
    for( auto ball : addBallQueue ) {
        ball->stopAllActions();
        ball->setOpacity(255);
        ball->setVisible(true);
        
        auto action = Sequence::create(move->clone(), moveCallback->clone(), RemoveSelf::create(), nullptr);
        ball->runAction(action);
    }
    
    addBall((int)addBallQueue.size(), false);
    addBallQueue.clear();
    
    SBDirector::postDelayed(this, [=]() {
        
        this->updateBallCountUI();
        
        // UP 효과
        auto label = Label::createWithTTF(STR_FORMAT("%dUP", ADD_BALL_COUNT),
                                          FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_MB);
        label->setPosition(ballCountLabel->getPosition() + Vec2(0, 40));
        label->setTextColor(Color4B(119, 255, 0, 255));
        label->enableOutline(Color4B::BLACK, 3);
        addChild(label, SBZOrder::MIDDLE);
        
        auto move = MoveBy::create(0.8f, Vec2(0, 70));
        auto remove = RemoveSelf::create();
        label->runAction(Sequence::create(move, remove, nullptr));
        
    }, BALL_JOIN_MOVE_DURATION+0.05f);
}

/**
 * 볼 제거
 */
void GameView::removeBall(Ball *ball) {
    
    Log::w("볼이 제거됨").showMessageBox();
    
    auto it = std::find(balls.begin(), balls.end(), ball);
    if( it == balls.end() ) {
        CCASSERT(false, "GameView::removeBall error.");
        return;
    }
    
    // remove from list
    balls.erase(it);
    
    // remove ball
    ball->setNeedRemove(true);
}

/**
 * 모든 볼이 추락했는지를 반환합니다
 */
//bool GameView::isAllBallFallFinished() {
//
//    return isUserBallFallFinished() && isFriendsBallFallFinished();
//}

/**
 * 모든 유저 볼이 추락했는지를 반환합니다
 */
bool GameView::isUserBallFallFinished() {
    
    return fallenBallCount == balls.size();
}

/**
 * 모든 프렌즈 볼이 추락했는지를 반환합니다
 */
bool GameView::isFriendsBallFallFinished() {
    
    return friendsLayer->isFallFinished();
}

/**
 * 브릭 다운 가능 여부를 반환합니다
 */
bool GameView::checkBrickDown() {
    
    return !GameManager::isStageLastFloor() && !tileLayer->isExistTile<Brick*>(1);
}

/**
 * 스테이지 클리어 여부를 반환합니다
 */
bool GameView::checkStageClear() {
    
    return GameManager::isStageLastFloor() && tileLayer->checkStageClear();
}

/**
 * 중앙에 Stage 라벨 표시
 */
void GameView::showStageLabel(int stage) {
    
    auto label = Label::createWithBMFont(FONT_STAGE_LARGE, STR_FORMAT("S %d", stage),
                                         TextHAlignment::CENTER);
    label->setAnchorPoint(ANCHOR_M);
    addChild(label);
    
    auto seq = Sequence::create(MoveTo::create(0,        Vec2MC(-700, 0)),
                                MoveTo::create(0.15f,    Vec2MC(20, 0)),
                                MoveTo::create(0.05f,    Vec2MC(0, 0)),
                                DelayTime::create(0.8f),
                                MoveTo::create(0.06f,    Vec2MC(-20, 0)),
                                MoveTo::create(0.06f,    Vec2MC(0, 0)),
                                MoveTo::create(0.18f,    Vec2MC(700, 0)),
                                RemoveSelf::create(), nullptr);
    label->runAction(seq);
}

/**
 * 볼 개수 UI 업데이트
 */
void GameView::updateBallCountUI() {
    
    ballCountLabel->setVisible(true);
    ballCountLabel->setPosition(Vec2(aimController->getStartPosition().x, ballCountLabel->getPositionY()));
    ballCountLabel->setString("X" + TO_STRING(balls.size()));
}

/**
 * 터치 시작
 */
bool GameView::onTouchBegan(Touch *touch, Event*) {
    
    if( !SBNodeUtils::hasVisibleParents(this) ) {
        return false;
    }
    
    if( !isWithdrawEnabled || isWithdraw ) {
        return false;
    }
    
    if( superbomb::isMultiTouch(touch) ) {
        return true;
    }
    
    return true;
}

/**
 * 터치 이동
 */
void GameView::onTouchMoved(Touch *touch, Event *event) {
    
    if( superbomb::isMultiTouch(touch) ) {
        return;
    }
    
    // drag 하여 볼 회수
    if( isWithdrawEnabled && !isWithdraw ) {
        float dist = touch->getLocation().y - touch->getStartLocation().y;
        bool isDragged = (dist < -DRAG_MIN_DIST);
        
        Log::i("onTouchMoved dist: %f, isDragged: %d, fallenBallCount: %d", dist, isDragged, fallenBallCount);
        
        if( isDragged ) {
            withdrawBall();
        }
    }
}

/**
 * 터치 종료
 */
void GameView::onTouchEnded(Touch *touch, Event*) {
    
    if( superbomb::isMultiTouch(touch) ) {
        return;
    }
}

/**
 * 터치 취소
 */
void GameView::onTouchCancelled(Touch *touch, Event *event) {
    
    onTouchEnded(touch, event);
}

/**
 * 물리 초기화
 */
void GameView::initPhysics() {

    auto physicsMgr = GameManager::getPhysicsManager();
    this->world = physicsMgr->initWorld();
    
#if DEBUG_DRAW_PHYSICS
    // DebugDrawView
    auto view = DebugDrawView::create(world);
    view->setTag(Tag::DEBUG_DRAW_VIEW);
    view->setVisible(false);
    addChild(view, SBZOrder::MIDDLE);
    
    physicsMgr->setDebugDrawView(view);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
//        flags += b2Draw::e_jointBit;
//        flags += b2Draw::e_aabbBit;
//        flags += b2Draw::e_pairBit;
//        flags += b2Draw::e_centerOfMassBit;
    view->getDebugDraw()->SetFlags(flags);
    
    float posY = -100;
    
    auto btn = SBButton::create("DebugDraw", FONT_COMMODORE, 30);
    btn->setZoomScale(0.1f);
    btn->setAnchorPoint(ANCHOR_TL);
    btn->setPosition(Vec2TL(10, posY));
    btn->setContentSize(Size(250, 50));
    addChild(btn, view->getLocalZOrder());
    
    btn->setOnClickListener([=](Node*) {
        view->setVisible(!view->isVisible());
    });
    
    // btn->addChild(SBNodeUtils::createBackgroundNode(btn, Color4B::RED));
    
    // Debug Tunneling Label
    tunnelingCountLabel = Label::createWithTTF("", FONT_COMMODORE, 30,
                                               Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    tunnelingCountLabel->setAnchorPoint(ANCHOR_TL);
    tunnelingCountLabel->setPosition(Vec2TL(30, posY-70));
    view->addChild(tunnelingCountLabel);

    bodyCountLabel = Label::createWithTTF("", FONT_COMMODORE, 30,
                                          Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    bodyCountLabel->setAnchorPoint(ANCHOR_TL);
    bodyCountLabel->setPosition(Vec2TL(30, posY-120));
    view->addChild(bodyCountLabel);
#endif
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
}

/**
 * 메뉴 초기화
 */
void GameView::initMenu() {
    
    auto topMenu = TopMenu::create();
    addChild(topMenu, (int)ZOrder::MENU);
    
    // 브릭 다운 버튼
    // game_btn_brick_down.png Vec2BR(-60, 222) , Size(116, 108)
    auto brickDownBtn = SBButton::create(DIR_IMG_GAME + "game_btn_brick_down.png");
    brickDownBtn->setTag(Tag::BTN_BRICK_DOWN);
    brickDownBtn->setZoomScale(0.1f);
    brickDownBtn->setAnchorPoint(ANCHOR_M);
    brickDownBtn->setPosition(Vec2BR(-60, 222));
    brickDownBtn->setVisible(false);
    addChild(brickDownBtn, (int)ZOrder::MENU);
    
    brickDownBtn->setOnClickListener([=](Node*) {
        this->onClickDownButton();
    });
}

/**
 * 맵 초기화
 */
void GameView::initMap() {
    
    auto map = GameMap::create();
    map->setTag(Tag::MAP);
    addChild(map);
    
    GameManager::getPhysicsManager()->setMap(map);
}

/**
 * 조준 컨트롤러 초기화
 */
void GameView::initAimController() {
    
    aimController = AimController::create();
    aimController->setEnabled(false);
    aimController->setOnAimingStartListener(CC_CALLBACK_0(GameView::onAimingStart, this));
    aimController->setOnAimingEndListener(CC_CALLBACK_0(GameView::onAimingEnd, this));
    aimController->setOnShootListener(CC_CALLBACK_1(GameView::shoot, this));
    addChild(aimController, (int)ZOrder::AIM_CONTROLLER);
}

/**
 * 타일 초기화
 */
void GameView::initTile() {
    
    auto stencil = LayerColor::create(Color4B::RED);
    stencil->setIgnoreAnchorPointForPosition(false);
    stencil->setAnchorPoint(Vec2::ZERO);
    stencil->setPosition(MAP_ORIGIN);
    stencil->setContentSize(MAP_CONTENT_SIZE);
    
    tileLayer = TileLayer::create();
    tileLayer->setOnTileDownFinishedListener(CC_CALLBACK_0(GameView::onTileDownFinished, this));
    tileLayer->setOnBrickBreakListener(CC_CALLBACK_1(GameView::onBrickBreak, this));
    addChild(tileLayer, (int)ZOrder::TILE);
    
#if DEBUG_DRAW_TILE
    auto parent = getChildByTag(Tag::DEBUG_DRAW_VIEW);
    
    const int TILE_ROWS = GAME_CONFIG->getTileRows();
    const int TILE_COLUMNS = GAME_CONFIG->getTileColumns();
    
    for( int x = 0; x < TILE_ROWS; ++x ) {
        for( int y = 0; y < TILE_COLUMNS; ++y ) {
            auto tile = LayerColor::create(Color4B(255,255,0,255*0.1f));
            tile->setIgnoreAnchorPointForPosition(false);
            tile->setCascadeOpacityEnabled(true);
            tile->setAnchorPoint(ANCHOR_M);
            tile->setPosition(Game::Tile::convertToTilePosition(x, y));
            tile->setContentSize(MEASURE_TILE_SIZE(1, 1));
            parent->addChild(tile, -1);
            
            auto label = Label::createWithTTF(STR_FORMAT("%d,%d", x, y), FONT_COMMODORE, 20);
            label->setAnchorPoint(ANCHOR_M);
            label->setPosition(Vec2MC(tile->getContentSize(), 0, 0));
            label->setTextColor(Color4B::WHITE);
            tile->addChild(label);
        }
    }
#endif
}

/**
 * 프렌즈 초기화
 */
void GameView::initFriends() {
    
    friendsLayer = FriendsLayer::create();
    friendsLayer->setOnFallFinishedListener(CC_CALLBACK_0(GameView::onFriendsBallFallFinished, this));
    friendsLayer->updatePosition(FIRST_SHOOTING_POSITION, false);
    addChild(friendsLayer, (int)ZOrder::FRIENDS);
    
    tileLayer->setFriendsPower(friendsLayer->getFriendsPower());
}

/**
 * 볼 초기화
 */
void GameView::initBall() {
    
    // 볼 갯수 표시 라벨
    ballCountLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    ballCountLabel->setAnchorPoint(ANCHOR_M);
    ballCountLabel->setPosition(Vec2BC(0, 25));
    ballCountLabel->setTextColor(Color4B::WHITE);
    ballCountLabel->enableOutline(Color4B::BLACK, 3);
    addChild(ballCountLabel, (int)ZOrder::BALL);
    
#if ENABLE_TEST_MENU
    addBall(TEST_HELPER->getFirstBallCount());
#else
    addBall(GAME_CONFIG->getFirstBallCount());
#endif
    
    balls[0]->setVisible(true);
}

/**
 * 터치 리스너 초기화
 */
void GameView::initTouchListener() {
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameView::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameView::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameView::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(GameView::onTouchCancelled, this);
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/**
 * 게임 리스너 초기화
 */
void GameView::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter               = CC_CALLBACK_0(GameView::onGameEnter, this);
    listener->onGameExit                = CC_CALLBACK_0(GameView::onGameExit, this);
    listener->onGameReset               = CC_CALLBACK_0(GameView::onGameReset, this);
    listener->onGameStart               = CC_CALLBACK_0(GameView::onGameStart, this);
    listener->onGameRestart             = CC_CALLBACK_0(GameView::onGameRestart, this);
    listener->onGamePause               = CC_CALLBACK_0(GameView::onGamePause, this);
    listener->onGameResume              = CC_CALLBACK_0(GameView::onGameResume, this);
    listener->onGameOver                = CC_CALLBACK_0(GameView::onGameOver, this);
    listener->onGameContinue            = CC_CALLBACK_0(GameView::onGameContinue, this);
    listener->onGameResult              = CC_CALLBACK_0(GameView::onGameResult, this);
    listener->onBoostStart              = CC_CALLBACK_0(GameView::onBoostStart, this);
    listener->onBoostEnd                = CC_CALLBACK_0(GameView::onBoostEnd, this);
    listener->onStageChanged            = CC_CALLBACK_1(GameView::onStageChanged, this);
    listener->onStageClear              = CC_CALLBACK_0(GameView::onStageClear, this);
    listener->onMoveNextStage           = CC_CALLBACK_1(GameView::onMoveNextStage, this);
    listener->onMoveNextStageFinished   = CC_CALLBACK_1(GameView::onMoveNextStageFinished, this);
    listener->onFloorChanged            = CC_CALLBACK_1(GameView::onFloorChanged, this);
    listener->onNextFloor               = CC_CALLBACK_1(GameView::onNextFloor, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

/**
 * 물리 리스너 초기화
 */
void GameView::initPhysicsListener() {

    auto listener = GamePhysicsListener::create();
    listener->setTarget(this);
    listener->onUpdate              = CC_CALLBACK_0(GameView::onPhysicsUpdate, this);
    listener->onContactBrick        = CC_CALLBACK_3(GameView::onContactBrick, this);
    listener->onContactItem         = CC_CALLBACK_2(GameView::onContactItem, this);
    listener->onContactWall         = CC_CALLBACK_1(GameView::onContactWall, this);
    listener->onContactFloor        = CC_CALLBACK_1(GameView::onContactFloor, this);
    GameManager::getPhysicsManager()->addListener(listener);
}

/**
 * IAP 리스너 초기화
 */
void GameView::initIAPListener() {
    
    auto onRemoveAds = [=]() {
        // TODO:
    };
    
    // purchase listener
    auto purchaseListener = iap::PurchaseListener::create();
    purchaseListener->setForever(true);
    purchaseListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
    
    // restore listener
    auto restoreListener = iap::RestoreListener::create();
    restoreListener->setForever(true);
    restoreListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, restoreListener);
}
