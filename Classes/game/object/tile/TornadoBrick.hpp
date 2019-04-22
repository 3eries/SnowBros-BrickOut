//
//  TornadoBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/04/2019.
//

#ifndef TornadoBrick_hpp
#define TornadoBrick_hpp

#include <stdio.h>

#include "SpecialBrick.hpp"

class TornadoBrick : public SpecialBrick {
public:
    static TornadoBrick* create(const BrickDef &def);
    ~TornadoBrick();
    
private:
    TornadoBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void onFloorChanged(const FloorData &floor) override;
    void onNextFloor(const FloorData &floor) override;
    void onPreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
    
    void checkCollision(float dt);
    
public:
    void setSpecialState(bool specialState) override;
    void updateSpecialState() override;
    
    void prepareRemove() override;
    
private:
    std::vector<Ball*> contactBalls;
};

#endif /* TornadoBrick_hpp */
