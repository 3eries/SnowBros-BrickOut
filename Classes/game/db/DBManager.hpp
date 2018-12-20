//
//  DBManager.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef DBManager_hpp
#define DBManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "StageData.h"
#include "BrickData.h"
#include "ItemData.h"

/** @class DBManager
 * @brief 이 클래스는 게임 DB를 관리합니다
 */
class DBManager {
public:
    static DBManager* getInstance();
    static void destroyInstance();
    ~DBManager();
    
    void init();
    
private:
    DBManager();
    
public:
    static void         addTempStage();
    
    static StageList    getStages();
    static StageData    getStage(int stage);
    static StageData    getLastStage();
    static bool         isLastStage(int stage);
    
    static FloorList    getFloors(int stage);
    static FloorData    getFloor(int stage, int floor);
    static bool         isLastFloor(int stage, int floor);
    static bool         isLastFloor(const FloorData &data);
    
    static BrickMap     getBricks();
    static BrickData    getBrick(const std::string &brickId);
    
private:
    StageList stages;
    BrickMap bricks;
};

#endif /* DBManager_hpp */
