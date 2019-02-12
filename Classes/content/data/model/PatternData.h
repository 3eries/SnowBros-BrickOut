//
//  PatternData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 12/02/2019.
//

#ifndef PatternData_h
#define PatternData_h

#include "cocos2d.h"
#include "superbomb.h"

#include "BrickData.h"

/**
 * PatternBrickData
 * 패턴 내에 정의된 브릭 데이터
 */
struct PatternBrickData {
    BrickData brick;
    cocos2d::Vec2 pos;
    int hp;
    bool isFlippedX;
    bool isFlippedY;
    
    PatternBrickData() : isFlippedX(false), isFlippedY(false) {}
    
    std::string toString() {
        std::string str = "\t\t\tPatternBrickData {\n";
        str += STR_FORMAT("\t\t\t\tbrick: %s, pos: (%d,%d), hp: %d, isFlippedX: %d, isFlippedY: %d",
                          brick.brickId.c_str(), (int)pos.x, (int)pos.y, hp, isFlippedX, isFlippedY);
        str += "\n\t\t\t}";
        
        return str;
    }
};

/**
 * PatternData
 */
struct PatternData {
    std::string patternId;
    std::vector<PatternBrickData> bricks;
    
    PatternData() {
    }
    
    bool isNull() const {
        return patternId == "";
    }
    
    bool isExistBrick(const std::string &brickId) const {
        for( auto brick : bricks ) {
            if( brick.brick.brickId == brickId ) {
                return true;
            }
        }
        
        return false;
    }
    
    bool isExistBoss() const {
        for( auto brick : bricks ) {
            if( brick.brick.isBoss() ) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string toString() {
        std::string str = "\t\tPatternData {\n";
        str += STR_FORMAT("\t\t\tpatternId: %s\n", patternId.c_str());
        
        for( auto brick : bricks ) {
            str += brick.toString() + "\n";
        }
        
        str += "\t\t}";
        
        return str;
    }
};

typedef std::map<std::string, PatternData> PatternDataMap;

#endif /* PatternData_h */
