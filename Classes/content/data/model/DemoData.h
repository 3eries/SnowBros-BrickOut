//
//  DemoData.h
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 12/03/2019.
//

#ifndef DemoData_h
#define DemoData_h

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "StageData.h"
#include "BrickData.h"

struct DemoBrickData {
    BrickData brick;
    cocos2d::Vec2 pos;
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

typedef std::vector<DemoStageData> DemoStageDataList;

#endif /* DemoData_h */
