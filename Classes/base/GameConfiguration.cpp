//
//  GameConfiguration.cpp
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#include "GameConfiguration.hpp"

#include "json/document-wrapper.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

USING_NS_CC;
using namespace std;

#define JSON_FILE       "config/game_config.json"

static GameConfiguration *instance = nullptr;
GameConfiguration* GameConfiguration::getInstance() {
    
    if( !instance ) {
        instance = new GameConfiguration();
    }
    
    return instance;
}

void GameConfiguration::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

GameConfiguration::GameConfiguration() {
}

GameConfiguration::~GameConfiguration() {
}

void GameConfiguration::init() {

//    string jsonStr = SBStringUtils::readTextFile(JSON_FILE);
//    CCLOG("jsonStr: %s", jsonStr.c_str());
}

void GameConfiguration::parse(const string &json) {
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto platformObj = doc["ios"].GetObject();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto platformObj = doc["android"].GetObject();
#endif
    
    // store
    storeUrl = platformObj["store"].GetString();
    
    // game value
    tileRows             = doc["tile_rows"].GetInt();
    tileColumns          = doc["tile_columns"].GetInt();
    firstBallCount       = doc["first_ball_count"].GetInt();
    maxBallCount         = doc["max_ball_count"].GetInt();
    maxScore             = doc["max_score"].GetInt();
    
    // levels
    levelInfos.clear();
    
    {
        auto levelList = doc["levels"].GetArray();
        
        for( int i = 0; i < levelList.Size(); ++i ) {
            const rapidjson::Value &levelValue = levelList[i];
            
            LevelInfo info;
            info.level = levelValue["level"].GetInt();
            info.score = levelValue["score"].GetInt();
            
            levelInfos.push_back(info);
        }
        
        // order by level asc
        sort(levelInfos.begin(), levelInfos.end(), [](const LevelInfo &l1, const LevelInfo &l2) {
            return l1.level < l2.level;
        });
    }
}

LevelInfo GameConfiguration::getLevel(int level) {
    
    if( level > getMaxLevel().level ) {
        return LevelInfo();
    }
    
    return levelInfos[level-1];
}

LevelInfo GameConfiguration::getLevelByScore(int score) {
    
    LevelInfo levelInfo;
    
    for( int i = levelInfos.size()-1; i >= 0; --i ) {
        auto info = levelInfos[i];
        
        if( score >= info.score ) {
            levelInfo = info;
            break;
        }
    }
    
    return levelInfo;
}

LevelInfo GameConfiguration::getMaxLevel() {
    
    if( levelInfos.size() == 0 ) {
        return LevelInfo();
    }
    
    return levelInfos[levelInfos.size()-1];
}


