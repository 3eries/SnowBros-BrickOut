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
#include "PatternData.h"

struct StageData {
    int            stage;                   // 스테이지 번호
    int            originStage;
    
    FloorList      floors;
    PatternDataMap patterns;
    
    // 스테이지에 등장한 브릭 리스트
    BrickList normalBrickList;
    BrickList bossBrickList;
    
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
        str += STR_FORMAT("\tstage: %d, originStage: %d\n", stage, originStage);
        str += STR_FORMAT("\tfirstFloor: %d\n", getFirstFloor().floor);
        str += STR_FORMAT("\tlastFloor: %d\n", getLastFloor().floor);
        str += STR_FORMAT("\tfloorLen: %d\n", (int)floors.size());
        
        str += "\tfloors:\n";
        
        for( auto floor : floors ) {
            str += floor.toString() + "\n";
        }
        
        str += "\tpatterns:\n";
        
        for( auto e : patterns ) {
            str += e.second.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageList;

#endif /* StageData_h */
