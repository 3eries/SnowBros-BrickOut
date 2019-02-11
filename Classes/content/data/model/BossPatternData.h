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

struct BossPatternBrickData {
    BrickData brick;
    cocos2d::Vec2 pos;
    int hp;
    bool isFlippedX;
    bool isFlippedY;
    
    BossPatternBrickData() : isFlippedX(false), isFlippedY(false) {}
    
    std::string toString() {
        std::string str = "\t\tBossPatternBrickData {\n";
        str += STR_FORMAT("\t\t\tbrick: %s, pos: (%d,%d), hp: %d, isFlippedX: %d, isFlippedY: %d \n",
                          brick.brickId.c_str(), (int)pos.x, (int)pos.y, hp, isFlippedX, isFlippedY);
        str += "\n\t\t}";
        
        return str;
    }
};

struct BossPatternData {
    std::string bossBrickId;
    std::vector<BossPatternBrickData> bricks;
    
    BossPatternData() {
    }
    
//    void parseEmptyPositions(const rapidjson::Value &v) {
//
//        auto list = v.GetArray();
//
//        for( int i = 0; i < list.Size(); ++i ) {
//            auto pos = list[i].GetArray();
//            emptyPositions.push_back(cocos2d::Vec2(pos[0].GetInt(), pos[1].GetInt()));
//        }
//    }
    
    bool isNull() const {
        return bossBrickId == "";
    }

    std::string toString() {
        std::string str = "\tBossPatternData {\n";
        str += STR_FORMAT("\t\tbossBrick: %s\n", bossBrickId.c_str());
        
        for( auto brick : bricks ) {
            str += brick.toString() + "\n";
        }
        
        str += "\t}";
        
        return str;
    }
};

typedef std::map<std::string, BossPatternData> BossPatternMap;

#endif /* BossPatternData_h */
