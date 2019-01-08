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

#define BRICK_FILE              (DIR_DB + "brick.json")
#define BOSS_PATTERN_FILE       (DIR_DB + "boss_pattern.json")

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
            const rapidjson::Value &v = list[i];
            
            BrickData brick(v);
            brick.idleAnims = ResourceHelper::getBrickIdleAnimationFiles(brick.image);
            brick.damageAnims = ResourceHelper::getBrickDamageAnimationFiles(brick.image);
            
            bricks[brick.brickId] = brick;
            
            CCLOG("%s", brick.toString().c_str());
        }
    }
    CCLOG("========== PARSE END (brick.json)  ==========");

    // boss_pattern.json
    CCLOG("========== PARSE START (boss_pattern.json)  ==========");
    {
        string json = SBStringUtils::readTextFile(BOSS_PATTERN_FILE);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        for( auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it ) {
            const auto v = it->value.GetObject();
            
            BossPatternData bossPattern;
            bossPattern.bossBrick = getBrick(it->name.GetString());
            bossPattern.friendBrick = getBrick(v["friend"].GetString());
            
            if( v.HasMember("empty_positions") ) {
                bossPattern.parseEmptyPositions(v["empty_positions"]);
            }
            
            bossPatterns[bossPattern.bossBrick.brickId] = bossPattern;
            
            CCLOG("%s", bossPattern.toString().c_str());
        }
    }
    CCLOG("========== PARSE END (boss_pattern.json)  ==========");
    
    // stage.json
    int stageIdx = 0;
    
    while( true ) {
        string fileName = STR_FORMAT("stage_%04d.json", stageIdx+1);
        string filePath = DIR_DB + fileName;
        
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
        stage.stage = doc["stage"].GetInt();
        
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
            
            // boss_brick_id
            if( floorValue.HasMember("boss_brick_id") ) {
                floor.bossBrick = getBrick(floorValue["boss_brick_id"].GetString());
                floor.bossPattern = getBossPattern(floor.bossBrick.brickId);
            }
            
            stage.floors.push_back(floor);
            floors.push_back(floor);
            
            prevFloor = floor;
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
 * 임시 스테이지 추가
 */
void DBManager::addTempStage() {
    
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
FloorList DBManager::getFloors() {
    return getInstance()->floors;
}

FloorData DBManager::getFloor(int floor) {
    
    auto floors = getFloors();
    
    for( auto data : floors ) {
        if( data.floor == floor ) {
            return data;
        }
    }
    
    CCASSERT(false, "DBManager::getFloor error: invalid floor.");
    return FloorData();
}

bool DBManager::isLastFloor(int floor) {
    
    auto floors = getFloors();
    return floors[floors.size()-1].floor == floor;
}

FloorList DBManager::getStageFloors(int stage) {
    return getStage(stage).floors;
}

bool DBManager::isStageLastFloor(int stage, int floor) {
    return getStage(stage).getLastFloor().floor == floor;
}

bool DBManager::isStageLastFloor(const FloorData &data) {
    return isStageLastFloor(data.stage, data.floor);
}

/**
 * 보스 패턴 데이터를 반환합니다
 */
BossPatternData DBManager::getBossPattern(const string &bossBrickId) {
    
    auto patterns = getInstance()->bossPatterns;
    auto it = patterns.find(bossBrickId);
    
    if( it == patterns.end() ) {
        CCASSERT(false, "DBManager::getBossPattern error.");
    }
    
    return it->second;
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
