//
//  BossPatternData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 09/01/2019.
//

#ifndef BossPatternData_h
#define BossPatternData_h

#include "cocos2d.h"
#include "superbomb.h"
#include "BrickData.h"

struct BossPatternData {
    BrickData bossBrick;   // 보스 벽돌
    BrickData friendBrick; // 부하 벽돌
    std::vector<cocos2d::Vec2> emptyPositions; // 비어 있는 부하 좌표
    
    BossPatternData() {
    }
    
    void parseEmptyPositions(const rapidjson::Value &v) {
        
        auto list = v.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            auto pos = list[i].GetArray();
            emptyPositions.push_back(cocos2d::Vec2(pos[0].GetInt(), pos[1].GetInt()));
        }
    }
    
    bool isNull() const {
        return bossBrick.brickId == "";
    }
    
    bool isEmptyPosition(const cocos2d::Vec2 &p) const {
        
        for( auto emptyPos : emptyPositions ) {
            if( p.equals(emptyPos) ) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string toString() {
        std::string str = "\t BossPatternData {\n";
        str += STR_FORMAT("\t\t bossBrick: %s, friendBrick: %s \n",
                          bossBrick.brickId.c_str(), friendBrick.brickId.c_str());
        
        str += "\t\t emptyPositions: ";
        
        for( auto pos : emptyPositions ) {
            str += STR_FORMAT("%d,%d", (int)pos.x, (int)pos.y) + ", ";
        }
        
        str += "\n\t }";
        
        return str;
    }
};

typedef std::map<std::string, BossPatternData> BossPatternMap;

#endif /* BossPatternData_h */
