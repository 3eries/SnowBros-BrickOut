//
//  StageData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef StageData_h
#define StageData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "FloorData.h"
#include "PatternData.h"

struct StageData {
    int            world;                   // 월드 번호
    int            stage;                   // 스테이지 번호
    int            originStage;
    int            finalBallCount;
    int            finalFriendsBallDamage;
    int            coinDropMin;             // 코인 최소 드랍 수
    int            coinDropMax;             // 코인 최대 드랍 수
    FloorDataList  floors;
    PatternDataMap patterns;
    
    // 스테이지에 등장한 브릭 리스트
    BrickDataList  normalBrickList;
    BrickDataList  bossBrickList;
    
    StageData() : world(0), stage(0) {}
    
    void parse(const rapidjson::Value &v, rapidjson::Document::AllocatorType &allocator) {
        
        // int values
        {
            StringList keys({
                "world",
                "stage",
                "final_ball_count",
                "final_friends_ball_damage",
            });
            
            std::vector<int*> ptrs({
                &world,
                &stage,
                &finalBallCount,
                &finalFriendsBallDamage,
            });
            
            SBJSON::parse(v, allocator, keys, ptrs);
        }
        
        originStage = stage;
        
        // coin_drop_count
        std::string dropCount = v["coin_drop_count"].GetString();
        dropCount = SBStringUtils::replaceAll(dropCount, " ", "");
        
        // 절대값
        if( SBStringUtils::isInteger(dropCount) ) {
            coinDropMin = TO_INTEGER(dropCount);
            coinDropMax = coinDropMin;
        }
        // 범위
        else if( dropCount.find("~") != std::string::npos ) {
            auto arr = SBStringUtils::split(dropCount, '~');
            CCASSERT(arr.size() == 2, "StageData parse error: invalid coin_drop_count.");
            
            coinDropMin = TO_INTEGER(arr[0]);
            coinDropMax = TO_INTEGER(arr[1]);
        }
    }
    
    bool isNull() const {
        return stage == 0;
    }

    int getRandomCoinDropCount() {
        if( coinDropMin == coinDropMax ) return coinDropMin;
        
        int len = (coinDropMax - coinDropMin) + 1;
        return (arc4random() % len) + coinDropMin;
    }
    
    FloorData getFirstFloor() const {
        return floors[0];
    }

    FloorData getLastFloor() const {
        return floors[floors.size()-1];
    }

    std::string toString() {
        std::string str = "StageData {\n";
        str += STR_FORMAT("\tworld: %d, stage: %d, originStage: %d\n", world, stage, originStage);
        str += STR_FORMAT("\tfinalBallCount: %d, finalFriendsBallDamage: %d\n", finalBallCount, finalFriendsBallDamage);
        str += STR_FORMAT("\tcoinDropMin: %d, coinDropMax: %d\n", coinDropMin, coinDropMax);
        str += STR_FORMAT("\tfirstFloor: %d\n", getFirstFloor().floor);
        str += STR_FORMAT("\tlastFloor: %d\n", getLastFloor().floor);
        str += STR_FORMAT("\tfloorLen: %d\n", (int)floors.size());
        
        str += "\tfloors:\n";
        
        for( auto floor : floors ) {
            str += floor.toString() + "\n";
        }
        
        str += "\tpatterns:\n";
        
        for( auto e : patterns ) {
            str += e.second.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageDataList;

#endif /* StageData_h */
