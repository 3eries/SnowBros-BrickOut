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

struct StageData {
    int                      level;                         // 레벨
    int                      stage;                         // 스테이지
    std::string              brickHpOrigin;                 // 벽돌 HP 원본 json
    int                      brickHp;                       // 벽돌 HP
    int                      brickDropMin;                  // 벽돌 최소 드랍 수
    int                      brickDropMax;                  // 벽돌 최대 드랍 수
    int                      eliteBrickDropRate;            // 엘리트 벽돌 드랍률
    int                      powerUpDropRate;               // 볼 증가 아이템 드랍률
    int                      friendPowerUpDropRate;         // 프렌즈 볼 증가 아이템 드랍률
    int                      moneyDropRate;                 // 재화 드랍률
    
    StringList               brickList;                     // 일반 벽돌 리스트
    StringList               eliteBrickList;                // 엘리트 벽돌 리스트
    StringList               bossBrickList;                 // 보스 벽돌 리스트
    
    StageData() : level(0), stage(0) {}
    
    StageData(const StageData &data) :
    level(data.level), stage(data.stage),
    brickHpOrigin(data.brickHpOrigin), brickHp(data.brickHp), brickDropMin(data.brickDropMin), brickDropMax(data.brickDropMax),
    eliteBrickDropRate(data.eliteBrickDropRate),
    powerUpDropRate(data.powerUpDropRate), friendPowerUpDropRate(data.friendPowerUpDropRate), moneyDropRate(data.moneyDropRate),
    brickList(data.brickList), eliteBrickList(data.eliteBrickList), bossBrickList(data.bossBrickList) {
    }
    
    std::string toString() {
        std::string str = "\tStageData {\n";
        str += STR_FORMAT("\t\tlevel: %d, stage: %d\n", level, stage);
        str += STR_FORMAT("\t\tbrickHpOrigin: %s, brickHp: %d, brickDropMin: %d, brickDropMax: %d\n", brickHpOrigin.c_str(), brickHp, brickDropMin, brickDropMax);
        str += STR_FORMAT("\t\teliteBrickDropRate: %d\n", eliteBrickDropRate);
        str += STR_FORMAT("\t\tpowerUpDropRate: %d, friendPowerUpDropRate: %d, moneyDropRate: %d\n",
                          powerUpDropRate, friendPowerUpDropRate, moneyDropRate);
        
        
        str += "\t\tbrickList: ";
        
        for( std::string brick : brickList ) {
            str += brick + ", ";
        }
        
        str += "\n";
        str += "\t\teliteBrickList: ";
        
        for( std::string brick : eliteBrickList ) {
            str += brick + ", ";
        }
        
        str += "\n";
        str += "\t\tbossBrickList: ";
        
        for( std::string brick : bossBrickList ) {
            str += brick + ", ";
        }
        
        str += "\n\t}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageList;

#endif /* StageData_h */
