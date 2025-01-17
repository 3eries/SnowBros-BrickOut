//
//  TestHelper.cpp
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#include "TestHelper.hpp"

#include "GameConfiguration.hpp"

USING_NS_CC;
using namespace std;

static TestHelper *instance = nullptr;
TestHelper* TestHelper::getInstance() {
    
    if( !instance ) {
        instance = new TestHelper();
    }
    
    return instance;
}

void TestHelper::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

TestHelper::TestHelper() :
cheatMode(false),
startWorld(1),
startStage(1),
firstBallCount(GAME_CONFIG->getFirstBallCount()),
demoViewBallEnabled(true) {
}

TestHelper::~TestHelper() {
    
}
