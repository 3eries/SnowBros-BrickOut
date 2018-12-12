//
//  LevelData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef LevelData_h
#define LevelData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "StageData.h"

struct LevelData {
    int            level;
    StageList      stages;
    
    LevelData() : level(0) {}
    
    std::string toString() {
        std::string str = "LevelData {\n";
        str += STR_FORMAT("\tlevel: %d\n", level);
        
        for( auto stage : stages ) {
            str += stage.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<LevelData>           LevelList;

#endif /* LevelData_h */
