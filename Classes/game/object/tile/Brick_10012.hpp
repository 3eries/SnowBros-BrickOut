//
//  Brick_10012.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/02/2019.
//

#ifndef Brick_10012_hpp
#define Brick_10012_hpp

#include <stdio.h>

#include "Brick.hpp"

class Brick_10012 : public Brick {
public:
    static Brick_10012* create(const BrickDef &def);
     ~Brick_10012();
    
private:
    Brick_10012(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    void cleanup() override;
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
public:
    void setTilePosition(const TilePosition &tilePos, bool action = true,
                         SBCallback onActionFinished = nullptr) override;
    
private:
    CC_SYNTHESIZE(std::vector<Brick*>, parts, Parts);
    int floorChangeCount;
};

#endif /* Brick_10012_hpp */
