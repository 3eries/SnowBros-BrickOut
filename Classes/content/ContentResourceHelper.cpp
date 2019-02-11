//
//  ContentResourceHelper.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 07/02/2019.
//

#include "ContentResourceHelper.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

void ContentResourceHelper::preload() {
    
}

string ContentResourceHelper::getStageBackgroundFile(int stage) {
    
    return DIR_STAGE + STR_FORMAT("stage_%04d_bg.png", stage);
}

StringList ContentResourceHelper::getBrickAnimationFiles(const string &image, const string &animKey) {
    
    StringList anims;
    int i = 0;
    
    while( true ) {
        string file = DIR_BRICK + STR_FORMAT("%s_%s_%02d.png", image.c_str(), animKey.c_str(), i+1);
        
        if( !FileUtils::getInstance()->isFileExist(file) ) {
            break;
        }
        
        anims.push_back(file);
        ++i;
    }
    
    return anims;
}

StringList ContentResourceHelper::getBrickIdleAnimationFiles(const string &image) {
    return getBrickAnimationFiles(image, "idle");
}

StringList ContentResourceHelper::getBrickDamageAnimationFiles(const string &image) {
    return getBrickAnimationFiles(image, "damage");
}

string ContentResourceHelper::getBrickBackgroundFile(int w, int h, int step) {
    
    string file = DIR_BRICK + STR_FORMAT("brick_bg_%dx%d_%02d.png", w, h, step);
    
    if( !FileUtils::getInstance()->isFileExist(file) ) {
        superbomb::Log::e("ContentResourceHelper::getBrickBackgroundFile invalid file: %s", file.c_str());
    }
    
    return file;
}
