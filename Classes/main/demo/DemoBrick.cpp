//
//  DemoBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#include "DemoBrick.hpp"

#include "Define.h"
#include "ContentManager.hpp"
#include "../../game/GameDefine.h"
#include "../../game/object/tile/Tile.hpp"

USING_NS_CC;
using namespace std;

DemoBrick* DemoBrick::create(const DemoBrickData &demoBrickData, b2World *world) {
    
    auto brick = new DemoBrick();
    
    if( brick && brick->init(demoBrickData, world) ) {
        brick->autorelease();
        return brick;
    }
    
    CC_SAFE_DELETE(brick);
    return nullptr;
}

DemoBrick::DemoBrick() : SBPhysicsObject(this) {
}

DemoBrick::~DemoBrick() {
}

bool DemoBrick::init(const DemoBrickData &data, b2World *world) {
    
    if( !Node::init() ) {
        return false;
    }
    
    this->data = data;
    
    auto brickData = data.brick;
    auto size = MEASURE_TILE_SIZE(brickData.width, brickData.height);
    
    setAnchorPoint(ANCHOR_M);
    setContentSize(size);
    setPosition(Game::Tile::convertToTilePosition(data.pos, brickData.width, brickData.height));
    
    // bg
    auto bg = Sprite::create(ContentResourceHelper::getBrickBackgroundFile(brickData.width,
                                                                           brickData.height, 1));
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(size, 0, 0));
    addChild(bg);
    
    // image
    image = SBAnimationSprite::create();
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(size, 0, brickData.is1x1() ? 5 : 0));
    image->setFlippedX(data.isFlippedX);
    image->setFlippedY(data.isFlippedY);
    addChild(image);
    
    setImage(BrickImageType::IDLE, true);
    
    // physics
    b2BodyDef bodyDef;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2PolygonShape box;
    box.SetAsBox(PTM(size.width*0.5f), PTM(size.height*0.5f));
    
    auto body = world->CreateBody(&bodyDef);
    setBody(body);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::BRICK;
    filter.maskBits = PhysicsCategory::BALL;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.1f;
    fixtureDef.restitution = 1;
    fixtureDef.friction = 0;
    fixtureDef.filter = filter;
    body->CreateFixture(&fixtureDef);
    
    syncNodeToBody();
    
    return true;
}

/**
 * 브릭 이미지 설정
 */
void DemoBrick::setImage(BrickImageType type, bool isRunAnimation) {
    
    this->imageType = type;
    
    auto anim = BRICK_ANIMATION(data.brick, type);
    
    switch( type ) {
        case BrickImageType::IDLE:        image->setAnimation(anim);       break;
        case BrickImageType::DAMAGE:      image->setAnimation(anim, 1);    break;
        default:
            CCASSERT(false, "DemoBrick::setImage error.");
            break;
    }
    
    if( isRunAnimation ) {
        image->runAnimation();
    }
}

void DemoBrick::onContactBrick() {
    
    // 브릭 애니메이션 변경
    if( imageType != BrickImageType::DAMAGE ) {
        setImage(BrickImageType::DAMAGE, false);
        
        image->runAnimation([=](Node*) {
            this->setImage(BrickImageType::IDLE, true);
        });
    }
}
