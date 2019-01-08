//
//  ResourceHelper.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef ResourceHelper_hpp
#define ResourceHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class ResourceHelper {
public:
    static void preload();
    
    static StringList getBrickAnimationFiles(const std::string &image,
                                             const std::string &animKey);
    static StringList getBrickIdleAnimationFiles(const std::string &image);
    static StringList getBrickDamageAnimationFiles(const std::string &image);
};

#endif /* ResourceHelper_hpp */
