//
//  PreviewBackground.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 20/05/2019.
//

#ifndef PreviewBackground_hpp
#define PreviewBackground_hpp

#include <stdio.h>

#include "../../content/data/model/StageData.h"

class PreviewBackground : public cocos2d::Node, public SBPhysicsObject {
public:
    static PreviewBackground* create(const cocos2d::Size &previewSize, b2World *world);
    ~PreviewBackground();
    
private:
    PreviewBackground();
    
    bool init(const cocos2d::Size &previewSize, b2World *world);
    void onEnter() override;
    
private:
    b2World *world;
    cocos2d::Sprite *bottomImage;
};

#endif /* PreviewBackground_hpp */
