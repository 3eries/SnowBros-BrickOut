//
//  WorldData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 03/05/2019.
//

#ifndef WorldData_h
#define WorldData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "StageData.h"

struct WorldData {
    int            world;                   // 월드
    int            originWorld;
    StageDataList  stages;
    
    WorldData() : world(0), originWorld(0) {}
    WorldData(int _world) : world(_world), originWorld(_world) {}
    
    bool isNull() const {
        return world == 0;
    }
    
    void addStage(const StageData &stage) {
        CCASSERT(world == stage.world, "WorldData::addStage error: invalid world.");
        stages.push_back(stage);
    }
    
    StageData getFirstStage() const {
        return stages.size() > 0 ? stages[0] : StageData();
    }
    
    StageData getLastStage() const {
        return stages.size() > 0 ? stages[stages.size()-1] : StageData();
    }
    
    bool isLastStage(int stage) const {
        return getLastStage().stage == stage;
    }
    
    StageData getStage(int stage) const {
        
        for( auto stageData : stages ) {
            if( stageData.stage == stage ) {
                return stageData;
            }
        }
        
        CCASSERT(false, "WorldData::getStage error: invalid stage.");
        return StageData();
    }
    
    std::string toString() {
        std::string str = "WorldData {\n";
        str += STR_FORMAT("\tworld: %d\n", world);
        str += STR_FORMAT("\tfirstStage: %d\n", getFirstStage().stage);
        str += STR_FORMAT("\tlastStage: %d\n", getLastStage().stage);
        str += STR_FORMAT("\tstageLen: %d\n", (int)stages.size());
        
        str += "\tstages:\n";
        
        for( auto stage : stages ) {
            str += stage.toString() + "\n";
        }
        str += "}";
        
        return str;
    }
};

typedef std::vector<WorldData>           WorldDataList;

#endif /* WorldData_h */
