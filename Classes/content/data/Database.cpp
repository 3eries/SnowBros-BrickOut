//
//  Database.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Database.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "../ContentResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define BALL_SKIN__FILE         (DIR_CONTENT_DATA + "ball_skin.json")
#define BRICK_FILE              (DIR_CONTENT_DATA + "brick.json")
#define FRIENDS_FILE            (DIR_CONTENT_DATA + "friends.json")
#define DEMO_FILE               (DIR_CONTENT_DATA + "demo.json")

static Database *instance = nullptr;
Database* Database::getInstance() {
    
    if( !instance ) {
        instance = new Database();
    }
    
    return instance;
}

void Database::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

Database::Database() {
    
}

Database::~Database() {
    
}

void Database::init() {
    
    parseBallSkinJson();
    parseBrickJson();
    parseStageJson();
    parseFriendsJson();
    parseDemoJson();
}

/**
 * ball_skin.json
 */
void Database::parseBallSkinJson() {
    
    CCLOG("========== PARSE START (ball_skin.json)  ==========");
    string json = SBStringUtils::readTextFile(BALL_SKIN__FILE);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    auto list = doc.GetArray();
    
    for( int i = 0; i < list.Size(); ++i ) {
        const rapidjson::Value &v = list[i];
        
        BallSkinData ballSkin;
        ballSkin.ballId = v["ball_id"].GetString();
        ballSkin.type = (BallSkinType)v["type"].GetInt();
        ballSkin.unlockAmount = v["unlock_amount"].GetInt();
        
        ballSkins.push_back(ballSkin);
        
        CCLOG("%s", ballSkin.toString().c_str());
    }
    
    CCLOG("========== PARSE END (ball_skin.json)  ==========");
}

/**
 * brick.json
 */
void Database::parseBrickJson() {
    
    CCLOG("========== PARSE START (brick.json)  ==========");
    string json = SBStringUtils::readTextFile(BRICK_FILE);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    auto list = doc.GetArray();
    
    for( int i = 0; i < list.Size(); ++i ) {
        const rapidjson::Value &v = list[i];
        
        BrickData brick(v);
        brick.idleAnims = BRICK_IDLE_ANIM_FILES(brick.image);
        brick.damageAnims = BRICK_DAMAGE_ANIM_FILES(brick.image);
        
        if( brick.enterDuration == 0 ) {
            brick.enterDuration = GAME_CONFIG->getTileEnterDuration();
        }
        
        bricks[brick.brickId] = brick;
        
        CCLOG("%s", brick.toString().c_str());
    }
    CCLOG("========== PARSE END (brick.json)  ==========");
}

/**
 * stage_xxxx.json
 */
void Database::parseStageJson() {
    
    int stageIdx = 0;
    
    while( true ) {
        string fileName = STR_FORMAT("stage_%04d.json", stageIdx+1);
        string filePath = DIR_CONTENT_DATA + fileName;
        
        stageIdx++;
        
        if( !FileUtils::getInstance()->isFileExist(filePath) ) {
            break;
        }
        
        CCLOG("========== PARSE START (%s)  ==========", fileName.c_str());
        
        string json = SBStringUtils::readTextFile(filePath);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        
        StageData stage;
        stage.parse(doc, allocator);
        
        // patterns
        auto patternValueList = doc["patterns"].GetArray();
        
        for( int i = 0; i < patternValueList.Size(); ++i ) {
            const rapidjson::Value &patternValue = patternValueList[i];
            
            PatternData pattern;
            pattern.patternId = patternValue["id"].GetString();
            
            auto brickList = patternValue["brick_list"].GetArray();
            
            for( int j = 0; j < brickList.Size(); ++j ) {
                const rapidjson::Value &brickValue = brickList[j];
                
                PatternBrickData brickData;
                brickData.brick = getBrick(brickValue["brick_id"].GetString());
                brickData.hp = brickValue["hp"].GetInt();
                
                auto pos = brickValue["pos"].GetArray();
                brickData.tile.pos = Vec2(pos[0].GetInt(), pos[1].GetInt());
                
                if( brickValue.HasMember("flipped_x") ) brickData.tile.isFlippedX = brickValue["flipped_x"].GetBool();
                if( brickValue.HasMember("flipped_y") ) brickData.tile.isFlippedY = brickValue["flipped_y"].GetBool();
                
                pattern.bricks.push_back(brickData);
            }
            
            if( stage.patterns.find(pattern.patternId) != stage.patterns.end() ) {
                CCASSERT(false, "PatternData parse error: duplicate pattern id.");
            }
            
            stage.patterns[pattern.patternId] = pattern;
        }
        
        // floors
        auto floorValueList = doc["floors"].GetArray();
        FloorData prevFloor;
        
        for( int i = 0; i < floorValueList.Size(); ++i ) {
            const rapidjson::Value &floorValue = floorValueList[i];
            bool usePrevData = floorValue.HasMember("use_prev_data") ? floorValue["use_prev_data"].GetBool() : true;
            
            FloorData floor = usePrevData ? prevFloor : FloorData();
            floor.stage       = stage.stage;
            floor.usePrevData = usePrevData;
            
            floor.parse(floorValue, allocator, prevFloor);
            
            // brick_list
            auto getBrickList = [&](string key) -> BrickDataList {
                
                auto list = floorValue[SBJSON::value(key, allocator)].GetArray();
                BrickDataList bricks;
                
                for( int i = 0; i < list.Size(); ++i ) {
                    bricks.push_back(getBrick(list[i].GetString()));
                }
                
                return bricks;
            };
            
            if( floorValue.HasMember("normal_brick_list") )   floor.normalBrickList = getBrickList("normal_brick_list");
            if( floorValue.HasMember("special_brick_list") )  floor.specialBrickList = getBrickList("special_brick_list");
            
            floor.neutralBrickList.clear();
            
            for( auto brick : floor.specialBrickList ) {
                if( brick.type == BrickType::SPECIAL_NEUTRAL ) {
                    floor.neutralBrickList.push_back(brick);
                }
            }
            
            // pattern
            if( floorValue.HasMember("pattern") ) {
                string patternId = floorValue["pattern"].GetString();
                
                if( patternId != "" ) {
                    if( stage.patterns.find(patternId) == stage.patterns.end() ) {
                        CCASSERT(false, "FloorData parse error: invalid pattern id.");
                    }
                    
                    floor.pattern = stage.patterns[patternId];
                    
                } else {
                    floor.pattern = PatternData();
                }
            }
            
            stage.floors.push_back(floor);
            floors.push_back(floor);
            
            prevFloor = floor;
        }
        
        // 스테이지에 등장한 브릭 리스트
        auto addBrickList = [](BrickDataList &target, BrickDataList src) {
            for( auto brick : src ) {
                auto list = SBCollection::find(target, [=](BrickData targetData) -> bool {
                    return brick.brickId == targetData.brickId;
                });
                
                if( list.size() == 0 ) {
                    target.push_back(brick);
                }
            }
        };
        
        for( auto floor : stage.floors ) {
            addBrickList(stage.normalBrickList, floor.normalBrickList);
            addBrickList(stage.bossBrickList, floor.getBossBrickList());
        }
        
        // order by floor asc
        sort(stage.floors.begin(), stage.floors.end(), [](const FloorData &f1, const FloorData &f2) {
            return f1.floor < f2.floor;
        });
        
        stages.push_back(stage);
        
        CCLOG("%s", stage.toString().c_str());
        CCLOG("========== PARSE END (%s)  ==========", fileName.c_str());
    }
    
    // order by stage asc
    sort(stages.begin(), stages.end(), [](const StageData &s1, const StageData &s2) {
        return s1.stage < s2.stage;
    });
    
    // order by floor asc
    sort(floors.begin(), floors.end(), [](const FloorData &f1, const FloorData &f2) {
        return f1.floor < f2.floor;
    });
}

/**
 * friends.json
 */
void Database::parseFriendsJson() {
    
    CCLOG("========== PARSE START (friends.json)  ==========");
    string json = SBStringUtils::readTextFile(FRIENDS_FILE);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    auto list = doc.GetArray();
    
    for( int i = 0; i < list.Size(); ++i ) {
        const rapidjson::Value &v = list[i];
        
        FriendsPackData packData(v);
        friendsPacks.push_back(packData);
        
        SBCollection::addAll(friends, packData.friends);
        
        CCLOG("%s", packData.toString().c_str());
    }
    
    CCLOG("========== PARSE END (friends.json)  ==========");
}

/**
 * demo.json
 */
void Database::parseDemoJson() {
    
    CCLOG("========== PARSE START (demo.json)  ==========");
    string json = SBStringUtils::readTextFile(DEMO_FILE);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    auto list = doc.GetArray();
    
    for( int i = 0; i < list.Size(); ++i ) {
        const rapidjson::Value &v = list[i];
        
        DemoStageData stage;
        stage.stage = Database::getStage(v["stage"].GetInt());
        
        // bricks
        auto bricks = v["bricks"].GetArray();
        
        for( int j = 0; j < bricks.Size(); ++j ) {
            const rapidjson::Value &brickValue = bricks[j];
            
            DemoBrickData brick;
            brick.brick = Database::getBrick(brickValue["brick_id"].GetString());
            
            auto pos = brickValue["pos"].GetArray();
            brick.pos = Vec2(pos[0].GetInt(), pos[1].GetInt());
            
            if( brickValue.HasMember("flipped_x") ) brick.isFlippedX = brickValue["flipped_x"].GetBool();
            if( brickValue.HasMember("flipped_y") ) brick.isFlippedY = brickValue["flipped_y"].GetBool();
            
            stage.bricks.push_back(brick);
        }
        
        // shooting_angles
        auto shootingAngles = v["shooting_angles"].GetArray();
        
        for( int j = 0; j < shootingAngles.Size(); ++j ) {
            stage.shootingAngles.push_back(shootingAngles[j].GetInt());
        }
        
        demoStages.push_back(stage);
    }
    
    CCLOG("========== PARSE END (demo.json)  ==========");
}

/**
 * 임시 스테이지 추가
 */
void Database::addTempStage() {
    
    auto stage = getLastStage();
    stage.stage++;
    
    FloorData prevFloor;
    
    for( auto &floor : stage.floors ) {
        floor.stage = stage.stage;
        floor.floor += stage.floors.size();
        
        floor.parseBrickHp(prevFloor);
        floor.brickHp *= 1.02f;
        
        auto &floors = getInstance()->floors;
        floors.push_back(floor);
        
        prevFloor = floor;
    }
    
    Log::i("addTempStage:\n%s", stage.toString().c_str());
    
    auto &stages = getInstance()->stages;
    stages.push_back(stage);
}

/**
 * 볼 스킨 데이터를 반환합니다
 */
BallSkinDataList Database::getBallSkins() {
    return instance->ballSkins;
}

BallSkinData Database::getBallSkin(const string &ballId) {
    
    auto skins = getBallSkins();
    
    for( auto ballSkin : skins ) {
        if( ballSkin.ballId == ballId ) {
            return ballSkin;
        }
    }
    
    return BallSkinData();
}

/**
 * 스테이지 데이터를 반환합니다
 */
StageDataList Database::getStages() {
    return getInstance()->stages;
}

StageData Database::getStage(int stage) {
    
    auto stages = getStages();
    
    for( auto data : stages ) {
        if( data.stage == stage ) {
            return data;
        }
    }
    
    CCASSERT(false, "Database::getStage error: invalid stage.");
    return StageData();
}

StageData Database::getFirstStage() {
    
    auto stages = getStages();
    
    if( stages.size() == 0 ) {
        return StageData();
    }
    
    return stages[0];
}

StageData Database::getLastStage() {
    
    auto stages = getStages();
    
    if( stages.size() == 0 ) {
        return StageData();
    }
    
    return stages[stages.size()-1];
}

bool Database::isLastStage(int stage) {
    return getLastStage().stage == stage;
}

/**
 * 층 데이터를 반환합니다
 */
FloorDataList Database::getFloors() {
    return getInstance()->floors;
}

FloorData Database::getFloor(int floor) {
    
    auto floors = getFloors();
    
    for( auto data : floors ) {
        if( data.floor == floor ) {
            return data;
        }
    }
    
    CCASSERT(false, "Database::getFloor error: invalid floor.");
    return FloorData();
}

bool Database::isLastFloor(int floor) {
    
    auto floors = getFloors();
    return floors[floors.size()-1].floor == floor;
}

FloorDataList Database::getStageFloors(int stage) {
    return getStage(stage).floors;
}

bool Database::isStageLastFloor(int stage, int floor) {
    return getStage(stage).getLastFloor().floor == floor;
}

bool Database::isStageLastFloor(const FloorData &data) {
    return isStageLastFloor(data.stage, data.floor);
}

/**
 * 해당 스테이지의 최초 볼 개수를 반환합니다
 */
int Database::getStageFirstBallCount(int stage) {
    
    if( stage == 1 ) {
        return GAME_CONFIG->getFirstBallCount();
    } else {
        return getStage(stage-1).finalBallCount;
    }
}

/**
 * 브릭 데이터를 반환합니다
 */
BrickDataMap Database::getBricks() {
    return instance->bricks;
}

BrickData Database::getBrick(const string &brickId) {
    
    auto bricks = getBricks();
    auto it = bricks.find(brickId);
    
    if( it == bricks.end() ) {
        CCASSERT(false, "Database::getBrick error");
    }
    
    return it->second;
}

/**
 * 프렌즈 데이터를 반환합니다
 */
FriendsPackDataList Database::getFriendsPacks() {
    return instance->friendsPacks;
}

FriendsPackData Database::getFriendsPack(const string &packId) {
    
    auto packs = getFriendsPacks();
    
    for( auto packData : packs ) {
        if( packData.packId == packId ) {
            return packData;
        }
    }
    
    return FriendsPackData();
}

FriendDataList Database::getFriends() {
    return instance->friends;
}

FriendData Database::getFriend(const string &friendId) {
    
    auto friends = getFriends();
    
    for( auto friendData : friends ) {
        if( friendData.friendId == friendId ) {
            return friendData;
        }
    }
    
    return FriendData();
}

/**
 * 데모 데이터를 반환합니다
 */
DemoStageDataList Database::getDemoStages() {
    return instance->demoStages;
}

DemoStageData Database::getDemoStage(int stage) {
    
    auto stages = instance->demoStages;
    
    for( auto stageData : stages ) {
        if( stageData.stage.stage == stage ) {
            return stageData;
        }
    }
    
    return DemoStageData();
}
