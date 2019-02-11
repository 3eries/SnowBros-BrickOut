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
    NORMAL          = 1,
    SPECIAL         = 10,
    BOSS_FRIENDS    = 50,
    BOSS            = 100,
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
    
    StringList               idleAnims;
    StringList               damageAnims;
    
    BrickData() {
        brickId = "";
        type = BrickType::NONE;
        idleAnimInterval = 0.6f;
        damageAnimInterval = 0.1f;
    }
    
    BrickData(const rapidjson::Value &v) {
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
    }
    
    bool isSpecial() const {
        return type == BrickType::SPECIAL;
    }
    
    bool isBoss() const {
        return type == BrickType::BOSS;
    }
    
    std::string toString() {
        std::string str = "\tBrickData {\n";
        str += STR_FORMAT("\t\tbrickId: %s, type: %d, width: %d, height: %d, color: %d,%d,%d \n",
                          brickId.c_str(), (int)type, width, height, color.r, color.g, color.b);
        str += STR_FORMAT("\t\timage: %s, idleAnimInterval: %f, damageAnimInterval: %f \n",
                          image.c_str(), idleAnimInterval, damageAnimInterval);
        
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
};

typedef std::map<std::string, BrickData> BrickMap;
typedef std::vector<BrickData>           BrickList;

#endif /* BrickData_h */