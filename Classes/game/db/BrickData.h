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
    cocos2d::Color3B         color;
    
    StringList               idleAnims;
    StringList               damageAnims;
    
    BrickData() {
        brickId = "";
        type = BrickType::NONE;
    }
    
    BrickData(const rapidjson::Value &v) {
        brickId = v["id"].GetString();
        type    = (BrickType)v["type"].GetInt();
        width   = v["width"].GetInt();
        height  = v["height"].GetInt();
        
        // image
        if( v.HasMember("image") ) {
            image = v["image"].GetString();
        }
        
        if( image == "" ) {
            image = brickId;
        }
        
        // color
        {
            std::string str = v["color"].GetString();
            str = SBStringUtils::replaceAll(str, " ", "");
            
            auto arr = SBStringUtils::split(str, ',');
            CCASSERT(arr.size() == 3, "BrickData parse error: invalid color.");
            
            color = cocos2d::Color3B(TO_INTEGER(arr[0]), TO_INTEGER(arr[1]), TO_INTEGER(arr[2]));
        }
    }
    
    bool isElite() const {
        return type == BrickType::ELITE;
    }
    
    bool isBoss() const {
        return type == BrickType::BOSS;
    }
    
    std::string toString() {
        std::string str = "\t BrickData {\n";
        str += STR_FORMAT("\t\t brickId: %s, type: %d width: %d height: %d \n",
                          brickId.c_str(), (int)type, width, height);
        str += STR_FORMAT("\t\t image: %s color: %d,%d,%d \n", image.c_str(), color.r, color.g, color.b);
        str += "\t\t idleAnims: ";
        
        for( std::string anim : idleAnims ) {
            str += anim + ", ";
        }
        
        str += "\n";
        str += "\t\t damageAnims: ";
        
        for( std::string anim : damageAnims ) {
            str += anim + ", ";
        }
        
        str += "\n\t }";
        
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
