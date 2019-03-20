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
#include "PatternData.h"

struct FloorData {
    int                      stage;                         // 스테이지
    int                      floor;                         // 층
    bool                     usePrevData;                   // 이전 데이터 사용 여부
    std::string              brickHpOrigin;                 // 벽돌 HP 원본 json
    int                      brickHp;                       // 벽돌 HP
    int                      brickDropMin;                  // 벽돌 최소 드랍 수
    int                      brickDropMax;                  // 벽돌 최대 드랍 수
    int                      eliteBrickDropRate;            // 엘리트 벽돌 드랍률
    
    BrickDataList            normalBrickList;               // 일반 브릭 리스트
    BrickDataList            specialBrickList;              // 특수 브릭 리스트
    BrickDataList            neutralBrickList;              // 중립 브릭 리스트
    PatternData              pattern;                       // 패턴
    
    FloorData() : stage(0), floor(0), usePrevData(true),
    brickHpOrigin(""), brickHp(0), brickDropMin(0), brickDropMax(0),
    eliteBrickDropRate(0) {
    }
    
    FloorData(const FloorData &data) :
    stage(data.stage), floor(data.floor), usePrevData(data.usePrevData),
    brickHpOrigin(data.brickHpOrigin), brickHp(data.brickHp), brickDropMin(data.brickDropMin), brickDropMax(data.brickDropMax),
    eliteBrickDropRate(data.eliteBrickDropRate),
    normalBrickList(data.normalBrickList), specialBrickList(data.specialBrickList), neutralBrickList(data.neutralBrickList),
    pattern(data.pattern) {
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
            });
            
            std::vector<int*> ptrs({
                &eliteBrickDropRate,
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
    
    BrickDataList getBossBrickList() const {
        return pattern.getBossBrickList();
    }
    
    bool isExistBoss() const {
        return pattern.isExistBoss();
    }
    
    int getRandomBrickDropCount() const {
        if( brickDropMin == brickDropMax ) return brickDropMin;
        return cocos2d::random<int>(brickDropMin, brickDropMax);
    }
    
    BrickData getRandomNormalBrick() const {
        return normalBrickList[arc4random() % normalBrickList.size()];
    }
    
    std::string toString() {
        std::string str = "\tFloorData {\n";
        str += STR_FORMAT("\t\tstage: %d, floor: %d\n", stage, floor);
        str += STR_FORMAT("\t\tusePrevData: %d\n", usePrevData);
        str += STR_FORMAT("\t\tbrickHpOrigin: %s, brickHp: %d, brickDropMin: %d, brickDropMax: %d\n", brickHpOrigin.c_str(), brickHp, brickDropMin, brickDropMax);
        str += STR_FORMAT("\t\teliteBrickDropRate: %d\n", eliteBrickDropRate);
        
        str += "\t\tnormalBrickList: ";
        
        for( auto brick : normalBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += "\t\tspecialBrickList: ";
        
        for( auto brick : specialBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += "\t\tneutralBrickList: ";
        
        for( auto brick : neutralBrickList ) {
            str += brick.brickId + ", ";
        }
        
        str += "\n";
        str += STR_FORMAT("\t\tpattern:\n%s", pattern.toString().c_str());
        str += "\n\t}";
        
        return str;
    }
};

typedef std::vector<FloorData>           FloorDataList;

#endif /* FloorData_h */
