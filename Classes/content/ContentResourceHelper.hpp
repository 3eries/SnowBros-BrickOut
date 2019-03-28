//
//  ContentResourceHelper.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 07/02/2019.
//

#ifndef ContentResourceHelper_hpp
#define ContentResourceHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "data/model/BrickData.h"

#define BALL_SMALL_IMAGE(__ball__) \
ContentResourceHelper::getBallImageFile(__ball__, "small")

#define BALL_LARGE_IMAGE(__ball__) \
ContentResourceHelper::getBallImageFile(__ball__, "large")

#define SELECTED_BALL_IMAGE \
ContentResourceHelper::getSelectedBallImage()

#define BRICK_IDLE_ANIM_FILES(__image__) \
ContentResourceHelper::getBrickAnimationFiles(__image__, "idle")

#define BRICK_DAMAGE_ANIM_FILES(__image__) \
ContentResourceHelper::getBrickAnimationFiles(__image__, "damage")

#define BRICK_HIDE_ANIM_FILES(__image__) \
ContentResourceHelper::getBrickAnimationFiles(__image__, "hide")

#define BRICK_WHITE_IMAGE(__image__) \
std::string("white_" + __image__)

#define BRICK_ANIMATION(__data__, __type__) \
ContentResourceHelper::createBrickAnimation(__data__, __type__)

class ContentResourceHelper {
public:
    static void preload();
    
    static std::string getBallImageFile(const std::string &ballId,
                                        const std::string &imgKey);
    static std::string getSelectedBallImage();
    
    static std::string getStageBackgroundFile(int stage);
    
    static std::string getBrickBackgroundFile(const BrickData &data, bool elite, int step);
    static StringList  getBrickAnimationFiles(const std::string &image,
                                             const std::string &animKey);
    
    // static std::string getBrickWhiteImageFile(const std::string &image);
    
    static std::string getFriendAnimationFile(const std::string &friendId);
    static std::string getFriendBallImageFile(const std::string &friendId);
    
    static cocos2d::Animation* createBrickAnimation(const BrickData &data,
                                                    BrickImageType type);
};

#endif /* ContentResourceHelper_hpp */
