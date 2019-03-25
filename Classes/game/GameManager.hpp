//
//  GameManager.hpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "Define.h"
#include "GiftDefine.h"
#include "GameConfiguration.hpp"
#include "ContentManager.hpp"
#include "GameDefine.h"

#include "event/GameEventDispatcher.hpp"
#include "GamePhysics.hpp"

class GameScene;
class GameView;

#define GAME_MANAGER    GameManager::getInstance()

/** @class GameManager
 * @brief 이 클래스는 게임 공통 데이터를 관리하고 상태 전환을 담당합니다
 * 별도의 초기화는 필요 없습니다.
 * 싱글톤이지만 GameScene을 벗어날 경우 destroyInstance 호출로 인스턴스가 제거되도록 설계되었습니다
 */
class GameManager {
public:
    static GameManager* getInstance();
    static void destroyInstance();
    static bool isNullInstance();
    ~GameManager();
    
private:
    GameManager();
    
    void reset();

public:
    // Game State
    void            setState(GameState state);
    void            addState(GameState state);
    void            removeState(GameState state);
    bool            hasState(GameState state);
    static bool     isPlaying();
    static bool     isPaused();
    
    // Stage
    void            setStartStage(int stage);
    
    // Item
    void            setGiftItems(std::vector<GiftRewardItem> items);
    void            addGiftItem(GiftRewardItem item);
    GiftRewardItem  getGiftItem(GiftType type);
    bool            hasGiftItem(GiftType type);
    
    // Score
    static void     setScore(int score);
    static void     addScore(int score);
    
    // Bick
    void            addBrick(const BrickData &brick);
    
    // Getter
    static GameEventDispatcher* getEventDispatcher();
    static PhysicsManager*      getPhysicsManager();
    static int                  getPlayCount();
    static int                  getScore();
    
    static StageData            getStage();
    static FloorData            getFloor();
    static int                  getFloorInStage();
    static bool                 isStageLastFloor();
    
    static bool                 isContinuable();
    
private:
    GameEventDispatcher *eventDispatcher;
    PhysicsManager *physicsManager;
    
    GameConfiguration *config;
    CC_SYNTHESIZE_READONLY(GameState, state, State);              // 게임 상태
    CC_SYNTHESIZE(GameView*, view, View);                         // 게임 View
    
    GiftRewardItems giftItems;
    
    int stage;                                                    // 스테이지
    int floor;                                                    // 층, 전체 기준
    int floorInStage;                                             // 층, 현재 스테이지 기준
    int score;                                                    // 스코어
    CC_SYNTHESIZE_READONLY(int, continueCount, ContinueCount);    // 이어하기한 횟수
    
    // 스테이지에 등장한 브릭 리스트, 스테이지 전환 시 리셋
    CC_SYNTHESIZE_READONLY(BrickDataList, bricks, Bricks);
                           
// Game Event
public:
    static void onGameEnter();
    static void onGameExit();
    static void onGameReset();
    static void onGameStart();
    static void onGameRestart();
    static void onGamePause();
    static void onGameResume();
    static void onGameOver();
    static void onGameContinue();
    static void onGameResult();
    
    static void onBoostStart();
    static void onBoostEnd();

    static void onStageChanged();
    static void onStageClear();
    
    static void onMoveNextStage();
    static void onMoveNextStageFinished();
    
    static void onFloorChanged();
    static void onNextFloor();
    
    static void onScoreChanged();
};

#endif /* GameManager_hpp */
