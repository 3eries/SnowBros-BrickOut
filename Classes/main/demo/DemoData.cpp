//
//  DemoData.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#include "DemoData.hpp"

#include "Define.h"
#include "ContentManager.hpp"

USING_NS_CC;
using namespace std;

#define DEMO_FILE                   (DIR_DATA + "demo.json")

static DemoData *instance = nullptr;

DemoData* DemoData::getInstance() {
    
    if( !instance ) {
        instance = new DemoData();
    }
    
    return instance;
}

void DemoData::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

DemoData::DemoData() {
}

DemoData::~DemoData() {
}

void DemoData::init() {
    
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
        
        stages.push_back(stage);
    }
    
//    return vector<DemoBrickData>({
//        DemoBrickData(stage.normalBrickList[0], TilePosition(1,8)),
//        DemoBrickData(stage.bossBrickList[0],   TilePosition(2,8)),
//        DemoBrickData(stage.normalBrickList[1], TilePosition(4,8)),
//    });
    
    CCLOG("========== PARSE END (demo.json)  ==========");
}

vector<DemoStageData> DemoData::getStages() {
    
    return stages;
}

DemoStageData DemoData::getStage(int stage) {
    
    for( auto stageData : stages ) {
        if( stageData.stage.stage == stage ) {
            return stageData;
        }
    }
    
    return DemoStageData();
}
