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

#include "object/GameMap.hpp"
#include "object/Ball.hpp"
#include "object/AimController.hpp"
#include "object/tile/Brick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const string SCHEDULER_SHOOT                 = "SCHEDULER_SHOOT";

#define DEBUG_DRAW_PHYSICS      1
#define DEBUG_DRAW_TILE         1

GameView::GameView() :
gameMgr(GameManager::getInstance()),
tunnelingCount(0) {
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
    initMap();
    initBall();
    initTile();
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
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    addBall(FIRST_BALL_COUNT);
    addBrick(4);
    
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
 * 스코어 변경
 */
void GameView::onScoreChanged(int score) {
    
    auto scoreLabel = getChildByTag<Label*>(Tag::SCORE);
    
    if( scoreLabel ) {
        scoreLabel->setString(TO_STRING(score));
    }
}

/**
 * 레벨 변경
 */
void GameView::onLevelChanged(const LevelInfo &level) {
}

/**
 * 타일 추가 완료
 */
void GameView::onTileAddFinished() {
    
    // 타일 등장 연출 완료 후 이동
    SBDirector::postDelayed(this, [=]() {
        this->downTile();
    }, Game::Tile::ENTER_DURATION + 0.2f);
}

/**
 * 타일 이동 완료
 */
void GameView::onTileDownFinished() {
    
    // 게임 오버 체크
    if( isExistTile(0) ) {
        GameManager::onGameOver();
        return;
    }
    
    updateBallCountUI();

    // 다음 턴
    onShootingReady();
}

/**
 * 발사 준비
 */
void GameView::onShootingReady() {
    
    aimController->setEnabled(true, getBricks());
    
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
    
    // 타일 추가
    addBrick(random(1, 2)); // 벽돌
    addItem(1);
    
    onTileAddFinished();
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
    if( !isWithdraw ) {
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
    bodyCountLabel->setString(STR_FORMAT("Body Count: %d", world->GetBodyCount()));
}

/**
 * 볼 & 벽돌 충돌
 */
void GameView::onContactBrick(Ball *ball, Brick *brick) {
    
    brick->sufferDamage(ball->getDamage());
}

/**
 * 볼 & 바닥 충돌
 */
void GameView::onContactFloor(Ball *ball) {
    
    if( isWithdraw ) {
        
        return;
    }

    ball->setFall(true);
    ++fallenBallCount;

    // 볼 바디 비활성화
    // ball->sleepWithAction();
    ball->sleep(false);
    
    // 첫번째 볼 추락
    if( fallenBallCount == 1 ) {
        aimController->setStartPosition(Vec2(ball->getPosition().x, SHOOTING_POSITION_Y));
    }
    
    // 시작 위치로 이동
    auto pos = aimController->getStartPosition();
    ball->setPosition(Vec2(ball->getPositionX(), pos.y));
    ball->runAction(MoveTo::create(0.2f, pos));
    
    // 모든 볼 추락
    if( fallenBallCount == balls.size() ) {
        SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onFallFinished, this), 1.0f);
    }
}

/**
 * 발사!
 */
void GameView::shoot(const Vec2 &endPosition) {
    
    shootIndex = 0;
    fallenBallCount = 0;
    
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

        ballCountLabel->setString(TO_STRING(balls.size() - shootIndex));

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
        auto move = MoveTo::create(0.25f, aimController->getStartPosition());
        ball->runAction(Sequence::create(/*delay, */spread, delay, move, nullptr));
    }
    
    // 모든 볼 추락 완료
    SBDirector::postDelayed(this, [=]() {
        this->onFallFinished();
    }, 1.0f);
}

/**
 * 모든 타일을 한칸 아래로 이동 시킵니다
 */
void GameView::downTile() {
    
    for( auto tile : tiles ) {
        tile->down();
    }

    SBDirector::postDelayed(this, CC_CALLBACK_0(GameView::onTileDownFinished, this),
                            Game::Tile::MOVE_DURATION + 0.2f);
}

/**
 * 볼 추가
 */
void GameView::addBall(int count) {
    
    for( int i = 0; i < count && balls.size() <= MAX_BALL_COUNT; ++i ) {
        auto ball = Ball::create();
        ball->setPosition(aimController->getStartPosition());
        ball->sleep(false);
        addChild(ball, ZOrder::BALL);
    
        balls.push_back(ball);
    }

    ballCountLabel->setString(TO_STRING(balls.size()));
}

/**
 * 볼 제거
 */
void GameView::removeBall(Ball *ball) {
    
    auto it = std::find(balls.begin(), balls.end(), ball);
    if( it == balls.end() ) {
        CCASSERT(false, "GameView::removeBall error.");
    }
    
    // remove from list
    balls.erase(it);
    
    // remove ball
    ball->sleep();
    ball->setNeedRemove(true);
}

/**
 * 벽돌 추가
 */
void GameView::addBrick(int count) {
    
    auto positions = getEmptyPositions(TILE_COLUMNS-1);
    CCASSERT(positions.size() >= count, "GameView::addBrick error.");
    
    random_shuffle(positions.begin(), positions.end());
    
    for( int i = 0; i < count; ++i ) {
        auto brick = Brick::create(FIRST_BALL_COUNT);
        brick->setTilePosition(positions[i], false);
        addTile(brick);
        
        brick->setOnBreakListener([=](Node*) {
            this->onBrickBreak(brick);
        });
    }
}

/**
 * 아이템 추가
 */
void GameView::addItem(int count) {
    
    auto positions = getEmptyPositions(TILE_COLUMNS-1);
    
    if( positions.size() == 0 ) {
        return;
    }
    
    CCASSERT(positions.size() >= count, "GameView::addItem error.");
    
    random_shuffle(positions.begin(), positions.end());
    
    for( int i = 0; i < count; ++i ) {
        
    }
}

/**
 * 타일 추가
 */
void GameView::addTile(Game::Tile *tile) {
    
    CCASSERT(!getTile(tile->getTilePosition()), "GameView::addTile error: already exists.");
    
    tile->enterWithAction();
    
    addChild(tile, ZOrder::TILE);
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
 * 볼 갯수 UI 업데이트
 */
void GameView::updateBallCountUI() {
    
    ballCountLabel->setString(TO_STRING(balls.size()));
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

/**
 * 좌표에 해당하는 타일을 반환합니다
 */
Game::Tile* GameView::getTile(const Game::Tile::Position &pos) {
    
    for( auto tile : tiles ) {
        if( tile->getTilePosition().x == pos.x && tile->getTilePosition().y == pos.y ) {
            return tile;
        }
    }
    
    return nullptr;
}

/**
 * y줄의 비어있는 좌표를 반환합니다
 */
Game::Tile::Positions GameView::getEmptyPositions(int y) {
    
    Game::Tile::Positions positions;
    
    for( int x = 0; x < TILE_ROWS; ++x ) {
        Game::Tile::Position pos(x,y);
        
        if( !getTile(pos) ) {
            positions.push_back(pos);
        }
    }
    
    return positions;
}

/**
 * y줄의 타일 유무를 반환합니다
 */
bool GameView::isExistTile(int y) {
    
    for( int x = 0; x < TILE_ROWS; ++x ) {
        Game::Tile::Position pos(x,y);
        
        if( getTile(pos) ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 물리 초기화
 */
void GameView::initPhysics() {

    auto physicsMgr = GameManager::getPhysicsManager();
    this->world = physicsMgr->initWorld();
    
    physicsMgr->setOnUpdateListener(CC_CALLBACK_0(GameView::onPhysicsUpdate, this));
    physicsMgr->setOnContactBrickListener(CC_CALLBACK_2(GameView::onContactBrick, this));
    physicsMgr->setOnContactFloorListener(CC_CALLBACK_1(GameView::onContactFloor, this));
    
#if DEBUG_DRAW_PHYSICS
    // DebugDrawView
    auto view = DebugDrawView::create(world);
    view->setTag(Tag::DEBUG_DRAW_VIEW);
    view->setVisible(false);
    addChild(view, SBZOrder::BOTTOM);
    
    physicsMgr->setDebugDrawView(view);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
//        flags += b2Draw::e_jointBit;
//        flags += b2Draw::e_aabbBit;
//        flags += b2Draw::e_pairBit;
//        flags += b2Draw::e_centerOfMassBit;
    view->getDebugDraw()->SetFlags(flags);
    
    auto btn = SBButton::create("DebugDraw", FONT_COMMODORE, 30);
    btn->setZoomScale(0.1f);
    btn->setAnchorPoint(ANCHOR_TL);
    btn->setPosition(Vec2TL(10, -20));
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
    tunnelingCountLabel->setPosition(Vec2TL(30, -70));
    view->addChild(tunnelingCountLabel);

    bodyCountLabel = Label::createWithTTF("", FONT_COMMODORE, 30,
                                          Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    bodyCountLabel->setAnchorPoint(ANCHOR_TL);
    bodyCountLabel->setPosition(Vec2TL(30, -120));
    view->addChild(bodyCountLabel);
    
#endif
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
}

/**
 * 맵 초기화
 */
void GameView::initMap() {
    
    auto map = GameMap::create();
    map->setTag(Tag::MAP);
    addChild(map);
    
    GameManager::getPhysicsManager()->setMap(map);
    
    // 스코어 라벨 초기화
    auto scoreLabel = Label::createWithTTF("0", FONT_COMMODORE, 40, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setTag(Tag::SCORE);
    scoreLabel->setAnchorPoint(ANCHOR_MT);
    scoreLabel->setPosition(Vec2TC(0, -20));
    scoreLabel->setTextColor(Color4B::WHITE);
    addChild(scoreLabel);
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
    ballCountLabel = Label::createWithTTF("0", FONT_COMMODORE, 30, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    ballCountLabel->setAnchorPoint(ANCHOR_MB);
    ballCountLabel->setPosition(Vec2BC(0, 20));
    ballCountLabel->setTextColor(Color4B::WHITE);
    addChild(ballCountLabel);
}

/**
 * 타일 초기화
 */
void GameView::initTile() {
    
#if DEBUG_DRAW_TILE
    auto parent = getChildByTag(Tag::DEBUG_DRAW_VIEW);

    for( int x = 0; x < TILE_ROWS; ++x ) {
        for( int y = 0; y < TILE_COLUMNS; ++y ) {
            auto tile = LayerColor::create(Color4B(255,255,0,255*0.3f));
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
 * 조준 컨트롤러 초기화
 */
void GameView::initAimController() {
    
    aimController = AimController::create();
    aimController->setOnShootListener(CC_CALLBACK_1(GameView::shoot, this));
    addChild(aimController, SBZOrder::BOTTOM);
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
    listener->onScoreChanged         = CC_CALLBACK_1(GameView::onScoreChanged, this);
    listener->onLevelChanged         = CC_CALLBACK_1(GameView::onLevelChanged, this);
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
