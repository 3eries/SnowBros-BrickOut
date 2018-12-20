//
//  SBDebug.hpp
//
//  Created by seongmin hwang on 20/12/2018.
//

#ifndef SBDebug_hpp
#define SBDebug_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "SBMacros.h"

// #define DEBUG_LOG(__msg__, __messageBox__)  SBDebug::log(__msg__, __messageBox__)

NS_SB_BEGIN

class Log {
public:
    static void d(const std::string &msg, bool showMessageBox = false);
    static void i(const std::string &msg, bool showMessageBox = false);
    static void w(const std::string &msg, bool showMessageBox = false);
    static void e(const std::string &msg, bool showMessageBox = false);
};

NS_SB_END

#endif /* SBDebug_hpp */
