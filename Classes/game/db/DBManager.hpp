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

#include "LevelData.h"
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
    static void         addTempLevel();
    
    static LevelList    getLevels();
    static LevelData    getLevel(int level);
    static LevelData    getLastLevel();
    static bool         isLastLevel(int level);
    
    static StageList    getStages(int level);
    static StageData    getStage(int level, int stage);
    static bool         isLastStage(int level, int stage);
    static bool         isLastStage(const StageData &data);
    
    static BrickMap     getBricks();
    static BrickData    getBrick(const std::string &brickId);
    
private:
    LevelList levels;
    BrickMap bricks;
};

#endif /* DBManager_hpp */
