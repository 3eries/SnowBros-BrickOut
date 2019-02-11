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

class ContentResourceHelper {
public:
    static void preload();
    
    static std::string getStageBackgroundFile(int stage);
    
    static StringList  getBrickAnimationFiles(const std::string &image,
                                             const std::string &animKey);
    static StringList  getBrickIdleAnimationFiles(const std::string &image);
    static StringList  getBrickDamageAnimationFiles(const std::string &image);
    static std::string getBrickBackgroundFile(int w, int h, int step);
};

#endif /* ContentResourceHelper_hpp */
