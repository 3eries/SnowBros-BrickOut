//
//  Item.hpp
//  SnowBrosItemOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef Item_hpp
#define Item_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Tile.hpp"

#include "../../db/ItemData.h"

class Item : public Game::Tile {
public:
    static Item* create(const ItemData &data);
    virtual ~Item();
    
protected:
    Item(const ItemData &data);
    
    virtual bool init() override;
    virtual void initPhysics() override;
    
    virtual cocos2d::Size getPhysicsSize() override;
    
public:
    virtual void enterWithAction() override;
    virtual void removeWithAction() override;
    
protected:
    CC_SYNTHESIZE_READONLY(ItemData, data, Data);
    
    SBAnimationSprite *image;
};
#endif /* Item_hpp */
