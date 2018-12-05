//
//  PausePopup.hpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#ifndef PausePopup_hpp
#define PausePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class PausePopup : public BasePopup {
public:
    enum Tag {
        RESUME = 100,
        MAIN,
        REMOVE_ADS,
        HOW_TO_PLAY,
    };
    
public:
    CREATE_FUNC(PausePopup);
    ~PausePopup();
    
private:
    PausePopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;

public:
    void performListener(Tag tag);
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(std::function<void(Tag)>, onClickMenuListener,
                  OnClickMenuListener);
    cocos2d::Node *stoneBg;
};

#endif /* PausePopup_hpp */
