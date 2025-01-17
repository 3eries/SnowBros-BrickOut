//
//  GameConfiguration.hpp
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#ifndef GameConfiguration_hpp
#define GameConfiguration_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Define.h"

#define GAME_CONFIG     GameConfiguration::getInstance()

class GameConfiguration : public cocos2d::Ref {
public:
    static GameConfiguration* getInstance();
    static void destroyInstance();
    ~GameConfiguration();
    
private:
    GameConfiguration();
    
public:
    void init();
    void parse(const std::string &json);
    
private:
    CC_SYNTHESIZE_READONLY(std::string, storeUrl, StoreUrl);
    CC_SYNTHESIZE_READONLY(int, firstCoin, FirstCoin);                  // 최초 코인
    CC_SYNTHESIZE_READONLY(std::string, firstBallSkin, FirstBallSkin);  // 최초 볼 스킨
    CC_SYNTHESIZE_READONLY(std::string, firstFriend, FirstFriend);      // 최초 프렌드
    CC_SYNTHESIZE_READONLY(int, restoreCost, RestoreCost);              // 리스토어 비용
    
    // 맵의 가로줄에 표시되는 타일 갯수
    CC_SYNTHESIZE_READONLY(int, tileRows, TileRows);
    // 맵의 세로줄에 표시되는 타일 갯수
    CC_SYNTHESIZE_READONLY(int, tileColumns, TileColumns);
    // 타일 등장 시간
    CC_SYNTHESIZE_READONLY(float, tileEnterDuration, TileEnterDuration);
    
    CC_SYNTHESIZE_READONLY(int, firstBallCount, FirstBallCount);        // 최초 볼 갯수
    CC_SYNTHESIZE_READONLY(int, firstFriendsDamage, FirstFriendsDamage);// 최초 프렌즈 데미지
    CC_SYNTHESIZE_READONLY(int, maxBallCount, MaxBallCount);            // 최대 볼 갯수
    CC_SYNTHESIZE_READONLY(int, maxScore, MaxScore);                    // 최대 스코어
};

#endif /* GameConfiguration_hpp */
