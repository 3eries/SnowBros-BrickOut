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
    
    setAnchorPoint(ANCHOR_M);
    setPosition(MAP_POSITION);
    setContentSize(MAP_CONTENT_SIZE);
    
    // image
    // TODO:
    auto image = LayerColor::create(Color4B::BLACK);
    image->setIgnoreAnchorPointForPosition(false);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(MAP_CONTENT_SIZE, 0, 0));
    image->setContentSize(MAP_CONTENT_SIZE);
    addChild(image);
    
    // 영역 확인용
    {
        /*
         auto n = LayerColor::create(Color4B(50, 50, 50, 255));
         n->setIgnoreAnchorPointForPosition(false);
         n->setAnchorPoint(bg->getAnchorPoint());
         n->setPosition(bg->getPosition());
         n->setContentSize(MAP_CONTENT_SIZE);
         addChild(n);
         */
        auto n = LayerColor::create(Color4B(50, 50, 50, 255));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(Vec2MC(MAP_CONTENT_SIZE, 0, 0));
        n->setContentSize(MAP_CONTENT_SIZE);
        addChild(n);
    }
    
    // 물리 객체 초기화
    // TODO: 노드 좌표를 월드 좌표로 변환하여 사용
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
