//
//  FriendData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 21/03/2019.
//

#ifndef FriendData_h
#define FriendData_h

#include "cocos2d.h"

enum class FriendType {
    NONE        = 0,
    NORMAL,     // 일반
    CRITICAL,   // 크리티컬
    SPLASH,     // 범위 공격
    PASS,       // 브릭 통과
};

struct FriendData {
    std::string friendId;
    std::string name;
    int price;
    FriendType type;
    bool isSingleBall;
    bool isRemoveAfterBrickContact;
    int damage;
    float shootingInterval;
    float ballSpeed;
    int brickDamageEffectZOrder;
    
    FriendData() : friendId(""), isSingleBall(false), isRemoveAfterBrickContact(false), brickDamageEffectZOrder(0) {}
    
    FriendData(const rapidjson::Value &v) : FriendData() {
        friendId                = v["friend_id"].GetString();
        name                    = v["name"].GetString();
        price                   = v["price"].GetInt();
        type                    = (FriendType)v["type"].GetInt();
        damage                  = v["damage"].GetInt();
        shootingInterval        = v["shooting_interval"].GetFloat();
        ballSpeed               = v["ball_speed"].GetFloat();
        
        if( v.HasMember("single_ball") )
            isSingleBall = v["single_ball"].GetBool();
        
        if( v.HasMember("remove_after_brick_contact") )
            isRemoveAfterBrickContact = v["remove_after_brick_contact"].GetBool();
        
        if( v.HasMember("brick_damage_effect_zorder") )
            brickDamageEffectZOrder = v["brick_damage_effect_zorder"].GetInt();
    }
    
    bool isNull() const {
        return friendId == "";
    }
    
    std::string toString() {
        return STR_FORMAT("FriendData { friendId: %s, name: %s, price: %d, type: %d, isSingleBall: %d, damage: %d }",
                          friendId.c_str(), name.c_str(), price, (int)type, isSingleBall, damage);
    }
};

typedef std::vector<FriendData>           FriendDataList;
typedef std::map<std::string, FriendData> FriendDataMap;
typedef std::function<void(FriendData)>   FriendDataListener;

#endif /* FriendData_h */
