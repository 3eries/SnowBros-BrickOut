//
//  TileLayer.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/03/2019.
//

#ifndef TileLayer_hpp
#define TileLayer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"
#include "Box2D/Box2D.h"

#include "../GameManager.hpp"

#include "tile/Tile.hpp"
#include "tile/Brick.hpp"
#include "tile/Item.hpp"

class Ball;

/** @class TileLayer
 * @brief 타일을 관리하는 레이어입니다.
 */
class TileLayer : public cocos2d::Node {
public:
    CREATE_FUNC(TileLayer);
    ~TileLayer();
    
private:
    TileLayer();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    
    void initFloorTile(const FloorData &floor);
    void initFloorBrick(const FloorData &floor);
    void initFloorItem(const FloorData &floor);
    void initGameListener();
    
// Game Event
public:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGameOver();
    void onGameContinue();
    
    void onStageChanged(const StageData &stage);
    void onStageClear();
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
public:
    bool        checkStageClear();
    
    Brick*      addBrick(Brick *brick);
    Game::Tile* addTile(Game::Tile *tile);
    Game::Tile* removeTile(Game::Tile *tile);
    
    void        downTile();
    
    void        updateEliteBrickDropRate(const FloorData &floor);
    void        resetRandomEngine();
    
private:
    CC_SYNTHESIZE(std::function<void(BrickList)>, onBrickAddedListener,
                  OnBrickAddedListener);
    CC_SYNTHESIZE(SBCallback, onTileDownFinishedListener,
                  OnTileDownFinishedListener);
    CC_SYNTHESIZE(OnBrickListener, onBrickBreakListener,
                  OnBrickBreakListener);
    
    CC_SYNTHESIZE_READONLY(std::vector<Game::Tile*>, tiles, Tiles);
    
    CC_SYNTHESIZE(int, ballCount, BallCount);
    CC_SYNTHESIZE(int, friendsDamage, FriendsDamage);
    
    struct RandomEngine {
        std::mt19937 brickPosition;
        std::mt19937 eliteBrickDrop;
        std::mt19937 ballCountUp;
        std::mt19937 friendsPowerUp;
    };
    RandomEngine randomEngine;
    
    // 엘리트 벽돌 드랍률
    // 엘리트 벽돌이 생성되지 않은 경우, 다음 층에서 확률 2배 증가
    CC_SYNTHESIZE_READONLY(int, eliteBrickDropRate, EliteBrickDropRate);
    bool isEliteDropped;
    
public:
    /**
     * 타입 T로 캐스팅 할 수 있는 타일을 반환합니다
     */
    template <typename T>
    std::vector<T> getTiles() {        
        std::vector<T> result;

        for( auto tile : tiles ) {
            auto t = dynamic_cast<T>(tile);

            if( t ) {
                result.push_back(t);
            }
        }

        return result;
    }
    
    /**
     * y줄의 타일을 반환합니다
     */
    template <typename T>
    std::vector<T> getTiles(int y) {
        return SBCollection::find(getTiles<T>(), [y](T tile) -> bool {
            return tile->getTilePosition().y == y;
        });
    }
    
    template <typename T>
    bool isExistTile(int y) {
        return getTiles<T>(y).size() > 0;
    }
    
    BrickList                getBricks();
    BrickList                getBricks(const std::string &brickId);
    BrickList                getCanDamageBricks();
    
    ItemList                 getItems();
    ItemList                 getItems(ItemType type);

    Game::Tile*              getTile(const TilePosition &pos);
    bool                     isExistTile(const TilePosition &pos,
                                         int rows, int columns);
    bool                     isRowClear(int y);
    
    TilePositions            getEmptyPositions(int y);
    TilePositions            getEmptyRandomPositions(int y);
};

/** @struct FloorDropData
 * @brief 층에 드랍된 브릭, 가용 칸수 및 좌표를 관리합니다
 */
struct FloorDropData {
    FloorData floor;
    BrickList bricks;                   // 드랍된 브릭 리스트
    
    int availableTileCount;             // 가용 칸수
    TilePositions availablePositions;   // 가용 좌표
    TilePositions randomPositions;      // 랜덤 가용 좌표
    
    FloorDropData(const FloorData &floor);
    
    bool         isNoSpace();
    bool         checkSpace(const BrickData &data);
    
    TilePosition getAvailablePosition(const BrickData &data);
    bool         isAvailablePosition(const TilePosition &pos);
    
    void dropped(Brick *brick);
};

#endif /* TileLayer_hpp */
