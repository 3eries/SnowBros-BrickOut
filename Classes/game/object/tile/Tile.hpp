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
#include "../../GameDefine.h"

namespace Game {
    
/** @class Tile
 * @brief 타일은 맵의 기본 구성 단위입니다
 * 벽돌, 아이템 등 맵에 표시되는 오브젝트는 Tile 클래스를 상속합니다
 */
class Tile : public cocos2d::Node, public SBPhysicsObject {
public:
    static const float ENTER_DURATION;
    static const float MOVE_DURATION;
    
public:
    Tile(int rows, int columns);
    virtual ~Tile();
    
protected:
    virtual bool init() override;
    virtual void initPhysics() override;
    
public:
    virtual void enterWithAction();
    virtual void removeWithAction();
    
    virtual void down();
    
public:
    virtual void setTilePosition(const TilePosition &tilePos, bool action = true);
    virtual bool isContainsPosition(const TilePosition &p);
    
    static cocos2d::Vec2 convertToTilePosition(int x, int y, int w = 1, int h = 1);
    static cocos2d::Vec2 convertToTilePosition(const TilePosition &p, int w = 1, int h = 1);
    
protected:
    CC_SYNTHESIZE_READONLY(int, rows, Rows);        // 가로칸
    CC_SYNTHESIZE_READONLY(int, columns, Columns);  // 세로칸
    CC_SYNTHESIZE_READONLY(TilePosition, tilePos, TilePosition);
};
    
} // namespace Game

#endif /* Tile_hpp */
