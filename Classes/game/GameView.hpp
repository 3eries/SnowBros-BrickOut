//
//  GameView.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"
#include "Box2D/Box2D.h"

#include "Define.h"
#include "GameManager.hpp"

#include "object/TileLayer.hpp"

class Ball;
class FriendsLayer;
class AimController;

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    enum Tag {
        MAP = 10,
        BTN_BRICK_DOWN,
        WITHDRAW_GUIDE,
        BOOST_EFFECT,
        DEBUG_DRAW_VIEW = 1000,
    };
    
    enum class ZOrder {
        TILE = 0,
        FRIENDS,
        AIM_CONTROLLER = 100,
        BALL,
        MENU,
    };
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    
    void initPhysics();
    void initBg();
    void initMenu();
    void initMap();
    void initAimController();
    void initTile();
    void initFriends();
    void initBall();
    void initTouchListener();
    void initGameListener();
    void initPhysicsListener();
    void initIAPListener();
    
// Game Event
public:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameContinue();
    void onGameRestore(const RestoreData &restoreData);
    void onGameResult();
    
    void onBoostStart();
    void onBoosting();
    void onBoostEnd();
    
    void onStageChanged(const StageData &stage);
    void onStageClear();
    
    void onMoveNextStage(const StageData &stage);
    void onMoveNextStageFinished(const StageData &stage);
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
private:
    void saveRestoreData();
    
    void onTileDownFinished() ;             // 1. 타일 이동 완료
    void onShootingReady();                 // 2. 발사 준비
    void onAimingStart();                   // 3. 조준 시작
    void onAimingEnd();                     // 4. 조준 종료
    void onShootFinished();                 // 5. 발사 완료
    void onAllBallFallFinished();           // 6. 모든 볼 추락 완료
    
    void onFriendsBallFallFinished();
    void onBrickBreak(Brick *brick);
    
    void onPhysicsUpdate();
    void onContactBrick(Ball *ball, Game::Tile *brickTile, cocos2d::Vec2 contactPoint);
    void onContactItem(Ball *ball, Game::Tile *itemTile);
    void onContactWall(Ball *ball);
    void onContactFloor(Ball *ball);
    
private:
    void shoot(const cocos2d::Vec2 &endPosition);
    void shootStop();
    
    void withdrawBall(float delay = 0);
    void checkAutoWithdrawBall(float dt);
    
    void checkWithdrawGuide(float dt);
    void stopWithdrawGuide();
    
    void onClickDownButton();
    
    void eatItem(Item *item, bool isFallAction);
    
    void addBall(int count = 1, bool updateUI = true);
    void addBallFromQueue();
    void removeBall(Ball *ball);
    
    bool isUserBallFallFinished();
    bool isFriendsBallFallFinished();
    
    bool checkBrickDown();
    bool checkStageClear();
    
    void showStageLabel(const StageData &stage);
    void updateBallCountUI();
    
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
private:
    GameManager *gameMgr;
    b2World *world;
    
    // Ball
    std::vector<Ball*> balls;
    cocos2d::Label*    ballCountLabel;
    int                shootIndex;               // 발사된 볼 인덱스
    int                fallenBallCount;          // 떨어진 볼 개수
    bool               isAllBallFallFinished;    // 모든 볼 추락 완료 여부 
    std::vector<cocos2d::Sprite*> addBallQueue;  // 추가돼야 할 볼
    
    AimController *aimController;
    TileLayer *tileLayer;
    FriendsLayer *friendsLayer;
    
    bool isWithdrawEnabled; // 볼 회수 기능 활성화 여부
    bool isWithdraw;        // 볼 회수 여부
    
    // Debug
    int             tunnelingCount;
    cocos2d::Label *tunnelingCountLabel;
    
    cocos2d::Label *bodyCountLabel;
};

#endif /* GameView_hpp */
