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
#include "GameDefine.h"

#include "event/GameEventDispatcher.hpp"
#include "GamePhysics.hpp"

#include "DBManager.hpp"

class GameScene;
class GameView;

/** @class GameManager
 * @brief 이 클래스는 게임 공통 데이터를 관리하고 상태 전환을 담당합니다
 * 별도의 초기화는 필요 없습니다.
 * 싱글톤이지만 GameScene을 벗어날 경우 destroyInstance 호출로 인스턴스가 제거되도록 설계되었습니다
 */
class GameManager {
public:
    static GameManager* getInstance();
    static void destroyInstance();
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
    
    // Item
    void            setGiftItems(std::vector<GiftRewardItem> items);
    void            addGiftItem(GiftRewardItem item);
    GiftRewardItem  getGiftItem(GiftType type);
    bool            hasGiftItem(GiftType type);
    
    // Score
    static void     setScore(int score);
    static void     addScore(int score);
    
    // Getter
    static GameEventDispatcher* getEventDispatcher();
    static PhysicsManager*      getPhysicsManager();
    static GameConfiguration*   getConfig();
    static int                  getPlayCount();
    static int                  getScore();
    static LevelData            getLevel();
    static StageData            getStage();
    static bool                 isContinuable();
    
private:
    GameEventDispatcher *eventDispatcher;
    PhysicsManager *physicsManager;
    
    GameConfiguration *config;
    CC_SYNTHESIZE_READONLY(GameState, state, State);              // 게임 상태
    CC_SYNTHESIZE(GameView*, view, View);                         // 게임 View
    
    GiftRewardItems giftItems;
    
    int score;                                                    // 스코어
    LevelData level;                                              // 레벨
    int stage;                                                    // 스테이지
    CC_SYNTHESIZE_READONLY(int, continueCount, ContinueCount);    // 이어하기한 횟수
    
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
    
    static void onScoreChanged();
    static void onLevelChanged();
    static bool onNextStage();
};

#endif /* GameManager_hpp */
