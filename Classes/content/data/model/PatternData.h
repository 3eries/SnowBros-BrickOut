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

#include "GameConfiguration.hpp"
#include "BrickData.h"
#include "TileData.h"

/**
 * PatternBrickData
 * 패턴 내에 정의된 브릭 데이터
 */
struct PatternBrickData {
    BrickData brick;
    int hp;
    TileData tile;
    
    PatternBrickData() : hp(0) {}
    
    std::string toString() {
        std::string str = "\t\t\tPatternBrickData {\n";
        str += STR_FORMAT("\t\t\t\tbrick: %s, hp: %d\n",
                          brick.brickId.c_str(), hp);
        str += STR_FORMAT("\t\t\t\t%s", tile.toString().c_str());
        str += "\n\t\t\t}";
        
        return str;
    }
};

typedef std::vector<PatternBrickData> PatternBrickDataList;

/**
 * PatternData
 */
struct PatternData {
    std::string patternId;
    PatternBrickDataList bricks;
    
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
    
    PatternBrickData getBrick(int x) {
        for( auto brick : bricks ) {
            if( brick.tile.pos.x == x ) {
                return brick;
            }
        }
        
        return PatternBrickData();
    }
    
    BrickDataList getBossBrickList() const {
        BrickDataList brickList;
        
        for( auto brick : bricks ) {
            if( brick.brick.isBoss() ) {
                brickList.push_back(brick.brick);
            }
        }
        
        return brickList;
    }
    
    bool isExistBoss() const {
        return getBossBrickList().size() > 0;
    }
    
    /**
     * ex) [01101][00000][01102][00000][00000][00000]
     */
    std::string getBrickListString() {
        
        std::string str = "";
        
        for( int x = 0; x < GAME_CONFIG->getTileRows(); ++x ) {
            auto brick = getBrick(x).brick;
            
            if( brick.isNull() ) {
                str += "[00000]";
            } else {
                str += STR_FORMAT("[%s]", brick.brickId.c_str());
            }
        }
        
        str = SBStringUtils::replaceAll(str, "brick_", "");
        return str;
    };
    
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
typedef std::vector<PatternData> PatternDataList;

#endif /* PatternData_h */
