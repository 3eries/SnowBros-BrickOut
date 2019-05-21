//
//  Tile.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "ContentManager.hpp"
#include "../../GameDefine.h"

NS_GAME_BEGIN
    
/** @class Tile
 * @brief 타일은 맵의 기본 구성 단위입니다
 * 벽돌, 아이템 등 맵에 표시되는 오브젝트는 Tile 클래스를 상속합니다
 */
class Tile : public cocos2d::Node, public SBPhysicsObject {
public:
    Tile(int rows, int columns, const FloorData &floorData);
    virtual ~Tile();
    
protected:
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void cleanup() override;
    virtual void initPhysics() override;
    virtual void initGameListener();
    
    virtual void onStageClear();
    virtual void onFloorChanged(const FloorData &floor);
    virtual void onNextFloor(const FloorData &floor);
    
public:
    virtual void prepareRemove();
    
    virtual void down();
    virtual void onDownFinished();
    
    virtual void enterWithAction();
    virtual void removeWithAction();
    
public:
//    virtual void setTilePosition(const TilePosition &tilePos, bool action = true,
//                                 SBCallback onActionFinished = nullptr);
    virtual void setTilePosition(const TilePosition &tilePos);
    virtual void moveWithAction(const TilePosition &tilePos, float duration,
                                SBCallback onActionFinished = nullptr);
    virtual bool isContainsPosition(const TilePosition &p);
    
    static cocos2d::Vec2 convertToTilePosition(int x, int y, int w = 1, int h = 1);
    static cocos2d::Vec2 convertToTilePosition(const TilePosition &p, int w = 1, int h = 1);
    
protected:
    CC_SYNTHESIZE_READONLY(int, rows, Rows);        // 가로칸
    CC_SYNTHESIZE_READONLY(int, columns, Columns);  // 세로칸
    CC_SYNTHESIZE_READONLY(FloorData, floorData, FloorData);
    CC_SYNTHESIZE_READONLY(TilePosition, tilePos, TilePosition);
    
    // 타일 유효 여부
    SB_SYNTHESIZE_READONLY_BOOL(available, Available);
    
    // 층 변경 횟수, onNextFloor 호출 시 업데이트
    CC_SYNTHESIZE_READONLY(int, floorChangedCount, FloorChangedCount);
};
    
NS_GAME_END

#endif /* Tile_hpp */
