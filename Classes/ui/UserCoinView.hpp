//
//  UserCoinView.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef UserCoinView_hpp
#define UserCoinView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class UserCoinView : public cocos2d::Node {
public:
    enum class Type {
        NORMAL,
        SHOP,
    };
    
public:
    static UserCoinView* create(Type type);
    ~UserCoinView();
    
private:
    enum Tag {
        BG                  = 10,
        COIN_ICON           = 100,
        COIN_LABEL,
        SHOP_BTN,
    };
    
private:
    UserCoinView();
    
    bool init(Type type);
    
    void initNormal();
    void initShop();
    
    void onClick(cocos2d::Node *sender);

public:
    void updateCoin(bool withAction);
    
    cocos2d::Node* getBackground();
    cocos2d::Node* getCoinIcon();
    cocos2d::Node* getCoinLabel();
    cocos2d::Node* getShopButton();
    
private:
    Type type;
    SB_SYNTHESIZE_BOOL(updateLocekd, UpdateLocekd);
};

#endif /* UserCoinView_hpp */
