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

class Ball;
class Brick;
class AimController;

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    enum Tag {
        MAP = 10,
        SCORE = 100,
        
        DEBUG_DRAW_VIEW = 1000,
    };
    
    enum ZOrder {
        TILE = 0,
        BALL,
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
    void initMap();
    void initBall();
    void initTile();
    void initAimController();
    void initTouchListener();
    void initGameListener();
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
    
    void onScoreChanged(int score);
    void onLevelChanged(const LevelData &level);
    
private:
    void onTileAddFinished();               // 1. 타일 추가 완료
    void onTileDownFinished() ;             // 2. 타일 이동 완료
    void onShootingReady();                 // 3. 발사 준비 
    void onShootFinished();                 // 4. 발사 완료
    void onFallFinished();                  // 5. 모든 볼 추락 완료
    
    void onBrickBreak(Brick *brick);
    
    void onPhysicsUpdate();
    void onContactBrick(Ball *ball, Brick *brick);
    void onContactFloor(Ball *ball);
    
private:
    void shoot(const cocos2d::Vec2 &endPosition);
    void withdrawBalls();
    
    void downTile();
    
    void addBall(int count = 1);
    void removeBall(Ball *ball);
    
    void addBrick(int count = 1);
    void addItem(int count);
    
    void addTile(Game::Tile *tile);
    void removeTile(Game::Tile *tile);
    
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
    
    Game::Tile*              getTile(const Game::Tile::Position &pos);
    Game::Tile::Positions    getEmptyPositions(int y);
    bool                     isExistTile(int y);
    
private:
    GameManager *gameMgr;
    b2World *world;
    
    std::vector<Ball*> balls;
    cocos2d::Label*    ballCountLabel;
    int                shootIndex;       // 발사된 볼 인덱스
    int                fallenBallCount;  // 떨어진 볼 갯수
    
    std::vector<Game::Tile*> tiles;      // 타일 리스트
    
    AimController *aimController;
    
    bool isWithdrawEnabled;              // 볼 회수 기능 활성화 여부
    bool isWithdraw;                     // 볼 회수 여부
    
    // Debug
    int             tunnelingCount;
    cocos2d::Label *tunnelingCountLabel;
    
    cocos2d::Label *bodyCountLabel;
};

#endif /* GameView_hpp */
