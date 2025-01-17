//
//  MainScene.hpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef MainScene_hpp
#define MainScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseScene.hpp"
#include "GiftDefine.h"

class MainScene : public BaseScene {
private:
    enum Tag {
        TAP_TO_START        = 10,
        
        BANNER              = 50,
        
        BTN_TITLE           = 100,
        BTN_START,
        BTN_SETTING,
        BTN_BALL_SKIN,
        BTN_FRIENDS,
        BTN_SHOP,
        BTN_LEADERBOARD,
        
        BTN_TEST,
    };
    
public:
    CREATE_FUNC(MainScene);
    ~MainScene();
    
private:
    MainScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    bool onBackKeyReleased() override;
    
    void initBg();
    void initMenu();
    void initFriends();
    
    void initIAPListener();
    void initGiftListener();
    void initPopupListener();
    
    void updateFriendsUI();
    
private:
    void onClick(cocos2d::Node *sender) override;
    
    void showSettingPopup();
    void showBallSkinPopup();
    void showFriendsPopup();
    
private:
    std::vector<spine::SkeletonAnimation*> friends;
};

#endif /* MainScene_hpp */
