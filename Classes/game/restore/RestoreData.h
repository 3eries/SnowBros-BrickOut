//
//  RestoreData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef RestoreData_h
#define RestoreData_h

#include "../../content/data/model/BrickData.h"
#include "../../content/data/model/TileData.h"
#include "../../content/data/model/FloorData.h"

struct RestoreBrickData {
    BrickData brick;
    TileData tile;
    FloorData floor;
    int hp;
    int originHp;
    bool isElite;
    bool isSpecialState;
    
    RestoreBrickData() : isElite(false), isSpecialState(false) {}
};

struct RestoreItemData {
    ItemData item;
    TileData tile;
    FloorData floor;
};

struct RestoreData {
    std::vector<RestoreBrickData> bricks;
    std::vector<RestoreItemData>  items;
    
    int           ballDamage;
    cocos2d::Vec2 ballPosition;
    
    int           friendsBallDamage;
};

#endif /* RestoreData_h */
