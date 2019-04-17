//
//  TornadoBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/04/2019.
//

#ifndef TornadoBrick_hpp
#define TornadoBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class TornadoBrick : public Brick {
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
    
    void updateState();
    void checkCollision(float dt);
    
public:
    void prepareRemove() override;
    bool canDamage() override;
    
private:
    SB_SYNTHESIZE_READONLY_BOOL(tornadoState, TornadoState);
    int savedHp;
    
    std::vector<Ball*> contactBalls;
};

#endif /* TornadoBrick_hpp */
