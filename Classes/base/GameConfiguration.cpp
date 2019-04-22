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
    firstCoin            = doc["first_coin"].GetInt();
    firstBallSkin        = doc["first_ball_skin"].GetString();
    firstFriend          = doc["first_friend"].GetString();
    restoreCost          = doc["restore_cost"].GetInt();
    tileRows             = doc["tile_rows"].GetInt();
    tileColumns          = doc["tile_columns"].GetInt();
    tileEnterDuration    = doc["tile_enter_duration"].GetFloat();
    firstBallCount       = doc["first_ball_count"].GetInt();
    maxBallCount         = doc["max_ball_count"].GetInt();
    maxScore             = doc["max_score"].GetInt();
}

