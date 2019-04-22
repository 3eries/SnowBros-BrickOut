//
//  GhostBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 28/02/2019.
//

#ifndef GhostBrick_hpp
#define GhostBrick_hpp

#include <stdio.h>

#include "SpecialBrick.hpp"

class GhostBrick : public SpecialBrick {
public:
    static GhostBrick* create(const BrickDef &def);
    ~GhostBrick();
    
private:
    GhostBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void onFloorChanged(const FloorData &floor) override;
    void onNextFloor(const FloorData &floor) override;
    
public:
    void setSpecialState(bool specialState) override;
    void updateSpecialState() override;
};

#endif /* GhostBrick_hpp */
