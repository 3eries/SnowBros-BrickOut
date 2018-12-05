//
//  GiftBox.hpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#ifndef GiftBox_hpp
#define GiftBox_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "GiftDefine.h"

/** @class GiftBox
 * @brief 화면에 표시되는 선물 박스 UI
 */
class GiftBox : public cocos2d::Node {
public:
    static GiftBox* create(GiftType type);
    ~GiftBox();
    
private:
    GiftBox(GiftType type);
    
    bool init() override;
    void onEnter() override;
    
    void initTouch();
    void initImage();
    
public:
    void remove();
    
    void move(float firstDelayTime = 0);
    
private:
    CC_SYNTHESIZE_READONLY(GiftType, type, Type);
    CC_SYNTHESIZE(SBCallbackNode, onClickListener, OnClickListener);
    
    SBAnimationSprite *image;
};

#endif /* GiftBox_hpp */
