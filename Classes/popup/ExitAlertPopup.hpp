//
//  ExitAlertPopup.hpp
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#ifndef ExitAlertPopup_hpp
#define ExitAlertPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ExitAlertPopup : public BasePopup {
public:
    CREATE_FUNC(ExitAlertPopup);
    ~ExitAlertPopup();
    
private:
    ExitAlertPopup();
    
    bool init() override;
    
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(SBCallback, onExitAppListener, OnExitAppListener);
};

#endif /* ExitAlertPopup_hpp */
