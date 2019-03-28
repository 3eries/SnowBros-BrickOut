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
    NONE            = 0,
    NORMAL,                 // 기본 볼
    ONE_SHOT,               // 1개만 발사
    SPLASH,                 // 광역 데미지
    PASS,                   // 브릭 통과
};

struct FriendData {
    std::string friendId;
    std::string name;
    FriendType type;
    int power;
    
    FriendData() : friendId("") {}
    
    FriendData(const rapidjson::Value &v) {
        friendId     = v["friend_id"].GetString();
        name         = v["name"].GetString();
        type         = (FriendType)v["type"].GetInt();
        power        = v["power"].GetInt();
    }
    
    bool isNull() const {
        return friendId == "";
    }
    
    std::string toString() {
        return STR_FORMAT("FriendData { friendId: %s, name: %s, type: %d, power: %d }",
                          friendId.c_str(), name.c_str(), (int)type, power);
    }
};

typedef std::vector<FriendData>           FriendDataList;
typedef std::map<std::string, FriendData> FriendDataMap;
typedef std::function<void(FriendData)>   FriendDataListener;

#endif /* FriendData_h */
