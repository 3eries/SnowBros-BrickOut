//
//  TileData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 12/03/2019.
//

#ifndef TileData_h
#define TileData_h

#include "cocos2d.h"

typedef cocos2d::Vec2 TilePosition;
typedef std::vector<TilePosition> TilePositions;
static const cocos2d::Vec2 INVALID_TILE_POSITION(-1,-1);

struct TileData {
    TilePosition pos;
    bool isFlippedX;
    bool isFlippedY;
    
    TileData() : pos(INVALID_TILE_POSITION), isFlippedX(false), isFlippedY(false) {}
    TileData(const TilePosition &_pos) : pos(_pos), isFlippedX(false), isFlippedY(false) {}
    
    std::string toString() {
        return STR_FORMAT("TileData { pos: %d,%d isFlippedX: %d isFlippedY: %d }",
                          (int)pos.x, (int)pos.y, isFlippedX, isFlippedY);
    }
};

#endif /* TileData_h */
