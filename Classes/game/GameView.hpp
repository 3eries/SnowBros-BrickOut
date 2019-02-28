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

#include "object/tile/Tile.hpp"
#include "object/tile/Brick.hpp"

class Ball;
class Item;
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
        
        DEBUG_DRAW_VIEW = 1000,
    };
    
    enum class ZOrder {
        TILE = 0,
        BALL,
        AIM_CONTROLLER = 100,
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
    void initBall();
    void initTile();
    void initFriends();
    void initAimController();
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
    void onGameResult();
    
    void onBoostStart();
    void onBoostEnd();
    
    void onStageChanged(const StageData &stage);
    void onStageClear();
    
    void onMoveNextStage(const StageData &stage);
    void onMoveNextStageFinished(const StageData &stage);
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
    void onScoreChanged(int score);
    
private:
    void onTileAddFinished();               // 1. 타일 추가 완료
    void onTileDownFinished() ;             // 2. 타일 이동 완료
    void onShootingReady();                 // 3. 발사 준비 
    void onShootFinished();                 // 4. 발사 완료
    void onFallFinished();                  // 5. 모든 볼 추락 완료
    
    void onBrickBreak(Brick *brick);
    
    void onPhysicsUpdate();
    void onContactBrick(Ball *ball, Game::Tile *brickTile, cocos2d::Vec2 contactPoint);
    void onContactItem(Ball *ball, Game::Tile *itemTile);
    void onContactWall(Ball *ball);
    void onContactFloor(Ball *ball);
    
private:
    void shoot(const cocos2d::Vec2 &endPosition);
    void shootStop();
    void withdrawBalls(float delay = 0);
    
    void downTile();
    void onClickDownButton();
    
    void eatItem(Item *item, bool isFallAction);
    
    void addBall(int count = 1, bool updateUI = true);
    void addBallFromQueue();
    void removeBall(Ball *ball);
    
    void addBrick();
    void addItem();
    
    void addTile(Game::Tile *tile);
    void removeTile(Game::Tile *tile);
    
    bool checkStageClear();
    
    void showStageLabel(int stage);
    void updateBallCountUI();
    
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
private:
    std::vector<Game::Tile*> getTiles(int y);
    std::vector<Game::Tile*> getBricks(int y);
    std::vector<Game::Tile*> getBricks();
    std::vector<Brick*>      getBricks(const std::string &brickId);
    std::vector<Game::Tile*> getItems();
    
    Game::Tile*              getTile(const TilePosition &pos);
    TilePositions            getEmptyPositions(int y);
    TilePositions            getEmptyRandomPositions(int y);
    bool                     isExistTile(int y);
    bool                     isExistBrick(int y);
    
private:
    GameManager *gameMgr;
    b2World *world;
    
    // Ball
    std::vector<Ball*> balls;
    cocos2d::Label*    ballCountLabel;
    int                shootIndex;               // 발사된 볼 인덱스
    int                fallenBallCount;          // 떨어진 볼 개수
    std::vector<cocos2d::Sprite*> addBallQueue;  // 추가돼야 할 볼
    
    // Tile
    cocos2d::Node *tileLayer;
    std::vector<Game::Tile*> tiles;
    
    std::mt19937 brickPositionRandomEngine;
    
    FriendsLayer *friendsLayer;
    AimController *aimController;
    
    bool isWithdrawEnabled; // 볼 회수 기능 활성화 여부
    bool isWithdraw;        // 볼 회수 여부
    
    // 엘리트 벽돌 드랍률
    // 엘리트 벽돌이 생성되지 않은 경우, 다음 층에서 확률 2배 증가
    CC_SYNTHESIZE_READONLY(int, eliteBrickDropRate, EliteBrickDropRate);
    bool isEliteDropped;
    
    // Debug
    int             tunnelingCount;
    cocos2d::Label *tunnelingCountLabel;
    
    cocos2d::Label *bodyCountLabel;
};

#endif /* GameView_hpp */
