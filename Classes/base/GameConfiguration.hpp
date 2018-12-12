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

struct LevelInfo {
    int level;
    int score;         // 레벨 시작 스코어
    
    LevelInfo() {
        level = -1;
    }
};

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
    
    LevelInfo getLevel(int level);
    LevelInfo getLevelByScore(int score);
    LevelInfo getMaxLevel();
    
private:
    CC_SYNTHESIZE_READONLY(std::string, storeUrl, StoreUrl);
    
    // 맵의 가로줄에 표시되는 타일 갯수
    CC_SYNTHESIZE_READONLY(int, tileRows, TileRows);

    // 맵의 세로줄에 표시되는 타일 갯수
    CC_SYNTHESIZE_READONLY(int, tileColumns, TileColumns);

    CC_SYNTHESIZE_READONLY(int, firstBallCount, FirstBallCount);        // 최초 볼 갯수
    CC_SYNTHESIZE_READONLY(int, maxBallCount, MaxBallCount);            // 최초 볼 갯수
    CC_SYNTHESIZE_READONLY(int, maxScore, MaxScore);                    // 최대 스코어
    
    // 레벨 정보
    CC_SYNTHESIZE(std::vector<LevelInfo>, levelInfos, LevelInfos);
};

#endif /* GameConfiguration_hpp */
