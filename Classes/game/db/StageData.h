//
//  StageData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef StageData_h
#define StageData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "FloorData.h"

struct StageData {
    int            stage;               // 스테이지 번호
    FloorList      floors;
    
    StageData() : stage(0) {}
    
    bool isNull() const {
        return stage == 0;
    }
    
    FloorData getFirstFloor() const {
        return floors[0];
    }

    FloorData getLastFloor() const {
        return floors[floors.size()-1];
    }
    
    std::string toString() {
        std::string str = "StageData {\n";
        str += STR_FORMAT("\t stage: %d\n", stage);
        str += STR_FORMAT("\t firstFloor: %d\n", getFirstFloor().floor);
        str += STR_FORMAT("\t lastFloor: %d\n", getLastFloor().floor);
        str += STR_FORMAT("\t floorLen: %d\n", (int)floors.size());
        
        for( auto floor : floors ) {
            str += floor.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageList;

#endif /* StageData_h */
