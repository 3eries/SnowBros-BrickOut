//
//  FriendData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 21/03/2019.
//

#ifndef FriendData_h
#define FriendData_h

#include "cocos2d.h"

struct FriendData {
    std::string friendId;
    
    FriendData() : friendId("") {}
    
    bool isNull() const {
        return friendId == "";
    }
    
    std::string toString() {
        return STR_FORMAT("FriendData { friendId: %s }",
                          friendId.c_str());
    }
};

typedef std::vector<FriendData>           FriendDataList;
typedef std::map<std::string, FriendData> FriendDataMap;
typedef std::function<void(FriendData)>   FriendDataListener;

#endif /* FriendData_h */
