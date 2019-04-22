//
//  SpecialBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef SpecialBrick_hpp
#define SpecialBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class SpecialBrick : public Brick {
protected:
    SpecialBrick(const BrickDef &def);
    virtual ~SpecialBrick();
    
    virtual bool init() override;
    virtual void onEnter() override;
    
    virtual void onFloorChanged(const FloorData &floor) override;
    virtual void onNextFloor(const FloorData &floor) override;
    
public:
    virtual void setSpecialState(bool specialState);
    virtual void updateSpecialState();
    
    virtual bool canDamage() override;
    
private:
    // 특수 상태
    SB_SYNTHESIZE_READONLY_BOOL(specialState, SpecialState);
};

#endif /* SpecialBrick_hpp */
