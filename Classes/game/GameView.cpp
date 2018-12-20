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
#include "object/Ball.hpp"
#include "object/friend/Friend.hpp"
#include "object/AimController.hpp"
#include "object/tile/Brick.hpp"
#include "object/tile/Item.hpp"

#include "ui/TopMenu.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const string SCHEDULER_SHOOT                 = "SCHEDULER_SHOOT";

static const float  BALL_MOVE_DURATION              = 0.2f;
static const float  BALL_WITHDRAW_MOVE_DURATION     = 0.25f;

#define DEBUG_DRAW_PHYSICS      1
#define DEBUG_DRAW_TILE         1

GameView::GameView() :
gameMgr(GameManager::getInstance()),
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
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initPhysics();
    initBg();
    initMenu();
    initMap();
    initBall();
    initTile();
    initFriends();
    initAimController();
    initTouchListener();
    initGameListener();
    initIAPListener();
    
    return true;
}

void GameView::onEnter() {
    
    Node::onEnter();
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
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
    
    isWithdrawEnabled = false;
    isWithdraw = false;
    
    addBallQueue.clear();
    toAddFriendsBalls = 0;
    
    eliteBrickDropRate = GameManager::getFloor().eliteBrickDropRate;
    isEliteDropped = false;
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    showStageLabel(1);
    
    addBall(GameManager::getConfig()->getFirstBallCount());
    addBrick();
    addItem();
    
    onTileAddFinished();
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
 * 스테이지 클리어
 */
void GameView::onStageClear() {
    
    GameManager::onNextStage();
}

/**
 * 다음 스테이지
 */
void GameView::onNextStage(const StageData &stage) {
    
    CCLOG("onNextStage: %d", stage.stage);
    
    eliteBrickDropRate = GameManager::getFloor().eliteBrickDropRate;
    
    showStageLabel(stage.stage);
    
    // TODO: 다음 스테이지 처리
    GameManager::onGameOver();
}

/**
 * 다음 층
 */
void GameView::onNextFloor(const FloorData &floor) {
    
    Log::i("onNextFloor: %d", floor.floor);
    
    // 다음 층 진행
    if( !floor.isNull() ) {
        // 엘리트 벽돌 드랍률 업데이트
        if( isEliteDropped ) {
            isEliteDropped = false;
            eliteBrickDropRate = floor.eliteBrickDropRate;
        } else {
            if( eliteBrickDropRate == 0 ) {
                eliteBrickDropRate = floor.eliteBrickDropRate;
            } else {
                eliteBrickDropRate *= 2;
            }
        }
        
        // 타일 추가
        addBrick();
        addItem();
        
        onTileAddFinished();
    }
    // 다음 층 없다면 타일만 이동
    else {
        downTile();
    }
}

/**
 * 스코어 변경
 */
void GameView::onScoreChanged(int score) {
}

/**
 * 타일 추가 완료
 */
void GameView::onTileAddFinished() {
    
    Log::i("onTileAddFinished");
    
    // 타일 등장 연출 완료 후 이동
    SBDirector::postDelayed(this, [=]() {
        this->downTile();
    }, Game::Tile::ENTER_DURATION + 0.1f);
}

/**
 * 타일 이동 완료
 */
void GameView::onTileDownFinished() {
    
    Log::i("onTileDownFinished");
    
    // 게임 오버 체크
    if( isExistBrick(0) ) {
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
    
    aimController->setEnabled(true, getBricks());
    getChildByTag(Tag::BTN_BRICK_DOWN)->setVisible(true);
    
    // 아이템 비활성화
    auto items = getItems();
    
    for( auto item : items ) {
        item->setActive(false, false);
    }
    
    // 볼 위치 설정
    for( auto ball : balls ) {
        ball->setPosition(aimController->getStartPosition());
        ball->setSyncLocked(false);
        ball->syncNodeToBody();
    }
}

/**
 * 발사 완료
 */
void GameView::onShootFinished() {
    
}

/**
 * 모든 볼 추락 완료
 */
void GameView::onFallFinished() {
    
    Log::i("onFallFinished");
    
    // 스테이지 클리어 체크
    if( GameManager::getFloor().isLastFloor && getBricks().size() == 0 ) {
        GameManager::onStageClear();
        return;
    }
    
    updateBallCountUI();
    
    // 획득한 아이템 반영
    addBallFromQueue();
    
    if( toAddFriendsBalls > 0 ) {
        toAddFriendsBalls = 0;
    }
    
    // 다음 층으로 전환
    GameManager::onNextFloor();
}

/**
 * 벽돌이 깨졌습니다
 */
void GameView::onBrickBreak(Brick *brick) {
    
    // 스코어 업데이트
    GameManager::addScore(brick->getOriginalHp());
    
    // remove brick
    removeTile(brick);
}

/**
 * 물리 세계 업데이트
 */
void GameView::onPhysicsUpdate() {
 
    // 터널링 체크
    if( !isWithdraw && tunnelingCountLabel ) {
        auto bricks = getBricks();
        
        for( auto brick : bricks ) {
            auto brickBox = SBNodeUtils::getBoundingBoxInWorld(brick);
        
            vector<Ball*> needRemoveBalls;
            
            for( auto ball : balls ) {
                auto body = ball->getBody();
                
                if( !body || !body->IsActive() || !body->IsAwake() ) {
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
void GameView::onContactBrick(Ball *ball, Brick *brick) {
    
    if( brick->isBroken() ) {
        Log::w("이미 깨진 벽돌 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    brick->sufferDamage(ball->getDamage());
}

/**
 * 볼 & 아이템 충돌
 */
void GameView::onContactItem(Ball *ball, Item *item) {
    
    if( !item->isAwake() ) {
        Log::w("아이템 sleep 상태에서 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    switch( item->getData().type ) {
        // 기본 볼 개수 증가
        case ItemType::POWER_UP: {
            auto ball = Sprite::create(BALL_IMAGE);
            ball->setAnchorPoint(ANCHOR_M);
            ball->setPosition(item->getPosition());
            ball->setColor(Color3B(255, 20, 20));
            addChild(ball, (int)ZOrder::BALL);
            
            addBallQueue.push_back(ball);
            
            // 볼 추락 연출
            Vec2 pos = Vec2(ball->getPositionX(), SHOOTING_POSITION_Y);
            float dist = pos.getDistance(ball->getPosition());
            
            ball->runAction(MoveTo::create(0.8f, pos));
            // ball->runAction(MoveTo::create(dist * 0.001f, pos));
            
        } break;
        
        // 프렌즈 볼 개수 증가
        case ItemType::FRIENDS_POWER_UP: {
            ++toAddFriendsBalls;
        } break;
            
        default:
            CCASSERT(false, "GameView::onContactItem error: invalid item.");
            break;
    }
    
    removeTile(item);
}

/**
 * 볼 & 바닥 충돌
 */
void GameView::onContactFloor(Ball *ball) {
    
    if( isWithdraw ) {
        Log::w("볼 회수 중 바닥 충돌 이벤트 발생!!").showMessageBox();
        return;
    }

    ball->setFall(true);
    ++fallenBallCount;

    CCLOG("onContactFloor fallenBallCount: %d", fallenBallCount);
    
    // 볼 바디 비활성화
    // ball->sleepWithAction();
    ball->sleep(false);
    ball->setCollisionLocked(true);
    
    // 첫번째 볼 추락
    if( fallenBallCount == 1 ) {
        aimController->setStartPosition(Vec2(ball->getPosition().x, SHOOTING_POSITION_Y));
    }
    
    // 시작 위치로 이동
    auto pos = aimController->getStartPosition();
    ball->setPosition(Vec2(ball->getPositionX(), pos.y));
    ball->runAction(MoveTo::create(BALL_MOVE_DURATION, pos));
    
    // 모든 볼 추락
    if( fallenBallCount == balls.size() ) {
        SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onFallFinished, this), 0.1f);
    }
}

/**
 * 발사!
 */
void GameView::shoot(const Vec2 &endPosition) {
    
    shootIndex = 0;
    fallenBallCount = 0;
    getChildByTag(Tag::BTN_BRICK_DOWN)->setVisible(false);
    
    // 아이템 활성화
    auto items = getItems();
    
    for( auto item : items ) {
        item->setActive(true);
    }
    
    // 속도 설정
    // b2Vec2 velocity(random(-30, 30), 30);
    Vec2 diff = endPosition - aimController->getStartPosition();
    b2Vec2 velocity = PTM(diff);
    velocity.Normalize();
    velocity.x *= BALL_MAX_VELOCITY;
    velocity.y *= BALL_MAX_VELOCITY;
    
    // CCLOG("onShoot diff: %f,%f (%f,%f)", diff.x, diff.y, velocity.x, velocity.y);
    
    // 볼 업데이트
    for( auto ball : balls ) {
        ball->awake();
    }
    
    // 하나씩 발사
    schedule([=](float dt) {

        if( shootIndex == balls.size() || isWithdraw ) {
            // 모두 발사 or 볼 회수됨
            this->unschedule(SCHEDULER_SHOOT);
            this->onShootFinished();
            return;
        }
        
        CCLOG("shoot! ball index: %d", shootIndex);
        // SBAudioEngine::playEffect(DIR_SOUND + "shoot.wav");
        
        auto ball = balls[shootIndex];
        ball->shoot(velocity);

        ++shootIndex;

        // 볼 개수 UI 업데이트
        ballCountLabel->setVisible(shootIndex < balls.size());
        ballCountLabel->setString("X" + TO_STRING(balls.size() - shootIndex));
        
    }, SHOOT_INTERVAL, SCHEDULER_SHOOT);
    
    // 볼 회수 기능 활성화
    isWithdrawEnabled = false;
    isWithdraw = false;
    
    SBDirector::postDelayed(this, [=]() {
        isWithdrawEnabled = true;
    }, SHOOT_INTERVAL*2);
}

/**
 * 모든 볼을 회수합니다
 */
void GameView::withdrawBalls() {
 
    CCLOG("withdrawBalls shootIndex: %d", shootIndex);
    
    isWithdraw = true;
    
    // for( auto ball : balls ) {
    for( int i = 0; i < shootIndex; ++i ) {
        auto ball = balls[i];
        
        if( ball->isFall() ) {
            continue;
        }
        
        ball->sleep(false);
        ball->setCollisionLocked(true);
        ball->setSyncLocked(true);
        
        // 시작 위치로 이동
        // auto delay = DelayTime::create(0.3f);
        Vec2 spreadPos(ball->getPosition() + Vec2(random<int>(-100, 100), random<int>(50, 100)));
        spreadPos.x = MIN(MAP_BOUNDING_BOX.getMaxX()*0.9f, spreadPos.x);
        spreadPos.x = MAX(MAP_BOUNDING_BOX.getMinX()*1.1f, spreadPos.x);
        spreadPos.y = MIN(MAP_BOUNDING_BOX.getMaxY()*0.9f, spreadPos.y);
        spreadPos.y = MAX(MAP_BOUNDING_BOX.getMinY()*1.1f, spreadPos.y);
        
        auto spread = MoveTo::create(0.07f, spreadPos);
        auto delay = DelayTime::create(random<int>(0,3) * 0.1f);
        auto move = MoveTo::create(BALL_WITHDRAW_MOVE_DURATION, aimController->getStartPosition());
        ball->runAction(Sequence::create(/*delay, */spread, delay, move, nullptr));
    }
    
    // 모든 볼 추락 완료
    SBDirector::postDelayed(this, [=]() {
        this->onFallFinished();
    }, BALL_WITHDRAW_MOVE_DURATION+0.5f);
}

/**
 * 모든 타일을 한칸 아래로 이동 시킵니다
 */
void GameView::downTile() {
    
    auto list = tiles;
    
    for( auto tile : list ) {
        // 아이템이 마지막 칸에 있으면 제거
        if( tile->getTilePosition().y == 0 && dynamic_cast<Item*>(tile) ) {
            removeTile(tile);
            continue;
        }
        
        tile->down();
    }

    SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onTileDownFinished, this),
                            Game::Tile::MOVE_DURATION + 0.1f);
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
 * 볼 추가
 */
void GameView::addBall(int count, bool updateUI) {
    
    const int MAX_BALL_COUNT = GameManager::getConfig()->getMaxBallCount();
    
    for( int i = 0; i < count && balls.size() <= MAX_BALL_COUNT; ++i ) {
        auto ball = Ball::create();
        ball->setPosition(aimController->getStartPosition());
        ball->sleep(false);
        ball->setCollisionLocked(true);
        addChild(ball, (int)ZOrder::BALL);
    
        balls.push_back(ball);
    }

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
    auto move = MoveTo::create(BALL_MOVE_DURATION, pos);
    auto moveCallback = CallFunc::create([=]() {
        
        // Step 2. 연출 후 실제로 추가
        this->addBall(1, false);
    });
    
    for( auto ball : addBallQueue ) {
        auto action = Sequence::create(move->clone(), moveCallback->clone(), RemoveSelf::create(), nullptr);
        ball->runAction(action);
    }
    
    addBallQueue.clear();
    
    SBDirector::postDelayed(this, [=]() {
        
        this->updateBallCountUI();
        
        // UP 효과
        auto label = Label::createWithTTF(STR_FORMAT("%dUP", ADD_BALL_COUNT),
                                          FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_MB);
        label->setPosition(ballCountLabel->getPosition() + Vec2(0, 40));
        label->setColor(Color3B(119, 255, 0));
        label->enableOutline(Color4B::BLACK, 3);
        addChild(label, SBZOrder::BOTTOM);
        
        auto move = MoveBy::create(0.8f, Vec2(0, 70));
        auto remove = RemoveSelf::create();
        label->runAction(Sequence::create(move, remove, nullptr));
        
    }, BALL_MOVE_DURATION+0.05f);
}

/**
 * 볼 제거
 */
void GameView::removeBall(Ball *ball) {
    
    CCLOG("GameView::removeBall");
    
    auto it = std::find(balls.begin(), balls.end(), ball);
    if( it == balls.end() ) {
        CCASSERT(false, "GameView::removeBall error.");
    }
    
    // remove from list
    balls.erase(it);
    
    // remove ball
    ball->setNeedRemove(true);
}

/**
 * 벽돌 추가
 */
void GameView::addBrick() {
    
    const int TILE_ROWS = GameManager::getConfig()->getTileRows();
    const int TILE_POSITION_Y = GameManager::getConfig()->getTileColumns()-1;
    auto floor = GameManager::getFloor();
    
    // 비어있는 좌표 리스트 생성
    auto positions = getEmptyPositions(TILE_POSITION_Y);
    if( positions.size() == 0 ) {
        return;
    }
    
    CCASSERT(positions.size() == TILE_ROWS, "GameView::addBrick error.");
    
    auto addBrick = [=](const BrickData &data, int hp, Game::Tile::Position tilePos) -> Brick* {
        auto brick = Brick::create(data, hp);
        brick->setTilePosition(tilePos, false);
        this->addTile(brick);
        
        brick->setOnBreakListener([=](Node*) {
            this->onBrickBreak(brick);
        });
        
        return brick;
    };
    
    // 보스 벽돌
    // FIXME: 보스 1개, 가운데 위치하도록 하드코딩 되어있음
    for( int i = 0; i < floor.bossBrickList.size() && positions.size() > 0; ++i ) {
    }
    
    if( floor.bossBrickList.size() > 0 ) {
        addBrick(floor.bossBrickList[0], floor.brickHp*10, Game::Tile::Position(2, TILE_POSITION_Y));
    }
    
    // 엘리트 벽돌
    positions = getEmptyRandomPositions(TILE_POSITION_Y);
    
    int  dropCount = floor.getRandomDropCount();
    isEliteDropped = (random<int>(1,100) <= eliteBrickDropRate);
    
    CCLOG("addBrick(%d) dropCount: %d eliteBrickDropRate: %d isEliteDropped: %d",
          floor.floor, dropCount, eliteBrickDropRate, isEliteDropped);
    
    if( isEliteDropped ) {
        addBrick(floor.getRandomEliteBrick(), floor.brickHp*3, positions[0]);
        
        --dropCount;
        positions.erase(positions.begin());
    }
    
    // 일반 벽돌
    for( int i = 0; i < dropCount && positions.size() > 0; ++i ) {
        addBrick(floor.getRandomBrick(), floor.brickHp, positions[0]);
        
        positions.erase(positions.begin());
    }
}

/**
 * 아이템 추가
 */
void GameView::addItem() {
    
    const int TILE_POSITION_Y = GameManager::getConfig()->getTileColumns()-1;
    auto floor = GameManager::getFloor();
    
    // 비어있는 좌표 리스트 생성
    auto positions = getEmptyRandomPositions(TILE_POSITION_Y);
    if( positions.size() == 0 ) {
        return;
    }
    
    // Power Up
    bool isDrop = (random<int>(1,100) <= floor.powerUpDropRate);
    
    if( isDrop ) {
        ItemData data;
        data.type = ItemType::POWER_UP;
        
        auto item = Item::create(data);
        item->setTilePosition(positions[0], false);
        addTile(item);
        
        positions.erase(positions.begin());
    }
}

/**
 * 타일 추가
 */
void GameView::addTile(Game::Tile *tile) {
    
    CCASSERT(!getTile(tile->getTilePosition()), "GameView::addTile error: already exists.");
    
    tile->enterWithAction();
    
    addChild(tile, (int)ZOrder::TILE);
    tiles.push_back(tile);
}

/**
 * 타일 제거
 */
void GameView::removeTile(Game::Tile *tile) {
    
    auto it = std::find(tiles.begin(), tiles.end(), tile);
    if( it == tiles.end() ) {
        CCASSERT(false, "GameView::removeTile error.");
    }
    
    // remove from list
    tiles.erase(it);
    
    // remove tile
    tile->removeWithAction();
}

/**
 * 중앙에 Stage 라벨 표시
 */
void GameView::showStageLabel(int stage) {
    
    auto label = Label::createWithTTF(STR_FORMAT("STAGE %d", stage), FONT_COMMODORE, 30, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(ANCHOR_M);
    label->setPosition(Vec2MC(0, 0));
    label->setTextColor(Color4B(244,0,137,255));
    label->enableOutline(Color4B::BLACK, 3);
    addChild(label);
    
    auto delay = DelayTime::create(1.5f);
    auto remove = RemoveSelf::create();
    label->runAction(Sequence::create(delay, remove, nullptr));
    
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
        float diffY = touch->getLocation().y - touch->getStartLocation().y;
        bool isDragged = (diffY < -250);
        CCLOG("onTouchMoved diffY: %f, isDragged: %d, fallenBallCount: %d", diffY, isDragged, fallenBallCount);
        
        if( isDragged ) {
            withdrawBalls();
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
 * y줄의 타일을 반환합니다
 */
vector<Game::Tile*> GameView::getTiles(int y) {
    
    return SBCollection::find(tiles, [y](Game::Tile *tile) -> bool {
        return tile->getTilePosition().y == y;
    });
}

vector<Game::Tile*> GameView::getBricks(int y) {
    
    return SBCollection::find(tiles, [y](Game::Tile *tile) -> bool {
        return tile->getTilePosition().y == y && dynamic_cast<Brick*>(tile);
    });
}

vector<Game::Tile*> GameView::getBricks() {
    
    return SBCollection::find(tiles, [](Game::Tile *tile) -> bool {
        return dynamic_cast<Brick*>(tile);
    });
}

vector<Game::Tile*> GameView::getItems() {
    
    return SBCollection::find(tiles, [](Game::Tile *tile) -> bool {
        return dynamic_cast<Item*>(tile);
    });
}

/**
 * 좌표에 해당하는 타일을 반환합니다
 */
Game::Tile* GameView::getTile(const Game::Tile::Position &pos) {
    
    for( auto tile : tiles ) {
        if( tile->isContainsPosition(pos) ) {
            return tile;
        }
    }
    
    return nullptr;
}

/**
 * y줄의 비어있는 좌표를 반환합니다
 */
Game::Tile::Positions GameView::getEmptyPositions(int y) {
    
    const int TILE_ROWS = GameManager::getConfig()->getTileRows();
    Game::Tile::Positions positions;
    
    for( int x = 0; x < TILE_ROWS; ++x ) {
        Game::Tile::Position pos(x,y);
        
        if( !getTile(pos) ) {
            positions.push_back(pos);
        }
    }
    
    return positions;
}

Game::Tile::Positions GameView::getEmptyRandomPositions(int y) {
    
    auto positions = getEmptyPositions(y);
    random_shuffle(positions.begin(), positions.end());
    
    return positions;
}

/**
 * y줄의 타일 유무를 반환합니다
 */
bool GameView::isExistTile(int y) {
    
    return getTiles(y).size() > 0;
}

bool GameView::isExistBrick(int y) {
    
    return getBricks(y).size() > 0;
}

/**
 * 물리 초기화
 */
void GameView::initPhysics() {

    auto physicsMgr = GameManager::getPhysicsManager();
    this->world = physicsMgr->initWorld();
    
    physicsMgr->setOnUpdateListener(CC_CALLBACK_0(GameView::onPhysicsUpdate, this));
    physicsMgr->setOnContactBrickListener(CC_CALLBACK_2(GameView::onContactBrick, this));
    physicsMgr->setOnContactItemListener(CC_CALLBACK_2(GameView::onContactItem, this));
    physicsMgr->setOnContactFloorListener(CC_CALLBACK_1(GameView::onContactFloor, this));
    
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
    
    // 벽돌 아래로 이동 버튼
    // game_bg_bottom.png Vec2BC(0, 26) , Size(720, 52)
    {
        auto btn = SBNodeUtils::createTouchNode();
        btn->setTag(Tag::BTN_BRICK_DOWN);
        btn->setVisible(false);
        btn->setAnchorPoint(ANCHOR_MB);
        btn->setPosition(Vec2BC(0,0));
        btn->setContentSize(Size(SB_WIN_SIZE.width, 52*0.8f));
        btn->setCascadeOpacityEnabled(true);
        btn->setOpacity(255*0.5f);
        addChild(btn, (int)ZOrder::MENU);
        
        btn->addClickEventListener([=](Ref*) {
            this->onClickDownButton();
        });
        
        auto bg = LayerColor::create(Color4B::BLACK);
        bg->setIgnoreAnchorPointForPosition(false);
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(Vec2::ZERO);
        bg->setContentSize(btn->getContentSize());
        btn->addChild(bg);
        
        // BRICK DOWN
        // Tap to move down bricks
        // TAP TO MOVE DOWN BRICKS
        auto label = Label::createWithTTF("BRICK DOWN", FONT_COMMODORE, 28, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(btn->getContentSize(), 0, 0));
        label->setScale(btn->getContentSize().height*0.9f / label->getContentSize().height);
        label->setTextColor(Color4B::WHITE);
        // label->enableOutline(Color4B::BLACK, 3);
        btn->addChild(label);
    }
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
 * 볼 초기화
 */
void GameView::initBall() {
    
    // 영역 확인용
    /*
    {
        auto ball = Sprite::create(BALL_IMAGE);
        ball->setAnchorPoint(ANCHOR_M);
        ball->setPosition(FIRST_SHOOTING_POSITION);
        ball->setColor(Color3B::RED);
        addChild(ball);

        SBNodeUtils::scale(ball, BALL_SIZE);
    }
    */
    
    // 볼 갯수 표시 라벨
    // X50 size:26 stroke:3 Vec2BC(0, 29) , Size(66, 28)
    ballCountLabel = Label::createWithTTF("X0", FONT_COMMODORE, 26, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    ballCountLabel->setAnchorPoint(ANCHOR_M);
    ballCountLabel->setPosition(Vec2BC(0, 29));
    ballCountLabel->setTextColor(Color4B::WHITE);
    ballCountLabel->enableOutline(Color4B::BLACK, 3);
    addChild(ballCountLabel);
}

/**
 * 타일 초기화
 */
void GameView::initTile() {
    
#if DEBUG_DRAW_TILE
    auto parent = getChildByTag(Tag::DEBUG_DRAW_VIEW);

    const int TILE_ROWS = GameManager::getConfig()->getTileRows();
    const int TILE_COLUMNS = GameManager::getConfig()->getTileColumns();
    
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
    
    // game_friends_02_idle1.png Vec2BL(46, 85) , Size(63, 75)
    auto friendNode = Friend::create();
    friendNode->setPosition(Vec2BL(46, 85));
    addChild(friendNode);
    
    friends.push_back(friendNode);
}

/**
 * 조준 컨트롤러 초기화
 */
void GameView::initAimController() {
    
    aimController = AimController::create();
    aimController->setEnabled(false);
    aimController->setOnShootListener(CC_CALLBACK_1(GameView::shoot, this));
    addChild(aimController, (int)ZOrder::AIM_CONTROLLER);
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
    listener->onGameEnter            = CC_CALLBACK_0(GameView::onGameEnter, this);
    listener->onGameExit             = CC_CALLBACK_0(GameView::onGameExit, this);
    listener->onGameReset            = CC_CALLBACK_0(GameView::onGameReset, this);
    listener->onGameStart            = CC_CALLBACK_0(GameView::onGameStart, this);
    listener->onGameRestart          = CC_CALLBACK_0(GameView::onGameRestart, this);
    listener->onGamePause            = CC_CALLBACK_0(GameView::onGamePause, this);
    listener->onGameResume           = CC_CALLBACK_0(GameView::onGameResume, this);
    listener->onGameOver             = CC_CALLBACK_0(GameView::onGameOver, this);
    listener->onGameContinue         = CC_CALLBACK_0(GameView::onGameContinue, this);
    listener->onGameResult           = CC_CALLBACK_0(GameView::onGameResult, this);
    listener->onBoostStart           = CC_CALLBACK_0(GameView::onBoostStart, this);
    listener->onBoostEnd             = CC_CALLBACK_0(GameView::onBoostEnd, this);
    listener->onStageClear           = CC_CALLBACK_0(GameView::onStageClear, this);
    listener->onNextStage            = CC_CALLBACK_1(GameView::onNextStage, this);
    listener->onNextFloor            = CC_CALLBACK_1(GameView::onNextFloor, this);
    listener->onScoreChanged         = CC_CALLBACK_1(GameView::onScoreChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
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
