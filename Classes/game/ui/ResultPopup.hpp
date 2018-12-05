//
//  ResultPopup.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef ResultPopup_hpp
#define ResultPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ResultPopup : public BasePopup {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    
public:
    static ResultPopup* create(int score);
    ~ResultPopup();
    
private:
    ResultPopup(int score);
    
    bool init() override;
    void onEnter() override;
    bool onBackKeyReleased() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
public:
    void runEnterAction(float duration, SBCallback onFinished) override;
    void runEnterAction(SBCallback onFinished = nullptr) override;
    
    void runExitAction(float duration, SBCallback onFinished) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    SB_SYNTHESIZE_BOOL(firstEnterAction, FirstEnterAction);
    
    int score;
    
    cocos2d::Node *stone;       // 배경 비석
    cocos2d::Label *scoreLabel; // 스코어 라벨
};

#endif /* ResultPopup_hpp */
