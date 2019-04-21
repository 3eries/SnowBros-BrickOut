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

static ContentResourceHelper *instance = nullptr;
ContentResourceHelper* ContentResourceHelper::getInstance() {
    
    if( !instance ) {
        instance = new ContentResourceHelper();
    }
    
    return instance;
}

void ContentResourceHelper::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

ContentResourceHelper::ContentResourceHelper() {
}

ContentResourceHelper::~ContentResourceHelper() {
}

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
    auto deck = User::getFriendsDeck();
    
    for( auto friendData : deck ) {
        string file = getAnimPNG(getFriendAnimationFile(friendData.friendId));
        textureCache->addImageAsync(file, nullptr);
    }
}

/**
 * 볼 이미지를 반환합니다
 */
string ContentResourceHelper::getBallImageFile(const string &ballId, const string &imgKey) {
    
    return DIR_CONTENT_BALL + STR_FORMAT("%s_%s.png", ballId.c_str(), imgKey.c_str());
}

string ContentResourceHelper::getSelectedBallImage() {
    
    return BALL_SMALL_IMAGE(User::getSelectedBallSkin());
}

/**
 * 스테이지 배경 이미지를 반환합니다
 */
string ContentResourceHelper::getStageBackgroundFile(int stage) {
    
    return DIR_CONTENT_STAGE + STR_FORMAT("stage_%04d_bg.png", stage);
}

/**
 * 브릭 배경 이미지를 반환합니다
 */
string ContentResourceHelper::getBrickBackgroundFile(const BrickData &data, bool elite, int step) {
    
    string type = "";
    
    if( elite )                   type = "_elite";
    else if( data.isSpecial() )   type = "_special";
    
    string file = DIR_CONTENT_BRICK + STR_FORMAT("brick_bg_%dx%d%s_%02d.png",
                                         data.width, data.height, type.c_str(), step);
    
    if( !FileUtils::getInstance()->isFileExist(file) ) {
        superbomb::Log::e("ContentResourceHelper::getBrickBackgroundFile invalid file: %s", file.c_str());
    }
    
    return file;
}

/**
 * 브릭 애니메이션을 반환합니다
 */
StringList ContentResourceHelper::getBrickAnimationFiles(const string &image, const string &animKey) {
    
    StringList anims;
    int i = 0;
    
    while( true ) {
        string file = DIR_CONTENT_BRICK + STR_FORMAT("%s_%s_%02d.png", image.c_str(), animKey.c_str(), i+1);
        
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
            
        case BrickImageType::HIDE: {
            return SBNodeUtils::createAnimation(BRICK_HIDE_ANIM_FILES(data.image), data.idleAnimInterval);
        }
            
        default:
            CCASSERT(false, "ContentResourceHelper::createBrickAnimation error.");
            break;
    }
    
    return nullptr;
}

/**
 * 브릭 흰색 텍스처를 저장합니다
 */
Texture2D* ContentResourceHelper::addBrickWhiteTexture(Texture2D *originTexture,
                                                       bool isFlippedX, bool isFlippedY) {
    
    auto getKey = [=]() -> string {
        string path = originTexture->getPath();
        string fileName = path.substr(path.find_last_of("/") + 1);
        string image = SBStringUtils::replaceAll(fileName, ".png", "");
        
        return STR_FORMAT("%s_%d_%d", image.c_str(), isFlippedX, isFlippedY);
    };
    
    string key = getKey();
    
    // 캐시된 텍스처 있는지 체크
    Texture2D *tex = getBrickWhiteTexture(key);
    
    if( tex ) {
        return tex;
    }
    
    // 텍스처를 생성하여 저장합니다
    auto size = originTexture->getContentSize();
    
    auto renderTexture = RenderTexture::create((int)size.width, (int)size.height,
                                               Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
    
    auto stencil = Sprite::createWithTexture(originTexture);
    stencil->setAnchorPoint(ANCHOR_M);
    stencil->setPosition(Vec2MC(size, 0,0));
    stencil->setFlippedX(isFlippedX);
    
    // RenderTexture에서 스프라이트가 상하 반전되기 때문에 원래 값을 반전 시킨다
    stencil->setFlippedY(!isFlippedY);
    
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setAnchorPoint(ANCHOR_M);
    clippingNode->setPosition(Vec2MC(size, 0,0));
    clippingNode->setContentSize(size);
    clippingNode->setAlphaThreshold(0);
    
    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2::ZERO, stencil->getContentSize(), Color4F::WHITE);
    clippingNode->addChild(drawNode);
    
    renderTexture->begin();
    clippingNode->Node::visit();
    renderTexture->end();
    
    tex = renderTexture->getSprite()->getTexture();
    brickWhiteTextures.insert(key, tex);
    
    return tex;
}

/**
 * 브릭 흰색 텍스처를 반환합니다
 */
Texture2D* ContentResourceHelper::getBrickWhiteTexture(const string &key) {

    if( brickWhiteTextures.find(key) != brickWhiteTextures.end() ) {
        return brickWhiteTextures.at(key);
    }
    
    return nullptr;
}

/**
 * 브릭 흰색 텍스처를 제거합니다
 */
void ContentResourceHelper::removeBrickWhiteTextures() {
    
    brickWhiteTextures.clear();
}

/**
 * 프렌즈 이미지를 반환합니다
 */
string ContentResourceHelper::getFriendAnimationFile(const string &friendId) {
    
    return DIR_CONTENT_FRIEND + friendId + ANIM_EXT;
}

StringList ContentResourceHelper::getFriendBallAnimationFiles(const string &friendId) {
    
    StringList anims;
    int i = 0;
    
    while( true ) {
        string file = DIR_CONTENT_FRIEND + STR_FORMAT("%s_ball_%02d.png", friendId.c_str(), i+1);
        
        if( !FileUtils::getInstance()->isFileExist(file) ) {
            break;
        }
        
        anims.push_back(file);
        ++i;
    }
    
    return anims;
}

spine::SkeletonAnimation* ContentResourceHelper::createFriendBrickDamageAnimation(const string &friendId,
                                                                                  const BrickData &brickData) {
    
    string file = DIR_CONTENT_FRIEND + STR_FORMAT("%s_brick_damage.json", friendId.c_str());
    
    if( !FileUtils::getInstance()->isFileExist(file) ) {
        return nullptr;
    }
    
    string animName = STR_FORMAT("run_%dx%d", brickData.width, brickData.height);
    
    auto anim = SBSpineHelper::runAnimation(nullptr, file, animName, true);
    return anim;
}
