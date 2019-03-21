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

#include "model/BallSkinData.h"
#include "model/BrickData.h"
#include "model/PatternData.h"
#include "model/StageData.h"
#include "model/ItemData.h"
#include "model/FriendData.h"
#include "model/DemoData.h"

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
    
    void parseBallSkinJson();
    void parseBrickJson();
    void parseStageJson();
    void parseFriendsJson();
    void parseDemoJson();
    
public:
    static void              addTempStage();
    
    static BallSkinDataList  getBallSkins();
    static BallSkinData      getBallSkin(const std::string &ballId);
    
    static StageDataList     getStages();
    static StageData         getStage(int stage);
    static StageData         getFirstStage();
    static StageData         getLastStage();
    static bool              isLastStage(int stage);
    
    static FloorDataList     getFloors();
    static FloorData         getFloor(int floor);
    static bool              isLastFloor(int floor);
    
    static FloorDataList     getStageFloors(int stage);
    static bool              isStageLastFloor(int stage, int floor);
    static bool              isStageLastFloor(const FloorData &data);
    
    static BrickDataMap      getBricks();
    static BrickData         getBrick(const std::string &brickId);
    
    static FriendDataList    getFriends();
    static FriendData        getFriend(const std::string &friendId);
    
    static DemoStageDataList getDemoStages();
    static DemoStageData     getDemoStage(int stage);
    
private:
    BallSkinDataList ballSkins;
    StageDataList stages;
    FloorDataList floors;
    BrickDataMap bricks;
    FriendDataList friends;
    DemoStageDataList demoStages;
};

#endif /* Database_hpp */
