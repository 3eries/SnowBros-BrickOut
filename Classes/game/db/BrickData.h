//
//  BrickData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef BrickData_h
#define BrickData_h

#include "cocos2d.h"
#include "superbomb.h"

enum class BrickType {
    NONE            = 0,
    NORMAL,
    ELITE           = 5,
    BOSS_FRIENDS    = 9,
    BOSS,
};

struct BrickData {
    std::string              brickId;
    BrickType                type;
    int                      width;
    int                      height;
    std::string              image;
    StringList               idleAnims;
    StringList               damageAnims;
    
    BrickData() {
        brickId = "";
        type = BrickType::NONE;
    }
    
    bool isElite() const {
        return type == BrickType::ELITE;
    }
    
    bool isBoss() const {
        return type == BrickType::BOSS;
    }
    
    std::string toString() {
        std::string str = "\tBrickData {\n";
        str += STR_FORMAT("\t\tbrickId: %s, type: %d width: %d height: %d",
                          brickId.c_str(), (int)type, width, height);
        str += "\n\t\tidleAnims: ";
        
        for( std::string anim : idleAnims ) {
            str += anim + ", ";
        }
        
        str += "\n\t\tdamageAnims: ";
        
        for( std::string anim : damageAnims ) {
            str += anim + ", ";
        }
        
        str += "\n\t}";
        
        return str;
    }
};

typedef std::map<std::string, BrickData> BrickMap;
typedef std::vector<BrickData>           BrickList;

static inline int getTotalTileRows(BrickList bricks) {
    
    int total = 0;
    
    for( auto brick : bricks ) {
        total += brick.width;
    }
    
    return total;
}

#endif /* BrickData_h */
