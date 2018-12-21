//
//  GameMap.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "GameMap.hpp"

#include "Define.h"
#include "../GameDefine.h"
#include "../GameManager.hpp"

USING_NS_CC;
using namespace std;

GameMap::GameMap() : SBPhysicsObject(this) {
    
}

GameMap::~GameMap() {
    
}

bool GameMap::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    setSyncLocked(true);
    
    // image
    // game_bg_01.png Vec2BC(0, 640) , Size(720, 1280)
    auto bg = Sprite::create(DIR_IMG_GAME + "game_bg_01.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg);
    
    // game_bg_bottom.png Vec2BC(0, 26) , Size(720, 52)
    auto bottomBg = Sprite::create(DIR_IMG_GAME + "game_bg_bottom.png");
    bottomBg->setAnchorPoint(ANCHOR_M);
    bottomBg->setPosition(Vec2BC(0, 26));
    addChild(bottomBg);
    
    // game_dead_line.png Vec2BC(0, 216) , Size(715, 3)
    /*
    auto deadLine = Sprite::create(DIR_IMG_GAME + "game_dead_line.png");
    deadLine->setAnchorPoint(ANCHOR_M);
    deadLine->setPosition(Vec2BC(0, 216));
    addChild(deadLine);
    */
    
    // 영역 확인용
    /*
    {
        auto n = LayerColor::create(Color4B(255, 0, 0, 255*0.2f));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(MAP_POSITION);
        n->setContentSize(MAP_CONTENT_SIZE);
        addChild(n);
    }
    */
     
    // 물리 객체 초기화
    b2BodyDef bodyDef;
    bodyDef.position = PTM(MAP_POSITION);
    bodyDef.userData = (SBPhysicsObject*)this;
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
    float left   = PTM(-MAP_CONTENT_SIZE.width*0.5f);
    float right  = PTM( MAP_CONTENT_SIZE.width*0.5f);
    float bottom = PTM(-MAP_CONTENT_SIZE.height*0.5f);
    float top    = PTM( MAP_CONTENT_SIZE.height*0.5f);
    
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
