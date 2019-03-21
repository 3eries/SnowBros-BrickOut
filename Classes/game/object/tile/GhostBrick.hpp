//
//  GhostBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 28/02/2019.
//

#ifndef GhostBrick_hpp
#define GhostBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class GhostBrick : public Brick {
public:
    static GhostBrick* create(const BrickDef &def);
    ~GhostBrick();
    
private:
    GhostBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void onFloorChanged(const FloorData &floor) override;
    void onNextFloor(const FloorData &floor) override;
    
    void updateOpacity();
    
private:
    SB_SYNTHESIZE_READONLY_BOOL(ghostState, GhostState);
};

#endif /* GhostBrick_hpp */
