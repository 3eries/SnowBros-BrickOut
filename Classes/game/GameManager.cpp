//
//  GameManager.cpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#include "GameManager.hpp"

#include "UserDefaultKey.h"
#include "RankingManager.hpp"
#include "TestHelper.hpp"

#include "GameView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static GameManager *instance = nullptr;
GameManager* GameManager::getInstance() {
    
    if( !instance ) {
        instance = new GameManager();
    }
    
    return instance;
}

void GameManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

bool GameManager::isNullInstance() {
    return instance == nullptr;
}

GameManager::GameManager() :
eventDispatcher(new GameEventDispatcher()),
physicsManager(new PhysicsManager()),
config(GameConfiguration::getInstance()) {
    
    reset();
}

GameManager::~GameManager() {
    
    CC_SAFE_DELETE(eventDispatcher);
    CC_SAFE_DELETE(physicsManager);
}

void GameManager::reset() {
 
    CCLOG("GameManager::reset");
    
    state = GameState::NONE;
    view = nullptr;
    giftItems.clear();
    continueCount = 0;
    score = 0;
    stage = 1;
    floor = 1;
    floorInStage = 1;
}

/**
 * 게임 상태를 변경합니다
 */
void GameManager::setState(GameState state) {
    this->state = state;
}

void GameManager::addState(GameState state) {
    this->state = (GameState)(this->state | state);
}

void GameManager::removeState(GameState state) {
    this->state = (GameState)(this->state & ~state);
}

bool GameManager::hasState(GameState state) {
    return (this->state & state) == state;
}

/**
 * 게임 플레이중인지 반환합니다
 */
bool GameManager::isPlaying() {
    
    auto mgr = getInstance();
    
    return mgr->hasState(GameState::STARTED) &&
          !mgr->hasState(GameState::PAUSED) &&
          !mgr->hasState(GameState::GAME_OVER) &&
          !mgr->hasState(GameState::RESULT);
}

/**
 * 게임이 일시정지 되었는지 반환합니다
 */
bool GameManager::isPaused() {
    
    return getInstance()->hasState(GameState::PAUSED);
}

/**
 * 시작 스테이지를 설정합니다
 */
void GameManager::setStartStage(int stage) {
    
    this->stage = stage;
    this->floor = getStage().getFirstFloor().floor;
}

/**
 * 선물 아이템을 설정합니다
 */
void GameManager::setGiftItems(vector<GiftRewardItem> items) {
    
    giftItems = items;
}

void GameManager::addGiftItem(GiftRewardItem item) {
    
    CCASSERT(item.type != GiftType::NONE, "GameManager::addGiftItem error.");
    giftItems.push_back(item);
}

GiftRewardItem GameManager::getGiftItem(GiftType type) {
    
    auto items = SBCollection::find(giftItems, [type](GiftRewardItem item) -> bool {
        return item.type == type;
    });
    
    if( items.size() > 0 ) {
        return items[0];
    }
    
    return GiftRewardItem();
}

bool GameManager::hasGiftItem(GiftType type) {
    
    return getGiftItem(type).type != GiftType::NONE;
}

/**
 * 스코어를 설정합니다
 */
void GameManager::setScore(int score) {
    
    auto mgr = getInstance();
    
    // 스코어 설정
    score = MIN(GAME_CONFIG->getMaxScore(), score);
    mgr->score = score;
    
    onScoreChanged();
}

void GameManager::addScore(int score) {
    
    setScore(getInstance()->getScore() + score);
}

/**
 * 브릭을 추가합니다
 */
void GameManager::addBrick(const BrickData &brick) {
    
    auto list = SBCollection::find(bricks, [brick](const BrickData &b) -> bool {
        return brick.brickId == b.brickId;
    });
    
    // 없을때만 추가
    if( list.size() == 0 ) {
        bricks.push_back(brick);
    }
}

GameEventDispatcher* GameManager::getEventDispatcher() {
    return getInstance()->eventDispatcher;
}

PhysicsManager* GameManager::getPhysicsManager() {
    return instance->physicsManager;
}

int GameManager::getPlayCount() {
    return UserDefault::getInstance()->getIntegerForKey(UserDefaultKey::PLAY_COUNT, 0);
}

int GameManager::getScore() {
    return instance->score;
}

StageData GameManager::getStage() {
    return Database::getStage(getInstance()->stage);
}

FloorData GameManager::getFloor() {
    return Database::getFloor(getInstance()->floor);
}

int GameManager::getFloorInStage() {
    return instance->floorInStage;
}

bool GameManager::isStageLastFloor() {
    return Database::isStageLastFloor(getFloor());
}

bool GameManager::isContinuable() {
    return instance->getContinueCount() == 0;
}

#pragma mark- Game Event

/**
 * 게임 진입
 */
void GameManager::onGameEnter() {
    
    // log
    {
        string log;
        log += "\t" + STR_FORMAT("play count: %d", getPlayCount()) + "\n";
        log += "\t" + STR_FORMAT("best score: %d", RankingManager::getBestScore()) + "\n";

        CCLOG("GameManager::onGameEnter\n{\n%s}", log.c_str());
    }
    
    getInstance()->setState(GameState::ENTERED);
    getEventDispatcher()->dispatchOnGameEnter();
}

/**
 * 게임 퇴장
 */
void GameManager::onGameExit() {
    
    CCLOG("GameManager::onGameExit");
    
    onGamePause();
    
    getPhysicsManager()->stopScheduler();
    getPhysicsManager()->removeBodies();
    
    getInstance()->reset();
    getInstance()->setState(GameState::EXITED);
    getEventDispatcher()->dispatchOnGameExit();
}

/**
 * 게임 리셋
 */
void GameManager::onGameReset() {
    
    CCLOG("GameManager::onGameReset");
    
    getPhysicsManager()->startScheduler();
    
    getEventDispatcher()->dispatchOnGameReset();
    onStageChanged();
}

/**
 * 게임 시작
 */
void GameManager::onGameStart() {
    
    CCLOG("GameManager::onGameStart start");
    
    onGameReset();
    
    UserDefault::getInstance()->setIntegerForKey(UserDefaultKey::PLAY_COUNT, getPlayCount()+1);
    UserDefault::getInstance()->flush();
    
    getInstance()->setState(GameState::STARTED);
    getEventDispatcher()->dispatchOnGameStart();
    
    // 부스트 발동
    if( getInstance()->hasGiftItem(GiftType::BOOST) ) {
        onBoostStart();
    }
    
    CCLOG("GameManager::onGameStart end");
}

/**
 * 게임 재시작
 */
void GameManager::onGameRestart() {
    
    CCLOG("GameManager::onGameRestart start");
    
    getInstance()->reset();
    getEventDispatcher()->dispatchOnGameRestart();
    
    onGameStart();
    
    CCLOG("GameManager::onGameRestart end");
}

/**
 * 게임 일시정지
 */
void GameManager::onGamePause() {
    
    CCLOG("GameManager::onGamePause state is paused: %d", getInstance()->hasState(GameState::PAUSED));
    
    if( getInstance()->hasState(GameState::PAUSED) ) {
        return;
    }
    
    getPhysicsManager()->pauseScheduler();
    
    getInstance()->addState(GameState::PAUSED);
    getEventDispatcher()->dispatchOnGamePause();
}

/**
 * 게임 재게
 */
void GameManager::onGameResume() {
    
    CCLOG("GameManager::onGameResume state is paused: %d", getInstance()->hasState(GameState::PAUSED));
    
    if( !getInstance()->hasState(GameState::PAUSED) ) {
        return;
    }
    
    getPhysicsManager()->resumeScheduler();
    
    getInstance()->removeState(GameState::PAUSED);
    getEventDispatcher()->dispatchOnGameResume();
}

/**
 * 게임 오버
 */
void GameManager::onGameOver() {
    
    CCLOG("GameManager::onGameOver");
    
    CCASSERT(getInstance()->hasState(GameState::STARTED), "GameManager::onGameOver invalid called.");
    
    getPhysicsManager()->stopScheduler();
    
    getInstance()->addState(GameState::GAME_OVER);
    getEventDispatcher()->dispatchOnGameOver();
}

/**
 * 게임 이어하기
 */
void GameManager::onGameContinue() {
   
    CCLOG("GameManager::onGameContinue start");
    
    CCASSERT(getInstance()->hasState(GameState::GAME_OVER), "GameManager::onGameContinue invalid called.");
    
    onGameReset();
    
    getInstance()->continueCount++;
    
    getInstance()->removeState(GameState::GAME_OVER);
    getInstance()->addState(GameState::CONTINUE);
    
    getEventDispatcher()->dispatchOnGameContinue();
    
    CCLOG("GameManager::onGameContinue end");
}

static void logEventGameResult(int score) {
    
    if( score < 1 ) {
        return;
    }
    
    // 스코어 범위 획득 함수
    auto getScoreRange = [](int score) -> string {
        int begin = 1;
        int end   = 100;

        while( true ) {
            if( score >= begin && score <= end ) {
                break;
            }
            
            int gap = (end < 800) ? 100 : 50;
            
            begin = (end + 1);
            end += gap;
        }
        
        return STR_FORMAT("%d ~ %d", begin, end);
    };
    
    // firebase 이벤트 기록
    auto gameMgr = GameManager::getInstance();
    
    firebase::Analytics::EventParams params;
    params[FA_EVENT_PARAM_SCORE] = Value(score);
    params[FA_EVENT_PARAM_SCORE_RANGE] = Value(getScoreRange(score));
    params[FA_EVENT_PARAM_CONTINUE] = Value(gameMgr->getContinueCount() > 0 ? 1 : 0);
    
    // firebase::Analytics::logEvent(FA_EVENT_GAME_RESULT, params);
}

/**
 * 게임 결과
 */
void GameManager::onGameResult() {
    
    CCLOG("GameManager::onGameResult");
    
    CCASSERT(getInstance()->hasState(GameState::GAME_OVER), "GameManager::onGameResult invalid called.");
    
    getInstance()->removeState(GameState::GAME_OVER);
    getInstance()->addState(GameState::RESULT);
    getEventDispatcher()->dispatchOnGameResult();
    
    // 스코어 등록
    const int score = getInstance()->getScore();
    
    if( score > 0 ) {
        // logEventGameResult(score);
        RankingManager::setBestScore(score);
    }
}

/**
 * 부스트 시작
 */
void GameManager::onBoostStart() {
    
    CCASSERT(getInstance()->hasState(GameState::STARTED), "GameManager::onBoostStart invalid called.");
    
    getInstance()->addState(GameState::BOOSTING);
    getEventDispatcher()->dispatchOnBoostStart();
}

/**
 * 부스트 종료
 */
void GameManager::onBoostEnd() {
 
    CCASSERT(getInstance()->hasState(GameState::BOOSTING), "GameManager::onBoostEnd invalid called.");
    
    getInstance()->removeState(GameState::BOOSTING);
    getEventDispatcher()->dispatchOnBoostEnd();
}

/**
 * 스테이지 변경
 */
void GameManager::onStageChanged() {
    
    auto stage = getStage();
    Log::i("GameManager::onStageChanged: %d-%d(%d)", stage.stage, getFloorInStage(), getFloor().floor);
    
    instance->bricks.clear();
    
    getEventDispatcher()->dispatchOnStageChanged(stage);
    onFloorChanged();
}

/**
 * 스테이지 클리어
 */
void GameManager::onStageClear() {
    
    getEventDispatcher()->dispatchOnStageClear();
}

/**
 * 다음 스테이지로 이동
 */
void GameManager::onMoveNextStage() {
 
    // 현재 스테이지가 정의된 마지막 스테이지면 임시 스테이지 추가
    if( Database::isLastStage(getStage().stage) ) {
        Database::addTempStage();
    }
    
    instance->stage++;
    instance->floor++;
    instance->floorInStage = 1;
    
    getEventDispatcher()->dispatchOnMoveNextStage(getStage());
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameManager::onMoveNextStageFinished() {
    
    getEventDispatcher()->dispatchOnMoveNextStageFinished(getStage());
    onStageChanged();
}

/**
 * 층 변경
 */
void GameManager::onFloorChanged() {
    
    auto floor = getFloor();
    Log::i("GameManager::onFloorChanged: %d-%d(%d)", floor.stage, getFloorInStage(), floor.floor);
    
    getEventDispatcher()->dispatchOnFloorChanged(floor);
}

/**
 * 다음 층
 */
void GameManager::onNextFloor() {
    
    FloorData floor;
    
    if( !Database::isStageLastFloor(getFloor()) ) {
        instance->floor++;
        floor = getFloor();
    }
    
    instance->floorInStage++;
    
    Log::i("GameManager::onNextFloor: %d-%d(%d)", getStage().stage, getFloorInStage(), floor.floor);
    
    getEventDispatcher()->dispatchOnNextFloor(floor);
    getEventDispatcher()->dispatchOnFloorChanged(floor);
}

/**
 * 스코어 변경
 */
void GameManager::onScoreChanged() {
    
    getEventDispatcher()->dispatchOnScoreChanged(getInstance()->getScore());
}
