//
//  FriendInfoView.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 18/04/2019.
//

#ifndef FriendInfoView_hpp
#define FriendInfoView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../content/data/model/FriendData.h"

class FriendInfoView : public cocos2d::Node {
public:
    CREATE_FUNC(FriendInfoView);
    ~FriendInfoView();
    
private:
    enum Tag {
        PREVIEW       = 10,
        
        LABEL_NAME,
        LABEL_DAMAGE,
        LABEL_TYPE,
        LABEL_PRICE,
        
        BTN_BUY,
        BTN_SELECT,
        BTN_UNSELECT,
    };
    
public:
    enum class Button {
        BUY = 10,
        SELECT,
        UNSELECT,
    };
    
private:
    FriendInfoView();
    
    bool init() override;
    
    void initPreview();
    void initText();
    void initButton();
    
public:
    void setFriend(const FriendData &data);
    void updateSelf();
    
private:
    CC_SYNTHESIZE_READONLY(FriendData, data, Data);
    CC_SYNTHESIZE(std::function<void(Button)>, onClickButtonListener, OnClickButtonListener);
};

#endif /* FriendInfoView_hpp */
