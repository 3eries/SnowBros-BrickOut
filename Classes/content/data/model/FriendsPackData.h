//
//  FriendsPackData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 04/04/2019.
//

#ifndef FriendsPackData_h
#define FriendsPackData_h

#include "cocos2d.h"
#include "FriendData.h"

struct FriendsPackData {
    std::string packId;
    std::string packName;
    FriendDataList friends;
    
    FriendsPackData() : packId("") {}
    
    FriendsPackData(const rapidjson::Value &v) : FriendsPackData() {
        packId     = v["pack_id"].GetString();
        packName   = v["pack_name"].GetString();
        
        auto friendDatas = v["friends"].GetArray();
        
        for( int i = 0; i < friendDatas.Size(); ++i ) {
            const rapidjson::Value &friendValue = friendDatas[i];
            friends.push_back(FriendData(friendValue));
        }
    }
    
    bool isNull() const {
        return packId == "";
    }
    
    bool isContainsFriend(const std::string &friendId) const {
        for( auto friendData : friends ) {
            if( friendData.friendId == friendId ) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string toString() const {
        std::string str = "FriendsPackData {\n";
        str += STR_FORMAT("\tpackId: %s, packName: %s\n", packId.c_str(), packName.c_str());
        str += "\tfriends:\n";
        
        for( auto friendData : friends ) {
            str += friendData.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<FriendsPackData>           FriendsPackDataList;
typedef std::map<std::string, FriendsPackData> FriendsPackDataMap;
typedef std::function<void(FriendsPackData)>   FriendsPackDataListener;

#endif /* FriendsPackData_h */
