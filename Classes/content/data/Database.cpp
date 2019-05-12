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

#define BALL_SKIN_FILE          (DIR_CONTENT_DATA + "ball_skin.json")
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
    string json = SBStringUtils::readTextFile(BALL_SKIN_FILE);
    
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
 * world_xx_stage_xx.json
 */
void Database::parseStageJson() {
    
    int worldId = 0;
    int stageId = 0;
    
    while( true ) {
        worldId++;
        WorldData world(worldId);
        
        while( true ) {
            stageId++;
            
            string fileName = STR_FORMAT("world_%02d_stage_%02d.json", worldId, stageId);
            string filePath = DIR_CONTENT_DATA + fileName;
            
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
            if( doc.HasMember("patterns") ) {
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
            }
            
            // floors
            auto floorValueList = doc["floors"].GetArray();
            FloorData prevFloor;
            
            for( int i = 0; i < floorValueList.Size(); ++i ) {
                const rapidjson::Value &floorValue = floorValueList[i];
                bool usePrevData = floorValue.HasMember("use_prev_data") ? floorValue["use_prev_data"].GetBool() : true;
                
                FloorData floor = usePrevData ? prevFloor : FloorData();
                floor.world       = stage.world;
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
                if( floorValue.HasMember("neutral_brick_list") )  floor.neutralBrickList = getBrickList("neutral_brick_list");
                if( floorValue.HasMember("special_brick_list") )  floor.specialBrickList = getBrickList("special_brick_list");
                
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
            
            auto &lastFloor = stage.floors[stage.floors.size()-1];
            lastFloor.isStageLastFloor = true;
            
            world.addStage(stage);
            stages.push_back(stage);
            
            CCLOG("========== PARSE END (%s)  ==========", fileName.c_str());
        } // stage loop
        
        // 월드의 스테이지가 없으면 loop 종료
        if( world.stages.size() == 0 ) {
            break;
        }
        // 유효한 월드 추가
        else {
            // order by stage asc
            sort(world.stages.begin(), world.stages.end(), [](const StageData &s1, const StageData &s2) {
                return s1.stage < s2.stage;
            });
            
            auto &lastStage = world.stages[world.stages.size()-1];
            lastStage.isWorldLastStage = true;
            
            CCLOG("%s", world.toString().c_str());
            worlds.push_back(world);
        }
    } // world loop
    
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
        stage.stage = Database::getWorld(v["world"].GetInt()).getFirstStage();
        
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
 * 임시 월드 추가
 */
void Database::addTempWorld() {
    
    auto world = getLastWorld();
    world.world++;
    
    for( auto &stage : world.stages ) {
        stage.world = world.world;
        
        FloorData prevFloor;
        
        for( auto &floor : stage.floors ) {
            floor.world = world.world;
            floor.parseBrickHp(prevFloor);
            floor.brickHp *= 1.02f;
            
            auto &floors = getInstance()->floors;
            floors.push_back(floor);
            
            prevFloor = floor;
        }
    }
    
    Log::i("addTempWorld:\n%s", world.toString().c_str());
    
    auto &worlds = getInstance()->worlds;
    worlds.push_back(world);
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
 * 월드 데이터를 반환합니다
 */
WorldDataList Database::getWorlds() {
    return getInstance()->worlds;
}

WorldData Database::getWorld(int world) {
    
    auto worlds = getWorlds();
    
    for( auto data : worlds ) {
        if( data.world == world ) {
            return data;
        }
    }
    
    CCASSERT(false, "Database::getWorld error: invalid world.");
    return WorldData();
}

WorldData Database::getFirstWorld() {
    
    auto worlds = getWorlds();
    
    if( worlds.size() == 0 ) {
        return WorldData();
    }
    
    return worlds[0];
}

WorldData Database::getLastWorld() {
    
    auto worlds = getWorlds();
    
    if( worlds.size() == 0 ) {
        return WorldData();
    }
    
    return worlds[worlds.size()-1];
}

bool Database::isLastWorld(int world) {
    return getLastWorld().world == world;
}

StageData Database::getWorldLastStage(int world) {

    return getWorld(world).getLastStage();
}

/**
 * 해당 스테이지의 최초 볼 개수를 반환합니다
 */
int Database::getStageFirstBallCount(int world, int stage) {
    
    // 최초 스테이지
    if( world == 1 && stage == 1 ) {
        return GAME_CONFIG->getFirstBallCount();
    }
    
    // 1 스테이지인 경우, 이전 월드 마지막 스테이지의 최종 개수
    if( stage == 1 ) {
        return getWorld(world-1).getLastStage().finalBallCount;
    }
    
    // 이전 스테이지의 최종 개수
    return getWorld(world).getStage(stage-1).finalBallCount;
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
