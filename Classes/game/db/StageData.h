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
    int            stage;
    int            floorBeginNum;
    int            floorLen;
    FloorList      floors;
    
    StageData() : stage(0) {}
    
    bool isNull() const {
        return stage == 0;
    }
    
    std::string toString() {
        std::string str = "StageData {\n";
        str += STR_FORMAT("\tstage: %d\n", stage);
        str += STR_FORMAT("\tfloorBeginNum: %d\n", floorBeginNum);
        str += STR_FORMAT("\tfloorLen: %d\n", floorLen);
        
        for( auto floor : floors ) {
            str += floor.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageList;

#endif /* StageData_h */
