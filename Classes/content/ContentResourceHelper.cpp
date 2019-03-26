//
//  ContentResourceHelper.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 07/02/2019.
//

#include "ContentResourceHelper.hpp"

#include "Define.h"
#include "data/Database.hpp"
#include "User.hpp"

USING_NS_CC;
using namespace std;

/**
 * 컨텐츠 리소스 preload
 */
void ContentResourceHelper::preload() {
    
    auto textureCache = Director::getInstance()->getTextureCache();
    
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    // 선택한 볼 스킨
    textureCache->addImage(SELECTED_BALL_IMAGE);
    
    // 유저 덱의 프렌즈
    // FIXME: 현재는 덱 기능이 없어 모든 프렌즈 리소스 preload
    auto friends = Database::getFriends();
    
    for( auto friendData : friends ) {
        string file = getAnimPNG(getFriendAnimationFile(friendData.friendId));
        textureCache->addImageAsync(file, nullptr);
    }
}

string ContentResourceHelper::getBallImageFile(const string &ballId, const string &imgKey) {
    
    return DIR_BALL + STR_FORMAT("%s_%s.png", ballId.c_str(), imgKey.c_str());
}

string ContentResourceHelper::getSelectedBallImage() {
    
    return BALL_SMALL_IMAGE(User::getSelectedBallSkin());
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

string ContentResourceHelper::getFriendAnimationFile(const string &friendId) {
    
    return DIR_FRIEND + friendId + ANIM_EXT;
}

Animation* ContentResourceHelper::createBrickAnimation(const BrickData &data, BrickImageType type) {
    
    switch( type ) {
        case BrickImageType::IDLE: {
            return SBNodeUtils::createAnimation(data.idleAnims, data.idleAnimInterval);
        }
            
        case BrickImageType::DAMAGE: {
            return SBNodeUtils::createAnimation(data.damageAnims, data.damageAnimInterval);
        }

        case BrickImageType::HIDE: {
            return SBNodeUtils::createAnimation(BRICK_HIDE_ANIM_FILES(data.image), data.idleAnimInterval);
        }
            
        default:
            CCASSERT(false, "ContentResourceHelper::createBrickAnimation error.");
            break;
    }
}
