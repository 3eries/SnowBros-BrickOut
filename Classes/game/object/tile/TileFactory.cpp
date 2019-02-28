//
//  TileFactory.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/02/2019.
//

#include "TileFactory.hpp"

#include "Brick_10012.hpp"
#include "ShieldBrick.hpp"
#include "GhostBrick.hpp"

USING_NS_CC;
using namespace std;

Brick* TileFactory::createBrick(const BrickDef &def) {
    
    auto data = def.data;
    
    // by id
    switch( HASH_STR(data.brickId.c_str()) ) {
        case HASH_STR("brick_10012"):       return Brick_10012::create(def);
        default: break;
    }
    
    // by type
    switch( data.type ) {
        case BrickType::SPECIAL_SHIELD:     return ShieldBrick::create(def);
        case BrickType::SPECIAL_GHOST:      return GhostBrick::create(def);
        default: break;
    }
    
    // default
    return Brick::create(def);
}

Item* TileFactory::createItem(const ItemDef &def) {
    
    return Item::create(def);
}
