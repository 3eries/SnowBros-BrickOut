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
public:
    static Brick*   createBrick(const BrickDef &def);
    static Item*    createItem(const ItemDef &def);
};

#endif /* TileFactory_hpp */
