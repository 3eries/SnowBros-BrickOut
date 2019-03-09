//
//  DemoData.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#ifndef DemoData_hpp
#define DemoData_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../game/GameDefine.h"
#include "../../content/data/model/StageData.h"
#include "../../content/data/model/BrickData.h"

struct DemoBrickData {
    BrickData brick;
    TilePosition pos;
    bool isFlippedX;
    bool isFlippedY;
    
    DemoBrickData() : isFlippedX(false), isFlippedY(false) {}
};

struct DemoStageData {
    StageData stage;
    std::vector<DemoBrickData> bricks;
    IntList shootingAngles;
    
    DemoStageData() {}
    
    bool isNull() const {
        return stage.isNull();
    }
};

class DemoData {
public:
    static DemoData* getInstance();
    static void destroyInstance();
    ~DemoData();
    
    void init();
    
    std::vector<DemoStageData> getStages();
    DemoStageData              getStage(int stage);
    
private:
    DemoData();
    
    std::vector<DemoStageData> stages;
};

#endif /* DemoData_hpp */
