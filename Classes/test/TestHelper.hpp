//
//  TestHelper.hpp
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#ifndef TestHelper_hpp
#define TestHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#define ENABLE_TEST_MENU        0
#define TEST_HELPER             TestHelper::getInstance()

class TestHelper : public cocos2d::Ref {
public:
    static TestHelper* getInstance();
    static void destroyInstance();
    
    ~TestHelper();
    
private:
    TestHelper();
    
private:
    SB_SYNTHESIZE_BOOL(cheatMode, CheatMode);                     // 치트 모드 on/off
    CC_SYNTHESIZE(int, startWorld, StartWorld);                   // 시작 월드
    CC_SYNTHESIZE(int, startStage, StartStage);                   // 시작 스테이지
    CC_SYNTHESIZE(int, firstBallCount, FirstBallCount);           // 최초 볼 갯수
    SB_SYNTHESIZE_BOOL(demoViewBallEnabled, DemoViewBallEnabled); // 데모뷰 볼 on/off
};

#endif /* TestHelper_hpp */
