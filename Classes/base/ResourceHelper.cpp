//
//  ResourceHelper.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResourceHelper.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

void ResourceHelper::preload() {
    
    // image
    auto textureCache = Director::getInstance()->getTextureCache();
    
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    /*
    auto addImageAsync = [textureCache](StringList list) {
      
        for( auto file : list ) {
            textureCache->addImageAsync(file, nullptr);
        }
    };
    */
    
    // add image
    {
        string files[] = {
        };
        
        for( string file : files ) {
            textureCache->addImage(getAnimPNG(file));
        }
    }
    
    // add image async
    {
        string files[] = {
            ANIM_WITHDRAW_GUIDE,
            ANIM_BOSS_WARNING,
            ANIM_CONTINUE,
        };
        
        for( string file : files ) {
            textureCache->addImageAsync(getAnimPNG(file), nullptr);
        }
        
        // ...2.png
        {
            string files[] = {
                ANIM_CONTINUE,
            };
            
            for( string file : files ) {
                const string secondFile = SBStringUtils::replaceAll(getAnimPNG(file), ".png", "2.png");
                textureCache->addImageAsync(getAnimPNG(secondFile), nullptr);
            }
        }
    }
    
    // sound
    SBAudioEngine::preload(SOUND_BGM_MAIN);
    SBAudioEngine::preload(SOUND_BUTTON_CLICK);
}

StringList ResourceHelper::getBrickAnimationFiles(const string &image, const string &animKey) {
    
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

StringList ResourceHelper::getBrickIdleAnimationFiles(const string &image) {
    
    return getBrickAnimationFiles(image, "idle");
}

StringList ResourceHelper::getBrickDamageAnimationFiles(const string &image) {
    
    return getBrickAnimationFiles(image, "damage");
}

