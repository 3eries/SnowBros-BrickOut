//
//  FloorData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef FloorData_h
#define FloorData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "BrickData.h"

struct FloorData {
    int                      stage;                         // 스테이지
    int                      floor;                         // 층
    bool                     isLastFloor;                   // 마지막 층 여부
    std::string              brickHpOrigin;                 // 벽돌 HP 원본 json
    int                      brickHp;                       // 벽돌 HP
    int                      brickDropMin;                  // 벽돌 최소 드랍 수
    int                      brickDropMax;                  // 벽돌 최대 드랍 수
    int                      eliteBrickDropRate;            // 엘리트 벽돌 드랍률
    int                      powerUpDropRate;               // 볼 증가 아이템 드랍률
    int                      friendPowerUpDropRate;         // 프렌즈 볼 증가 아이템 드랍률
    int                      moneyDropRate;                 // 재화 드랍률
    
    BrickList                brickList;                     // 일반 벽돌 리스트
    BrickList                eliteBrickList;                // 엘리트 벽돌 리스트
    BrickList                bossBrickList;                 // 보스 벽돌 리스트
    
    FloorData() : stage(0), floor(0), isLastFloor(false),
    brickHpOrigin(""), brickHp(0), brickDropMin(0), brickDropMax(0),
    eliteBrickDropRate(0),
    powerUpDropRate(0), friendPowerUpDropRate(0), moneyDropRate(0) {
    }
    
    FloorData(const FloorData &data) :
    stage(data.stage), floor(data.floor), isLastFloor(data.isLastFloor),
    brickHpOrigin(data.brickHpOrigin), brickHp(data.brickHp), brickDropMin(data.brickDropMin), brickDropMax(data.brickDropMax),
    eliteBrickDropRate(data.eliteBrickDropRate),
    powerUpDropRate(data.powerUpDropRate), friendPowerUpDropRate(data.friendPowerUpDropRate), moneyDropRate(data.moneyDropRate),
    brickList(data.brickList), eliteBrickList(data.eliteBrickList), bossBrickList(data.bossBrickList) {
    }
    
    bool isNull() const {
        return floor == 0;
    }
    
    int getRandomDropCount() {
        return cocos2d::random<int>(brickDropMin, brickDropMax);
    }
    
    BrickData getRandomBrick() {
        int i = cocos2d::random<int>(0, (int)brickList.size()-1);
        return brickList[i];
    }
    
    BrickData getRandomEliteBrick() {
        int i = cocos2d::random<int>(0, (int)eliteBrickList.size()-1);
        return eliteBrickList[i];
    }
    
    BrickData getRandomBossBrick() {
        int i = cocos2d::random<int>(0, (int)bossBrickList.size()-1);
        return bossBrickList[i];
    }
    
    std::string toString() {
        std::string str = "\tFloorData {\n";
        str += STR_FORMAT("\t\tstage: %d, floor: %d isLastFloor: %d\n", stage, floor, isLastFloor);
        str += STR_FORMAT("\t\tbrickHpOrigin: %s, brickHp: %d, brickDropMin: %d, brickDropMax: %d\n", brickHpOrigin.c_str(), brickHp, brickDropMin, brickDropMax);
        str += STR_FORMAT("\t\teliteBrickDropRate: %d\n", eliteBrickDropRate);
        str += STR_FORMAT("\t\tpowerUpDropRate: %d, friendPowerUpDropRate: %d, moneyDropRate: %d\n",
                          powerUpDropRate, friendPowerUpDropRate, moneyDropRate);
        
        
        str += "\t\tbrickList: ";
        
        for( auto brick : brickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += "\t\teliteBrickList: ";
        
        for( auto brick : eliteBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += "\t\tbossBrickList: ";
        
        for( auto brick : bossBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n\t}";
        
        return str;
    }
};

typedef std::vector<FloorData>           FloorList;

#endif /* FloorData_h */
