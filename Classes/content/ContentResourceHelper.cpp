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

string ContentResourceHelper::getBrickBackgroundFile(const BrickData &data, bool elite, int step) {
    
    string type = "";
    
    if( elite )                   type = "_elite";
    else if( data.isSpecial() )   type = "_special";
    
    string file = DIR_BRICK + STR_FORMAT("brick_bg_%dx%d%s_%02d.png",
                                         data.width, data.height, type.c_str(), step);
    
    if( !FileUtils::getInstance()->isFileExist(file) ) {
        superbomb::Log::e("ContentResourceHelper::getBrickBackgroundFile invalid file: %s", file.c_str());
    }
    
    return file;
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

Animation* ContentResourceHelper::createBrickAnimation(const BrickData &data, BrickImageType type) {
    
    switch( type ) {
        case BrickImageType::IDLE: {
            return SBNodeUtils::createAnimation(data.idleAnims, data.idleAnimInterval);
        }
            
        case BrickImageType::DAMAGE: {
            return SBNodeUtils::createAnimation(data.damageAnims, data.damageAnimInterval);
        }
            
        default:
            CCASSERT(false, "ContentResourceHelper::createBrickAnimation error.");
            break;
    }
}
