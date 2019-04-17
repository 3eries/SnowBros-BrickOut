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
    NONE                = 0,
    NORMAL              = 1,
    SPECIAL             = 10,
    SPECIAL_SHIELD      = 11,
    SPECIAL_GHOST       = 12,
    SPECIAL_TELEPORT    = 13,
    SPECIAL_TORNADO     = 14,
    SPECIAL_NEUTRAL     = 20,
    BOSS_FRIENDS        = 50,
    BOSS                = 100,
};

enum class BrickImageType {
    NONE,
    IDLE,
    DAMAGE,
    HIDE,       // 고스트 브릭 전용
};

static const int BRICK_INFINITY_HP = -1;

struct BrickData {
    std::string              brickId;
    BrickType                type;
    int                      width;
    int                      height;
    cocos2d::Color3B         color;
    std::string              image;
    StringList               parts;
    float                    idleAnimInterval;
    float                    damageAnimInterval;
    float                    enterDuration;
    int                      zOrder;
    
    StringList               idleAnims;
    StringList               damageAnims;
    
    BrickData() {
        brickId = "";
        type = BrickType::NONE;
        width = 0;
        height = 0;
        idleAnimInterval = 0.6f;
        damageAnimInterval = 0.1f;
        enterDuration = 0;
        zOrder = 0;
    }
    
    BrickData(const rapidjson::Value &v) : BrickData() {
        // Required Properties
        brickId = v["id"].GetString();
        type    = (BrickType)v["type"].GetInt();
        width   = v["width"].GetInt();
        height  = v["height"].GetInt();
        
        // color
        {
            std::string str = v["color"].GetString();
            str = SBStringUtils::replaceAll(str, " ", "");
            
            auto arr = SBStringUtils::split(str, ',');
            CCASSERT(arr.size() == 3, "BrickData parse error: invalid color.");
            
            color = cocos2d::Color3B(TO_INTEGER(arr[0]), TO_INTEGER(arr[1]), TO_INTEGER(arr[2]));
        }
        
        // Optional Properties
        // image
        if( v.HasMember("image") ) {
            image = v["image"].GetString();
        }
        
        if( image == "" ) {
            image = brickId;
        }
        
        // parts
        if( v.HasMember("parts") ) {
            auto partList = v["parts"].GetArray();
            
            for( int i = 0; i < partList.Size(); ++i ) {
                parts.push_back(partList[i].GetString());
            }
        }
        
        // anim_interval
        if( v.HasMember("idle_anim_interval") )    idleAnimInterval = v["idle_anim_interval"].GetFloat();
        if( v.HasMember("damage_anim_interval") )  damageAnimInterval = v["damage_anim_interval"].GetFloat();
        
        // enter_duration
        if( v.HasMember("enter_duration") )        enterDuration = v["enter_duration"].GetFloat();
        
        // zorder
        if( v.HasMember("zorder") )                zOrder = v["zorder"].GetFloat();
    }
    
    bool isNull() const {
        return brickId == "";
    }
    
    bool is1x1() const {
        return width == 1 && height == 1;
    }
    
    bool isSpecial() const {
        return type >= BrickType::SPECIAL && type < BrickType::BOSS_FRIENDS;
    }
    
    bool isBoss() const {
        return type == BrickType::BOSS;
    }
    
    std::string toString() {
        std::string str = "\tBrickData {\n";
        str += STR_FORMAT("\t\tbrickId: %s, type: %d, width: %d, height: %d, color: %d,%d,%d \n",
                          brickId.c_str(), (int)type, width, height, color.r, color.g, color.b);
        str += STR_FORMAT("\t\timage: %s, idleAnimInterval: %f, damageAnimInterval: %f, enterDuration: %f\n",
                          image.c_str(), idleAnimInterval, damageAnimInterval, enterDuration);
        
        str += "\t\tparts: ";
        
        for( auto part : parts ) {
            str += part + ", ";
        }
        
        str += "\n";
        str += "\t\tidleAnims: ";
        
        for( std::string anim : idleAnims ) {
            str += anim + ", ";
        }
        
        str += "\n";
        str += "\t\tdamageAnims: ";
        
        for( std::string anim : damageAnims ) {
            str += anim + ", ";
        }
        
        str += "\n\t}";
        
        return str;
    }
    
    static int getRows(std::vector<BrickData> bricks) {
        int rows = 0;
        
        for( auto brick : bricks ) {
            rows += brick.width;
        }
        
        return rows;
    }
};

typedef std::map<std::string, BrickData> BrickDataMap;
typedef std::vector<BrickData>           BrickDataList;

#endif /* BrickData_h */
