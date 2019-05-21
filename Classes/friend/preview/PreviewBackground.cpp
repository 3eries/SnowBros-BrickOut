//
//  PreviewBackground.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 20/05/2019.
//

#include "PreviewBackground.hpp"

#include "Define.h"
#include "ContentManager.hpp"
#include "../../game/GameDefine.h"

USING_NS_CC;
using namespace std;

PreviewBackground* PreviewBackground::create(const Size &previewSize, b2World *world) {
    
    auto map = new PreviewBackground();
    
    if( map && map->init(previewSize, world) ) {
        map->autorelease();
        return map;
    }
    
    CC_SAFE_DELETE(map);
    return nullptr;
}

PreviewBackground::PreviewBackground() : SBPhysicsObject(this) {
}

PreviewBackground::~PreviewBackground() {
}

bool PreviewBackground::init(const Size &previewSize, b2World *world) {
    
    if( !Node::init() ) {
        return false;
    }
    
    this->world = world;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(previewSize);
    setSyncLocked(true);
    
    // 배경 초기화
    {
        auto center = Sprite::create(ContentResourceHelper::getStageBackgroundFile(1));
        center->setAnchorPoint(ANCHOR_MB);
        center->setPosition(Vec2BC(previewSize, 0, 0));
        center->setScale(3);
        addChild(center);
        
        bottomImage = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
        bottomImage->setAnchorPoint(ANCHOR_MB);
        bottomImage->setPosition(Vec2BC(previewSize, 0,0));
        bottomImage->setScale(0.75f);
        addChild(bottomImage);
    }
    
    return true;
}

void PreviewBackground::onEnter() {
    
    Node::onEnter();
    
    // 물리 객체 초기화
    auto box = SB_BOUNDING_BOX_IN_WORLD(this);
    
    b2BodyDef bodyDef;
    bodyDef.position = PTM(box.getMidX(), box.getMidY());
    bodyDef.userData = (SBPhysicsObject*)this;
    
    auto body = world->CreateBody(&bodyDef);
    setBody(body);
    
    float left   = PTM(-box.size.width*0.5f);
    float right  = PTM( box.size.width*0.5f);
    float bottom = PTM(-box.size.height*0.5f + (bottomImage->getContentSize().height * bottomImage->getScaleY()));
    float top    = PTM( box.size.height*0.5f);
    
    b2Vec2 vectors[4][2] = {
        { b2Vec2(left, bottom), b2Vec2(left, top) },          // left
        { b2Vec2(right, bottom), b2Vec2(right, top) },        // right
        { b2Vec2(left, bottom), b2Vec2(right, bottom) },      // bottom
        { b2Vec2(left, top), b2Vec2(right, top) },            // top
    };
    
    PhysicsCategory categorys[] = {
        PhysicsCategory::WALL_LEFT,
        PhysicsCategory::WALL_RIGHT,
        PhysicsCategory::FLOOR,
        PhysicsCategory::WALL_TOP,
    };
    
    for( int i = 0; i < 4; ++i ) {
        b2Vec2 v1 = vectors[i][0];
        b2Vec2 v2 = vectors[i][1];
        
        b2EdgeShape shape;
        shape.Set(v1, v2);
        
        b2Filter filter;
        filter.categoryBits = categorys[i];
        filter.maskBits = PHYSICS_MASK_BITS_WALL;
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 0.1f;      // 밀도
        fixtureDef.restitution = 1;     // 반발력 - 물체가 다른 물체에 닿았을때 팅기는 값
        fixtureDef.friction = 0;        // 마찰력
        fixtureDef.filter = filter;
        body->CreateFixture(&fixtureDef);
    }
}
