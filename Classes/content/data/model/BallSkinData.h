//
//  BallSkinData.h
//  SnowBrosBrickOut
//
//  Created by seongmin hwang on 14/03/2019.
//

#ifndef BallSkinData_h
#define BallSkinData_h

#include "cocos2d.h"

enum class BallSkinType {
    NONE        = 0,
    COIN        = 1,
    ADS         = 100,
};

struct BallSkinData {
    std::string ballId;
    BallSkinType type;
    int unlockAmount;
    
    BallSkinData() : type(BallSkinType::NONE) {}
    
    std::string toString() {
        return STR_FORMAT("BallSkinData { ballId: %s type: %d unlockAmount: %d }",
                          ballId.c_str(), (int)type, unlockAmount);
    }
};

typedef std::vector<BallSkinData>           BallSkinDataList;
typedef std::map<std::string, BallSkinData> BallSkinDataMap;
typedef std::function<void(BallSkinData)>   BallSkinDataListener;

#endif /* BallSkinData_h */
