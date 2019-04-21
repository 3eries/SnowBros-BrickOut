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
        LABEL_COIN          = 10,
        BTN_SHOP            = 100,
    };
    
private:
    UserCoinView();
    
    bool init(Type type);
    
    void initNormal();
    void initShop();
    
    void onClick(cocos2d::Node *sender);

public:
    void updateCoin(bool withAction);
    
private:
    Type type;
};

#endif /* UserCoinView_hpp */
