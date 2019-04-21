//
//  ShopPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef ShopPopup_hpp
#define ShopPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ShopPopup : public BasePopup {
public:
    CREATE_FUNC(ShopPopup);
    ~ShopPopup();
    
private:
    enum Tag {
        BANNER                = 10,
        
        BTN_CLOSE             = 100,
        BTN_REWARDED_VIDEO,
    };
    
private:
    ShopPopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initCoin();
    void initRemoveAds();
    void initRewardedVideo();
    
    void onClick(cocos2d::Node *sender) override;
    void purchase(const std::string &prodName);
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    cocos2d::Node *shopBg;
};

#endif /* ShopPopup_hpp */
