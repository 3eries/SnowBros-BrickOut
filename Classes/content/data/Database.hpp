//
//  Database.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "model/BrickData.h"
#include "model/BossPatternData.h"
#include "model/StageData.h"
#include "model/ItemData.h"

/** @class Database
 * @brief 이 클래스는 컨텐츠 데이터를 관리합니다
 */
class Database {
public:
    static Database* getInstance();
    static void destroyInstance();
    ~Database();
    
    void init();
    
private:
    Database();
    
public:
    static void            addTempStage();
    
    static StageList       getStages();
    static StageData       getStage(int stage);
    static StageData       getLastStage();
    static bool            isLastStage(int stage);
    
    static FloorList       getFloors();
    static FloorData       getFloor(int floor);
    static bool            isLastFloor(int floor);
    
    static FloorList       getStageFloors(int stage);
    static bool            isStageLastFloor(int stage, int floor);
    static bool            isStageLastFloor(const FloorData &data);
    
    static BossPatternData getBossPattern(const std::string &bossBrickId);
    
    static BrickMap        getBricks();
    static BrickData       getBrick(const std::string &brickId);
    
private:
    StageList stages;
    FloorList floors;
    BossPatternMap bossPatterns;
    BrickMap bricks;
};

// Temp
void makeBrickJSON();
void makeStageJSON();

#endif /* Database_hpp */
