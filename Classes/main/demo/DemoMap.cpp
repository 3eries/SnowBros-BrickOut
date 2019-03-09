//
//  DemoMap.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/03/2019.
//

#include "DemoMap.hpp"

#include "Define.h"
#include "ContentManager.hpp"
#include "../../game/GameDefine.h"

USING_NS_CC;
using namespace std;

DemoMap* DemoMap::create(const StageData &stage, b2World *world) {
    
    auto map = new DemoMap();
    
    if( map && map->init(stage, world) ) {
        map->autorelease();
        return map;
    }
    
    CC_SAFE_DELETE(map);
    return nullptr;
}

DemoMap::DemoMap() : SBPhysicsObject(this) {
}

DemoMap::~DemoMap() {
}

bool DemoMap::init(const StageData &stage, b2World *world) {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    setSyncLocked(true);
    
    // 배경 초기화
    {
        auto center = Sprite::create(ContentResourceHelper::getStageBackgroundFile(stage.stage));
        center->setAnchorPoint(ANCHOR_M);
        center->setPosition(Vec2MC(0,0));
        center->setScale(4);
        addChild(center);
    
        auto bottom = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
        bottom->setAnchorPoint(ANCHOR_MB);
        bottom->setPosition(Vec2BC(0,0));
        addChild(bottom);
    }
    
    // 물리 객체 초기화
    b2BodyDef bodyDef;
    bodyDef.position = PTM(MAP_POSITION);
    bodyDef.userData = (SBPhysicsObject*)this;
    
    auto body = world->CreateBody(&bodyDef);
    setBody(body);
    
    float left   = PTM(-MAP_CONTENT_SIZE.width*0.5f);
    float right  = PTM( MAP_CONTENT_SIZE.width*0.5f);
    float bottom = PTM(-MAP_CONTENT_SIZE.height*0.5f);
    float top    = PTM(SB_WIN_SIZE.height-MAP_POSITION.y);
    
    b2Vec2 vectors[4][2] = {
        { b2Vec2(left, bottom), b2Vec2(left, top) },          // left
        { b2Vec2(right, bottom), b2Vec2(right, top) },        // right
        { b2Vec2(left, bottom), b2Vec2(right, bottom) },      // bottom
        { b2Vec2(left, top), b2Vec2(right, top) },            // top
    };
    
    PhysicsCategory categorys[] = {
        PhysicsCategory::WALL,
        PhysicsCategory::WALL,
        PhysicsCategory::FLOOR,
        PhysicsCategory::WALL,
    };
    
    for( int i = 0; i < 4; ++i ) {
        b2Vec2 v1 = vectors[i][0];
        b2Vec2 v2 = vectors[i][1];
        
        b2EdgeShape shape;
        shape.Set(v1, v2);
        
        b2Filter filter;
        filter.categoryBits = categorys[i];
        filter.maskBits = PhysicsCategory::BALL;
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 0.1f;      // 밀도
        fixtureDef.restitution = 1;     // 반발력 - 물체가 다른 물체에 닿았을때 팅기는 값
        fixtureDef.friction = 0;        // 마찰력
        fixtureDef.filter = filter;
        body->CreateFixture(&fixtureDef);
    }
    
    return true;
}
