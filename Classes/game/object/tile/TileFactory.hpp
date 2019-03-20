//
//  TileFactory.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/02/2019.
//

#ifndef TileFactory_hpp
#define TileFactory_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "Brick.hpp"
#include "Item.hpp"

class TileFactory {
// Brick
public:
    static Brick*          createBrick(const BrickDef &def);
    static Brick*          createBrick(const PatternBrickData &patternData,
                                       const FloorData &floor);
    static Brick*          createNormalBrick(const BrickData &brickData,
                                             const TileData &tileData,
                                             const FloorData &floor);
    static Brick*          createEliteBrick(const BrickData &brickData,
                                            const TileData &tileData,
                                            const FloorData &floor);
    static Brick*          createSpecialBrick(const BrickData &brickData,
                                              const TileData &tileData,
                                              const FloorData &floor);
    static PatternDataList createNeutralBrickPatterns(BrickDataList neutralBricks);
    
// Item
public:
    static Item*    createItem(const ItemDef &def);
};

#endif /* TileFactory_hpp */
