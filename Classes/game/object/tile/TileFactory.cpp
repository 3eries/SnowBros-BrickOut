//
//  TileFactory.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/02/2019.
//

#include "TileFactory.hpp"

#include "Brick_10012.hpp"

USING_NS_CC;
using namespace std;

Brick* TileFactory::createBrick(const BrickDef &def) {
    
    switch( HASH_STR(def.data.brickId.c_str()) ) {
        case HASH_STR("brick_10012"):   return Brick_10012::create(def);
        default: break;
    }
    
    return Brick::create(def);
}

Item* TileFactory::createItem(const ItemDef &def) {
    
    return Item::create(def);
}
