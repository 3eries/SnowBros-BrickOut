//
//  ResourceHelper.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResourceHelper.hpp"

#include "superbomb.h"
#include "Define.h"

USING_NS_CC;
using namespace std;

void ResourceHelper::preload() {
    
    // image
    auto textureCache = Director::getInstance()->getTextureCache();
    
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    auto addImageAsync = [textureCache](StringList list) {
      
        for( auto file : list ) {
            textureCache->addImageAsync(file, nullptr);
        }
    };
    
    // add image
    {
        string files[] = {
            ANIM_CLOUD,
        };
        
        for( string file : files ) {
            textureCache->addImage(getAnimPNG(file));
        }
    }
    
    // add image async
    {
        string files[] = {
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

StringList ResourceHelper::getBrickAnimationFiles(const BrickData &brick, const string &key) {
    
    StringList anims;
    int i = 0;
    
    while( true ) {
        string file = DIR_BRICK + STR_FORMAT("%s_%s%d.png", brick.brickId.c_str(), key.c_str(), i+1);
        
        if( !FileUtils::getInstance()->isFileExist(file) ) {
            break;
        }
        
        anims.push_back(file);
        ++i;
    }
    
    return anims;
}

StringList ResourceHelper::getBrickIdleAnimationFiles(const BrickData &brick) {
    
    return getBrickAnimationFiles(brick, "idle");
}
