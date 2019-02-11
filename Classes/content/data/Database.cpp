//
//  Database.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Database.hpp"

#include "Define.h"
#include "../ContentResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define BRICK_FILE              (DIR_DATA + "brick.json")
#define BOSS_PATTERN_FILE       (DIR_DATA + "boss_pattern.json")

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
    
    // makeBrickJSON();
    // makeStageJSON();
    
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
            brick.idleAnims = ContentResourceHelper::getBrickIdleAnimationFiles(brick.image);
            brick.damageAnims = ContentResourceHelper::getBrickDamageAnimationFiles(brick.image);
            
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
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            const rapidjson::Value &v = list[i];
            
            BossPatternData bossPattern;
            bossPattern.bossBrickId = v["id"].GetString();
            
            auto brickList = v["brick_list"].GetArray();
            
            for( int j = 0; j < brickList.Size(); ++j ) {
                const rapidjson::Value &brickValue = brickList[j];
                
                BossPatternBrickData brickData;
                brickData.brick = getBrick(brickValue["brick_id"].GetString());
                brickData.hp = brickValue["hp"].GetInt();
                
                auto pos = brickValue["pos"].GetArray();
                brickData.pos = Vec2(pos[0].GetInt(), pos[1].GetInt());
                
                if( brickValue.HasMember("flipped_x") ) brickData.isFlippedX = brickValue["flipped_x"].GetBool();
                if( brickValue.HasMember("flipped_y") ) brickData.isFlippedY = brickValue["flipped_y"].GetBool();
                
                bossPattern.bricks.push_back(brickData);
            }
            
            bossPatterns[bossPattern.bossBrickId] = bossPattern;
            
            CCLOG("%s", bossPattern.toString().c_str());
        }
        
//        for( auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it ) {
//            const auto v = it->value.GetObject();
//
//            BossPatternData bossPattern;
//            bossPattern.bossBrickId = it->name.GetString();
//            bossPattern.friendBrick = getBrick(v["friend"].GetString());
//
//            if( v.HasMember("empty_positions") ) {
//                bossPattern.parseEmptyPositions(v["empty_positions"]);
//            }
//
//            bossPatterns[bossPattern.bossBrick.brickId] = bossPattern;
//
//            CCLOG("%s", bossPattern.toString().c_str());
//        }
    }
    CCLOG("========== PARSE END (boss_pattern.json)  ==========");
    
    // stage.json
    int stageIdx = 0;
    
    while( true ) {
        string fileName = STR_FORMAT("stage_%04d.json", stageIdx+1);
        string filePath = DIR_DATA + fileName;
        
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
        stage.originStage = stage.stage;
        
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
 * 스테이지 데이터를 반환합니다
 */
StageList Database::getStages() {
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
FloorList Database::getFloors() {
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

FloorList Database::getStageFloors(int stage) {
    return getStage(stage).floors;
}

bool Database::isStageLastFloor(int stage, int floor) {
    return getStage(stage).getLastFloor().floor == floor;
}

bool Database::isStageLastFloor(const FloorData &data) {
    return isStageLastFloor(data.stage, data.floor);
}

/**
 * 보스 패턴 데이터를 반환합니다
 */
BossPatternData Database::getBossPattern(const string &bossBrickId) {
    
    auto patterns = getInstance()->bossPatterns;
    auto it = patterns.find(bossBrickId);
    
    if( it == patterns.end() ) {
        CCASSERT(false, "Database::getBossPattern error.");
    }
    
    return it->second;
}

/**
 * 벽돌 데이터를 반환합니다
 */
BrickMap Database::getBricks() {
    return getInstance()->bricks;
}

BrickData Database::getBrick(const string &brickId) {
    
    auto bricks = getBricks();
    auto it = bricks.find(brickId);
    
    if( it == bricks.end() ) {
        CCASSERT(false, "Database::getBrick error");
    }
    
    return it->second;
}

// brick.json
void makeBrickJSON() {
    
    rapidjson::Document doc;
    doc.SetArray();
    
    auto addBricks = [](rapidjson::Document &doc,
                        int begin, int end, int type, int w, int h, vector<Color3B> colors) {
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        int idx = 0;
        
        for( int i = begin; i <= end; ++i, ++idx ) {
            string brickId = STR_FORMAT("brick_%05d", i);
            auto color = colors[idx];
            
            rapidjson::Value brickValue(rapidjson::kObjectType);
            brickValue.AddMember("id", SBJSON::value(brickId, allocator), allocator);
            brickValue.AddMember("type", type, allocator);
            brickValue.AddMember("width", w, allocator);
            brickValue.AddMember("height", h, allocator);
            brickValue.AddMember("color", SBJSON::value(STR_FORMAT("%d,%d,%d", color.r, color.g, color.b),
                                                        allocator), allocator);
            brickValue.AddMember("image", "", allocator);
            brickValue.AddMember("idle_anim_interval", 0.6f, allocator);
            brickValue.AddMember("damage_anim_interval", 0.1f, allocator);
            
            doc.PushBack(brickValue, allocator);
        }
    };
    
    // 일반
    {
        vector<Color3B> colors({
            Color3B(255,41,115),
            Color3B(255,231,66),
            Color3B(0,255,66),
            Color3B(0,168,255),
            Color3B(255,255,165),
            Color3B(255,41,115),
            Color3B(0,168,255),
            Color3B(0,255,66),
            Color3B(255,83,0),
            Color3B(255,48,71),
            Color3B(140,103,255),
            Color3B(255,255,222),
            Color3B(255,48,71),
            Color3B(186,92,255),
            Color3B(71,103,255),
            Color3B(255,195,39),
            Color3B(165,255,255),
            Color3B(156,255,0),
            Color3B(71,154,255),
            Color3B(255,231,66),
            Color3B(0,168,255),
            Color3B(255,124,0),
        });
        
        addBricks(doc, 1, 22, 1, 1, 1, colors);
    }
    
    // 특수
    {
        vector<Color3B> colors({
            Color3B(255,175,136),
            Color3B(0,255,255),
            Color3B(85,175,255),
            Color3B(0,168,255),
        });
        
        addBricks(doc, 1001, 1004, 10, 1, 1, colors);
    }
    
    // 보스 부하
    {
        vector<Color3B> colors({
            Color3B(0,157,92),
            Color3B(0,0,0),
            Color3B(255,96,110),
            Color3B(255,49,0),
            Color3B(232,165,126),
            Color3B(211,148,66),
            Color3B(255,156,66),
            Color3B(255,156,66),
            Color3B(255,85,74),
            Color3B(0,173,148),
            Color3B(255,156,66),
            Color3B(255,246,55),
            Color3B(255,156,66),
            Color3B(255,159,0),
            Color3B(255,156,66),
        });
        
        addBricks(doc, 5001, 5015, 50, 1, 1, colors);
    }
    
    // 보스
    {
        vector<Color3B> colors({
            Color3B(0,157,92),
            Color3B(255,41,115),
            Color3B(255,231,66),
            Color3B(0,168,255),
            Color3B(211,148,66),
            Color3B(193,71,193),
            Color3B(255,163,136),
            Color3B(193,92,255),
            Color3B(0,173,148),
            Color3B(219,222,255),
            Color3B(255,165,115),
            Color3B(255,159,0),
            Color3B(0,154,204),
            Color3B(82,163,255),
        });
        
        addBricks(doc, 10001, 10014, 100, 2, 2, colors);
    }
    
    // make json
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    string json = strbuf.GetString();
    CCLOG("make brick.json\n%s", json.c_str());
}

// stage_xxxx.json
void makeStageJSON() {
    
    const int FLOOR_LEN = 15;
    
    vector<StringList> BRICK_LIST({
        StringList({ "brick_00001","brick_00002","brick_00003",}),
        StringList({ "brick_00001","brick_00002","brick_00003","brick_00004"}),
        StringList({ "brick_00001","brick_00002","brick_00003","brick_00004"}),
        StringList({ "brick_00001","brick_00002","brick_00003","brick_00004","brick_00005"}),
        StringList({ "brick_00004","brick_00006","brick_00007",}),
        
        StringList({ "brick_00008","brick_00009","brick_00010",}),
        StringList({ "brick_00009","brick_00011","brick_00012","brick_00013","brick_00014","brick_00015"}),
        StringList({ "brick_00016","brick_00017","brick_00018",}),
        StringList({ "brick_00010","brick_00015","brick_00019",}),
        StringList({ "brick_00010","brick_00015","brick_00019","brick_00020",}),
        StringList({ "brick_00016","brick_00017","brick_00018","brick_00021","brick_00022"}),
    });
    
    StringList BOSS_LIST({
        "brick_10001",
        "brick_10002",
        "brick_10003",
        "brick_10004",
        "brick_10005",
        
        "brick_10006",
        "brick_10007",
        "brick_10008",
        "brick_10009",
        "brick_10010",
        "brick_10012",
    });
    
    for( int stage = 1; stage <= 11; ++stage ) {
        CCLOG("========== MAKE START (stage_%04d.json)  ==========", stage);
        rapidjson::Document doc;
        doc.SetObject();
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        
        // stage
        doc.AddMember("stage", stage, allocator);
        
        // floors
        // 1st: list[0]
        // 2nd: list[0,1]
        // 3rd
        // list.size() <= 4, list[0] ~ list[list.size()-2]
        // list.size() > 4,  list[2] ~ list[list.size()-2]
        // elite : list[list.size()-1]
        rapidjson::Value floors(rapidjson::kArrayType);
        int floor = ((stage-1) * FLOOR_LEN) + 1;
        auto stageBricks = BRICK_LIST[stage-1];
        
        for( int i = 1; i <= FLOOR_LEN; ++i, ++floor ) {
            rapidjson::Value floorValue(rapidjson::kObjectType);
            floorValue.AddMember("floor", floor, allocator);
            
            // floor1
            if( i == 1 ) {
                floorValue.AddMember("brick_hp", "floor+9", allocator);
                floorValue.AddMember("brick_drop_count", "1~3", allocator);
            
                rapidjson::Value brickList(rapidjson::kArrayType);
                brickList.PushBack(SBJSON::value(stageBricks[0], allocator), allocator);
                floorValue.AddMember("brick_list", brickList, allocator);
                
                floorValue.AddMember("elite_brick_drop_rate", 0, allocator);
                floorValue.AddMember("power_up_drop_rate", 100, allocator);
                floorValue.AddMember("friend_power_up_drop_rate", 0, allocator);
                floorValue.AddMember("money_drop_rate", 0, allocator);
            }
            // floor2~4
            else if( i >= 2 && i <= 4 ) {
            }
            // floor5
            else if( i == 5 ) {
                floorValue.AddMember("brick_drop_count", "2~3", allocator);
                
                rapidjson::Value brickList(rapidjson::kArrayType);
                brickList.PushBack(SBJSON::value(stageBricks[0], allocator), allocator);
                brickList.PushBack(SBJSON::value(stageBricks[1], allocator), allocator);
                floorValue.AddMember("brick_list", brickList, allocator);
                
                floorValue.AddMember("elite_brick_drop_rate", 5, allocator);
                
                rapidjson::Value eliteBrickList(rapidjson::kArrayType);
                eliteBrickList.PushBack(SBJSON::value(stageBricks[stageBricks.size()-1], allocator), allocator);
                floorValue.AddMember("elite_brick_list", eliteBrickList, allocator);
            }
            // floor6~9
            else if( i >= 6 && i <= 9 ) {
            }
            // floor10
            else if( i == 10 ) {
                floorValue.AddMember("brick_drop_count", "1~3", allocator);
                
                // list.size() <= 4, list[0] ~ list[list.size()-2]
                // list.size() > 4,  list[2] ~ list[list.size()-2]
                rapidjson::Value brickList(rapidjson::kArrayType);
                
                if( stageBricks.size() <= 4 ) {
                    for( int j = 0; j <= stageBricks.size()-2; ++j ) {
                        brickList.PushBack(SBJSON::value(stageBricks[j], allocator), allocator);
                    }
                } else {
                    for( int j = 2; j <= stageBricks.size()-2; ++j ) {
                        brickList.PushBack(SBJSON::value(stageBricks[j], allocator), allocator);
                    }
                }
                
                floorValue.AddMember("brick_list", brickList, allocator);
            }
            // floor11
            else if( i == 11 ) {
            }
            // floor12
            else if( i == 12 ) {
                floorValue.AddMember("brick_drop_count", "4", allocator);
            }
            // floor13
            else if( i == 13 ) {
                floorValue.AddMember("brick_hp", "prev_brick_hp+2", allocator);
            }
            // floor14
            else if( i == 14 ) {
                floorValue.AddMember("brick_drop_count", "1~2", allocator);
            }
            // floor15
            else if( i == 15 ) {
                floorValue.AddMember("boss_brick_id", SBJSON::value(BOSS_LIST[stage-1], allocator), allocator);
            }
            
            floors.PushBack(floorValue, allocator);
        }
        
        doc.AddMember("floors", floors, allocator);
        
        // make json
        rapidjson::StringBuffer strbuf;
        strbuf.Clear();
        
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        doc.Accept(writer);
        
        string json = strbuf.GetString();
        CCLOG("%s", json.c_str());
        CCLOG("========== MAKE END (stage_%04d.json)  ==========", stage);
    }
}
