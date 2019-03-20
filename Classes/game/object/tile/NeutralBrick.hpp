//
//  NeutralBrick.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/03/2019.
//

#ifndef NeutralBrick_hpp
#define NeutralBrick_hpp

#include <stdio.h>

#include "Brick.hpp"

class NeutralBrick : public Brick {
public:
    static NeutralBrick* create(BrickDef def);
    ~NeutralBrick();
    
private:
    NeutralBrick(const BrickDef &def);
    
    bool init() override;
    void onEnter() override;
    
    void initBg() override;
    void initHpGage() override;
  
public:
    void runRemoveAction() override;
    void sufferDamage(int damage) override;
};

#endif /* NeutralBrick_hpp */
