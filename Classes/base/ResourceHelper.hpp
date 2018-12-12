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

#include "../game/db/BrickData.h"

class ResourceHelper {
public:
    static void preload();
    
    static StringList getBrickAnimationFiles(const BrickData &brick,
                                             const std::string &key);
    static StringList getBrickIdleAnimationFiles(const BrickData &brick);
};

#endif /* ResourceHelper_hpp */
