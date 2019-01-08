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
#include "BossPatternData.h"

struct FloorData {
    int                      stage;                         // 스테이지
    int                      floor;                         // 층
    bool                     usePrevData;                   // 이전 데이터 사용 여부
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
    BrickData                bossBrick;                     // 보스 벽돌
    BossPatternData          bossPattern;                   // 보스 패턴
    
    FloorData() : stage(0), floor(0), usePrevData(true),
    brickHpOrigin(""), brickHp(0), brickDropMin(0), brickDropMax(0),
    eliteBrickDropRate(0),
    powerUpDropRate(0), friendPowerUpDropRate(0), moneyDropRate(0) {
    }
    
    FloorData(const FloorData &data) :
    stage(data.stage), floor(data.floor), usePrevData(data.usePrevData),
    brickHpOrigin(data.brickHpOrigin), brickHp(data.brickHp), brickDropMin(data.brickDropMin), brickDropMax(data.brickDropMax),
    eliteBrickDropRate(data.eliteBrickDropRate),
    powerUpDropRate(data.powerUpDropRate), friendPowerUpDropRate(data.friendPowerUpDropRate), moneyDropRate(data.moneyDropRate),
    brickList(data.brickList), eliteBrickList(data.eliteBrickList), bossBrick(data.bossBrick), bossPattern(data.bossPattern) {
    }
    
    void parse(const rapidjson::Value &v, rapidjson::Document::AllocatorType &allocator,
               const FloorData &prevFloor) {

        floor = v["floor"].GetInt();
        
        // brick_hp
        if( v.HasMember("brick_hp") ) {
            brickHpOrigin = v["brick_hp"].GetString();
            brickHpOrigin = SBStringUtils::replaceAll(brickHpOrigin, " ", "");
        }
        
        parseBrickHp(prevFloor);
        
        // brick_drop_count
        if( v.HasMember("brick_drop_count") ) {
            std::string dropCount = v["brick_drop_count"].GetString();
            dropCount = SBStringUtils::replaceAll(dropCount, " ", "");
            
            // 절대값
            if( SBStringUtils::isInteger(dropCount) ) {
                brickDropMin = TO_INTEGER(dropCount);
                brickDropMax = brickDropMin;
            }
            // 범위
            else if( dropCount.find("~") != std::string::npos ) {
                auto arr = SBStringUtils::split(dropCount, '~');
                CCASSERT(arr.size() == 2, "FloorData parse error: invalid brick_drop_count.");
                
                brickDropMin = TO_INTEGER(arr[0]);
                brickDropMax = TO_INTEGER(arr[1]);
            }
        }
        
        // int values
        {
            StringList keys({
                "elite_brick_drop_rate",
                "power_up_drop_rate",
                "friend_power_up_drop_rate",
                "money_drop_rate",
            });
            
            std::vector<int*> ptrs({
                &eliteBrickDropRate,
                &powerUpDropRate,
                &friendPowerUpDropRate,
                &moneyDropRate,
            });
            
            SBJSON::parse(v, allocator, keys, ptrs);
        }
    }
    
    void parseBrickHp(const FloorData &prevFloor) {
        
        // 절대값
        if( SBStringUtils::isInteger(brickHpOrigin) ) {
            brickHp = SBStringUtils::toNumber<int>(brickHpOrigin);
        }
        // 표현식
        else {
            auto parse = [=](std::string str, int i) -> bool {
              
                if( str == "floor" ) {
                    this->brickHp = this->floor + i;
                    return true;
                }
                
                if( str == "prev_brick_hp" ) {
                    this->brickHp = prevFloor.brickHp + i;
                    return true;
                }
                
                return false;
            };
            
            if( parse(brickHpOrigin,0) ) {
                return;
            }
            
            std::string op = "";
            
            if( brickHpOrigin.find("+") != std::string::npos )        op = "+";
            else if( brickHpOrigin.find("-") != std::string::npos )   op = "-";
            
            if( op != "" ) {
                auto arr = SBStringUtils::split(brickHpOrigin, op.c_str()[0]);
                
                if( arr.size() == 2 ) {
                    int i = SBStringUtils::toNumber<int>(arr[1]) * (op == "-" ? -1 : 1);
                    
                    if( parse(arr[0], i) ) {
                        return;
                    }
                }
            }
        }
    }
    
    bool isNull() const {
        return floor == 0;
    }
    
    bool isExistBoss() const {
        return bossBrick.brickId != "";
    }
    
    int getRandomDropCount() {
        if( brickDropMin == brickDropMax ) return brickDropMin;
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
    
    std::string toString() {
        std::string str = "\t FloorData {\n";
        str += STR_FORMAT("\t\t stage: %d, floor: %d\n", stage, floor);
        str += STR_FORMAT("\t\t usePrevData: %d\n", usePrevData);
        str += STR_FORMAT("\t\t brickHpOrigin: %s, brickHp: %d, brickDropMin: %d, brickDropMax: %d\n", brickHpOrigin.c_str(), brickHp, brickDropMin, brickDropMax);
        str += STR_FORMAT("\t\t eliteBrickDropRate: %d\n", eliteBrickDropRate);
        str += STR_FORMAT("\t\t powerUpDropRate: %d, friendPowerUpDropRate: %d, moneyDropRate: %d\n",
                          powerUpDropRate, friendPowerUpDropRate, moneyDropRate);
        
        
        str += "\t\t brickList: ";
        
        for( auto brick : brickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += "\t\t eliteBrickList: ";
        
        for( auto brick : eliteBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += STR_FORMAT("\t\t bossBrickId: %s", bossBrick.brickId.c_str());
        str += "\n\t}";
        
        return str;
    }
};

typedef std::vector<FloorData>           FloorList;

#endif /* FloorData_h */
