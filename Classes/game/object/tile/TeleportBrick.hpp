//
//  TeleportBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/04/2019.
//

#ifndef TeleportBrick_hpp
#define TeleportBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class TeleportBrick : public Brick {
public:
    static TeleportBrick* create(const BrickDef &def);
    ~TeleportBrick();
    
private:
    TeleportBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void onFloorChanged(const FloorData &floor) override;
    void onNextFloor(const FloorData &floor) override;
    
    void teleport();
    
    TilePositions getTeleportTargetPositions();
    
public:
    float getEnterDuration() override;
    
private:
    CC_SYNTHESIZE_READONLY(TilePosition, teleportTargetPos, TeleportTargetPosition);
};

#endif /* TeleportBrick_hpp */
