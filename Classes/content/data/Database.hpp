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
#include "model/WorldData.h"
#include "model/StageData.h"
#include "model/ItemData.h"
#include "model/FriendsPackData.h"
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
    static void                addTempWorld();
    
    static BallSkinDataList    getBallSkins();
    static BallSkinData        getBallSkin(const std::string &ballId);
    
    static WorldDataList       getWorlds();
    static int                 getOriginWorldCount();
    static WorldData           getWorld(int world);
    static WorldData           getFirstWorld();
    static WorldData           getLastWorld();
    static bool                isLastWorld(int world);
    
    static StageData           getStage(int world, int stage);
    static StageData           getWorldLastStage(int world);
    static StageData           getStageBySeq(int stageSeq);
    
    static BrickDataMap        getBricks();
    static BrickData           getBrick(const std::string &brickId);
    static BrickData           getFirstBrick();
    
    static FriendsPackDataList getFriendsPacks();
    static FriendsPackData     getFriendsPack(const std::string &packId);
    
    static FriendDataList      getFriends();
    static FriendData          getFriend(const std::string &friendId);
    
    static DemoStageDataList   getDemoStages();
    static DemoStageData       getDemoStage(int stage);
    
private:
    BallSkinDataList ballSkins;
    
    WorldDataList worlds;
    int originWorldCount;
    
    BrickDataMap bricks;
    
    FriendsPackDataList friendsPacks;
    FriendDataList friends;
    
    DemoStageDataList demoStages;
};

#endif /* Database_hpp */
