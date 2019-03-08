//
//  Item.cpp
//  SnowBrosItemOut-mobile
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Item.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

USING_NS_CC;
using namespace std;

Item* Item::create(const ItemDef &def) {
    
    auto item = new Item(def);
    
    if( item && item->init() ) {
        item->autorelease();
        return item;
    }
    
    delete item;
    return nullptr;
}

Item::Item(const ItemDef &def) : Game::Tile(1, 1, def.floorData),
data(def.data) {
}

Item::~Item() {
    
}

bool Item::init() {
    
    if( !Game::Tile::init() ) {
        return false;
    }

    initPhysics();
    
    StringList anims;
    
    switch( data.type ) {
        case ItemType::POWER_UP: {
            anims.push_back(DIR_IMG_GAME + "game_item_01_idle1.png");
            anims.push_back(DIR_IMG_GAME + "game_item_01_idle2.png");
        } break;
            
        case ItemType::FRIENDS_POWER_UP: {
            anims.push_back(DIR_IMG_GAME + "game_item_friends_02.png");
        } break;
            
        default:
            CCASSERT(false, "Item::init error: invalid item type.");
            break;
    }
    
    // 이미지 초기화
    image = SBAnimationSprite::create(anims, 0.2f);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(image);
    
    image->runAnimation();
    
    return true;
}

/**
 * 물리 객체 초기화
 */
void Item::initPhysics() {
    
    Size size(getPhysicsSize());
    
    b2BodyDef bodyDef;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2PolygonShape box;
    box.SetAsBox(PTM(size.width*0.5f), PTM(size.height*0.5f));
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::ITEM;
    filter.maskBits = PhysicsCategory::BALL;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.1f;
    fixtureDef.restitution = 1;
    fixtureDef.friction = 0;
    fixtureDef.filter = filter;
    body->CreateFixture(&fixtureDef);
}

Size Item::getPhysicsSize() {
    return ITEM_CONTENT_SIZE;
}

/**
 * 등장
 */
void Item::enterWithAction() {
    
    Game::Tile::enterWithAction();
}

/**
 * 제거
 */
void Item::removeWithAction() {
    
    Game::Tile::removeWithAction();
    
    auto remove = [=]() {
        this->setVisible(false);
        this->setNeedRemove(true);
    };
    
    remove();
    
    /*
    // 제일 아래칸, 바로 제거
    if( getTilePosition().y <= 1 ) {
        remove();
    }
    // 제거 액션
    else {
        auto scale = ScaleTo::create(ENTER_DURATION, 0);
        auto callFunc = CallFunc::create(remove);
        runAction(Sequence::create(scale, callFunc, nullptr));
    }
    */
}
