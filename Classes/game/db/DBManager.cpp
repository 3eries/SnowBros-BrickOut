//
//  DBManager.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "DBManager.hpp"

#include "Define.h"
#include "ResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define STAGE_FILE              "db/stage.json"
#define BRICK_FILE              "db/brick.json"

static DBManager *instance = nullptr;
DBManager* DBManager::getInstance() {
    
    if( !instance ) {
        instance = new DBManager();
    }
    
    return instance;
}

void DBManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

DBManager::DBManager() {
    
}

DBManager::~DBManager() {
    
}

void DBManager::init() {
    
    // parse db json file
    // brick.json
    CCLOG("========== PARSE START (brick.json)  ==========");
    {
        string json = SBStringUtils::readTextFile(BRICK_FILE);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            const rapidjson::Value &brickValue = list[i];
            
            BrickData brick;
            brick.brickId   = brickValue["id"].GetString();
            brick.type      = (BrickType)brickValue["type"].GetInt();
            brick.width     = brickValue["width"].GetInt();
            brick.height    = brickValue["height"].GetInt();
            
            if( brickValue.HasMember("image") ) {
                brick.image = brickValue["image"].GetString();
            }
            
            if( brick.image == "" ) {
                brick.image = brick.brickId;
            }
            
            brick.idleAnims = ResourceHelper::getBrickIdleAnimationFiles(brick);
            brick.damageAnims = ResourceHelper::getBrickDamageAnimationFiles(brick);
            
            bricks[brick.brickId] = brick;
            
            CCLOG("%s", brick.toString().c_str());
        }
    }
    CCLOG("========== PARSE END (brick.json)  ==========");
    
    // stage.json
    CCLOG("========== PARSE START (stage.json)  ==========");
    {
        string json = SBStringUtils::readTextFile(STAGE_FILE);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            const rapidjson::Value &stageValue = list[i];
            
            StageData stage;
            stage.stage = stageValue["stage"].GetInt();
            stage.floorBeginNum = stageValue["floor_begin_num"].GetInt();
            stage.floorLen = stageValue["floor_len"].GetInt();
            
            const int FLOOR_BEGIN_INDEX = stage.floorBeginNum-1;
            
            auto floorValueList = stageValue["floors"].GetArray();
            FloorData prevFloor;
            
            for( int j = 0; j < floorValueList.Size(); ++j ) {
                const rapidjson::Value &floorValue = floorValueList[j];
                
                FloorData floor(prevFloor);
                floor.stage             = stage.stage;
                floor.floor             = floorValue["floor"].GetInt();
                floor.isLastFloor       = floor.floor == stage.floorLen;
                
                // brick_hp
                if( floorValue.HasMember("brick_hp") ) {
                    string hp = floorValue["brick_hp"].GetString();
                    hp = SBStringUtils::replaceAll(hp, " ", "");
                    
                    floor.brickHpOrigin = hp;
                }
                
                // 현재 floor 값으로 hp 설정
                if( floor.brickHpOrigin == "floor" ) {
                    floor.brickHp = floor.floor + FLOOR_BEGIN_INDEX;
                }
                // 이전 floor 값으로 hp 설정
                else if( floor.brickHpOrigin.find("prev_brick_hp") != string::npos ) {
                    auto arr = SBStringUtils::split(floor.brickHpOrigin, '+');
                    CCASSERT(arr.size() == 2, "DBManager stage.json parse error: invalid brick_hp.");
                    
                    floor.brickHp = prevFloor.brickHp + SBStringUtils::toNumber<int>(arr[1]);
                }
                
                // brick_drop_count
                if( floorValue.HasMember("brick_drop_count") ) {
                    string dropCount = floorValue["brick_drop_count"].GetString();
                    dropCount = SBStringUtils::replaceAll(dropCount, " ", "");
                    
                    // integer
                    if( SBStringUtils::isInteger(dropCount) ) {
                        floor.brickDropMin = SBStringUtils::toNumber<int>(dropCount);
                        floor.brickDropMax = floor.brickDropMin;
                    }
                    // range
                    else if( dropCount.find("~") != string::npos ) {
                        auto arr = SBStringUtils::split(dropCount, '~');
                        CCASSERT(arr.size() == 2, "DBManager stage.json parse error: invalid brick_drop_count.");
                        
                        floor.brickDropMin = SBStringUtils::toNumber<int>(arr[0]);
                        floor.brickDropMax = SBStringUtils::toNumber<int>(arr[1]);
                    }
                }
                
                // brick_list
                auto getBrickList = [&](string key) -> BrickList {
                    
                    auto list = floorValue[SBJSON::value(key, allocator)].GetArray();
                    BrickList bricks;
                    
                    for( int i = 0; i < list.Size(); ++i ) {
                        bricks.push_back(getBrick(list[i].GetString()));
                    }
                    
                    return bricks;
                };
                
                if( floorValue.HasMember("brick_list") )          floor.brickList = getBrickList("brick_list");
                if( floorValue.HasMember("elite_brick_list") )    floor.eliteBrickList = getBrickList("elite_brick_list");
                if( floorValue.HasMember("boss_brick_list") )     floor.bossBrickList = getBrickList("boss_brick_list");
                
                if( floor.isExistBoss() ) {
                    stage.bossFloor = floor.floor;
                }
                
                // int values
                {
                    string keys[] = {
                        "elite_brick_drop_rate",
                        "power_up_drop_rate",
                        "friend_power_up_drop_rate",
                        "money_drop_rate",
                    };
                    
                    int *values[] = {
                        &floor.eliteBrickDropRate,
                        &floor.powerUpDropRate,
                        &floor.friendPowerUpDropRate,
                        &floor.moneyDropRate,
                    };
                    
                    for( int i = 0; i < sizeof(keys)/sizeof(string); ++i ) {
                        auto key = SBJSON::value(keys[i], allocator);
                        
                        if( floorValue.HasMember(key) ) {
                            *values[i] = floorValue[key].GetInt();
                        }
                    }
                }
                
                stage.floors.push_back(floor);
                prevFloor = floor;
            }
            
            // order by floor asc
            sort(stage.floors.begin(), stage.floors.end(), [](const FloorData &f1, const FloorData &f2) {
                return f1.floor < f2.floor;
            });
            
            CCASSERT(stage.floorLen == stage.floors.size(), "DBManager stage.json parse error: invalid floor length.");
            stages.push_back(stage);
            
            CCLOG("%s", stage.toString().c_str());
        }
        
        // order by stage asc
        sort(stages.begin(), stages.end(), [](const StageData &s1, const StageData &s2) {
            return s1.stage < s2.stage;
        });
    }
    CCLOG("========== PARSE END (stage.json)  ==========");
}

/**
 * 임시 스테이지 추가
 */
void DBManager::addTempStage() {
    
    auto stage = getLastStage();
    stage.stage++;
    stage.floorBeginNum += stage.floorLen;
    
    for( auto &floor : stage.floors ) {
        floor.stage++;
        floor.brickHp += stage.floorBeginNum-1;
        floor.brickHp *= 1.03f;
    }
    
    Log::i("addTempStage:\n%s", stage.toString().c_str());
    
    auto &stages = getInstance()->stages;
    stages.push_back(stage);
}

/**
 * 스테이지 데이터를 반환합니다
 */
StageList DBManager::getStages() {
    return getInstance()->stages;
}

StageData DBManager::getStage(int stage) {
    
    auto stages = getStages();
    
    for( auto data : stages ) {
        if( data.stage == stage ) {
            return data;
        }
    }
    
    CCASSERT(false, "DBManager::getStage error: invalid stage.");
    return StageData();
}

StageData DBManager::getLastStage() {
    
    auto stages = getStages();
    
    if( stages.size() == 0 ) {
        return StageData();
    }
    
    return stages[stages.size()-1];
}

bool DBManager::isLastStage(int stage) {
    return getLastStage().stage == stage;
}

/**
 * 층 데이터를 반환합니다
 */
FloorList DBManager::getFloors(int stage) {
    return getStage(stage).floors;
}

FloorData DBManager::getFloor(int stage, int floor) {
    
    auto floors = getFloors(stage);
    CCASSERT(floor-1 < floors.size(), "DBManager::getFloor error: invalid floor.");
    
    return floors[floor-1];
}

bool DBManager::isLastFloor(int stage, int floor) {
    return getStage(stage).floorLen == floor;
}

bool DBManager::isLastFloor(const FloorData &data) {
    return isLastFloor(data.stage, data.floor);
}

/**
 * 벽돌 데이터를 반환합니다
 */
BrickMap DBManager::getBricks() {
    return getInstance()->bricks;
}

BrickData DBManager::getBrick(const string &brickId) {
    
    auto bricks = getBricks();
    auto it = bricks.find(brickId);
    
    if( it == bricks.end() ) {
        CCASSERT(false, "DBManager::getBrick error");
    }
    
    return it->second;
}
