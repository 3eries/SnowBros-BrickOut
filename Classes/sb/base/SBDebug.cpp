//
//  SBDebug.cpp
//
//  Created by seongmin hwang on 20/12/2018.
//

#include "SBDebug.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN

void Log::d(const string &msg, bool showMessageBox) {
    
    CCLOG("D/%s", msg.c_str());
    
    if( showMessageBox ) {
        MessageBox(msg.c_str(), "DEBUG");
    }
}

void Log::i(const string &msg, bool showMessageBox) {
    
    CCLOG("I/%s", msg.c_str());
    
    if( showMessageBox ) {
        MessageBox(msg.c_str(), "INFO");
    }
}

void Log::w(const string &msg, bool showMessageBox) {
    
    CCLOG("W/%s", msg.c_str());
    
    if( showMessageBox ) {
        MessageBox(msg.c_str(), "WARNING");
    }
}

void Log::e(const string &msg, bool showMessageBox) {
    
    CCLOG("E/%s", msg.c_str());
    
    if( showMessageBox ) {
        MessageBox(msg.c_str(), "ERROR");
    }
}

NS_SB_END
