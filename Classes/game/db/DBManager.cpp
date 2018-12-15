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

#define LEVEL_FILE              "db/level.json"
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
            
            bricks[brick.brickId] = brick;
            
            CCLOG("%s", brick.toString().c_str());
        }
    }
    CCLOG("========== PARSE END (brick.json)  ==========");
    
    // level.json
    CCLOG("========== PARSE START (level.json)  ==========");
    {
        string json = SBStringUtils::readTextFile(LEVEL_FILE);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            const rapidjson::Value &levelValue = list[i];
            
            LevelData level;
            level.level = levelValue["level"].GetInt();
            level.stageBeginNum = levelValue["stage_begin_num"].GetInt();
            level.stageLen = levelValue["stage_len"].GetInt();
            
            const int STAGE_BEGIN_INDEX = level.stageBeginNum-1;
            
            auto stageValueList = levelValue["stages"].GetArray();
            StageData prevStage;
            
            for( int j = 0; j < stageValueList.Size(); ++j ) {
                const rapidjson::Value &stageValue = stageValueList[j];
                
                StageData stage(prevStage);
                stage.level             = level.level;
                stage.stage             = stageValue["stage"].GetInt();
                
                // brick_hp
                if( stageValue.HasMember("brick_hp") ) {
                    string hp = stageValue["brick_hp"].GetString();
                    hp = SBStringUtils::replaceAll(hp, " ", "");
                    
                    stage.brickHpOrigin = hp;
                }
                
                // 현재 stage 값으로 hp 설정
                if( stage.brickHpOrigin == "stage" ) {
                    stage.brickHp = stage.stage + STAGE_BEGIN_INDEX;
                }
                // 이전 stage 값으로 hp 설정
                else if( stage.brickHpOrigin.find("prev_brick_hp") != string::npos ) {
                    auto arr = SBStringUtils::split(stage.brickHpOrigin, '+');
                    CCASSERT(arr.size() == 2, "DBManager level.json parse error: invalid brick_hp.");
                    
                    stage.brickHp = prevStage.brickHp + SBStringUtils::toNumber<int>(arr[1]);
                }
                
                // brick_drop_count
                if( stageValue.HasMember("brick_drop_count") ) {
                    string dropCount = stageValue["brick_drop_count"].GetString();
                    dropCount = SBStringUtils::replaceAll(dropCount, " ", "");
                    
                    // integer
                    if( SBStringUtils::isInteger(dropCount) ) {
                        stage.brickDropMin = SBStringUtils::toNumber<int>(dropCount);
                        stage.brickDropMax = stage.brickDropMin;
                    }
                    // range
                    else if( dropCount.find("~") != string::npos ) {
                        auto arr = SBStringUtils::split(dropCount, '~');
                        CCASSERT(arr.size() == 2, "DBManager level.json parse error: invalid brick_drop_count.");
                        
                        stage.brickDropMin = SBStringUtils::toNumber<int>(arr[0]);
                        stage.brickDropMax = SBStringUtils::toNumber<int>(arr[1]);
                    }
                }
                
                // brick_list
                auto getBrickList = [&](string key) -> BrickList {
                    
                    auto list = stageValue[SBJSON::value(key, allocator)].GetArray();
                    BrickList bricks;
                    
                    for( int i = 0; i < list.Size(); ++i ) {
                        bricks.push_back(getBrick(list[i].GetString()));
                    }
                    
                    return bricks;
                };
                
                if( stageValue.HasMember("brick_list") )          stage.brickList = getBrickList("brick_list");
                if( stageValue.HasMember("elite_brick_list") )    stage.eliteBrickList = getBrickList("elite_brick_list");
                if( stageValue.HasMember("boss_brick_list") )     stage.bossBrickList = getBrickList("boss_brick_list");
                
                // int values
                {
                    string keys[] = {
                        "elite_brick_drop_rate",
                        "power_up_drop_rate",
                        "friend_power_up_drop_rate",
                        "money_drop_rate",
                    };
                    
                    int *values[] = {
                        &stage.eliteBrickDropRate,
                        &stage.powerUpDropRate,
                        &stage.friendPowerUpDropRate,
                        &stage.moneyDropRate,
                    };
                    
                    for( int i = 0; i < sizeof(keys)/sizeof(string); ++i ) {
                        auto key = SBJSON::value(keys[i], allocator);
                        
                        if( stageValue.HasMember(key) ) {
                            *values[i] = stageValue[key].GetInt();
                        }
                    }
                }
                
                level.stages.push_back(stage);
                prevStage = stage;
            }
            
            // order by stage asc
            sort(level.stages.begin(), level.stages.end(), [](const StageData &s1, const StageData &s2) {
                return s1.stage < s2.stage;
            });
            
            CCASSERT(level.stageLen == level.stages.size(), "DBManager level.json parse error: invalid stage length.");
            levels.push_back(level);
            
            CCLOG("%s", level.toString().c_str());
        }
        
        // order by level asc
        sort(levels.begin(), levels.end(), [](const LevelData &l1, const LevelData &l2) {
            return l1.level < l2.level;
        });
    }
    CCLOG("========== PARSE END (level.json)  ==========");
}

LevelList DBManager::getLevels() {
    return getInstance()->levels;
}

LevelData DBManager::getLevel(int level) {
    
    auto levels = getLevels();
    
    for( auto data : levels ) {
        if( data.level == level ) {
            return data;
        }
    }
    
    CCASSERT(false, "DBManager::getLevel error: invalid level.");
    return LevelData();
}

LevelData DBManager::getLastLevel() {
    
    auto levels = getLevels();
    
    if( levels.size() == 0 ) {
        return LevelData();
    }
    
    return levels[levels.size()-1];
}

StageList DBManager::getStages(int level) {
    return getLevel(level).stages;
}

StageData DBManager::getStage(int level, int stage) {
    
    auto stages = getStages(level);
    CCASSERT(stage-1 < stages.size(), "DBManager::getStage error: invalid stage.");
    
    return stages[stage-1];
}

bool DBManager::isLastStage(int level, int stage) {
    return getLevel(level).stageLen == stage;
}

bool DBManager::isLastStage(const StageData &data) {
    return isLastStage(data.level, data.stage);
}

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
