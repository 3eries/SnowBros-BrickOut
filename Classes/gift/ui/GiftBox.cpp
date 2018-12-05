//
//  GiftBox.cpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#include "GiftBox.hpp"

#include "Define.h"

USING_NS_CC;
USING_NS_SB;
using namespace std;

GiftBox* GiftBox::create(GiftType type) {
 
    auto obj = new GiftBox(type);
    
    if( obj && obj->init() ) {
        obj->autorelease();
        return obj;
    }
    
    CC_SAFE_DELETE(obj);
    return nullptr;
}

GiftBox::GiftBox(GiftType type) :
type(type),
onClickListener(nullptr) {
}

GiftBox::~GiftBox() {
}

bool GiftBox::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    
    initImage();
    initTouch();

    return true;
}

void GiftBox::onEnter() {
    
    Node::onEnter();
    
    // DebugDraw: 상자 이동 범위
    /*
    const float MIN_Y = (SB_WIN_SIZE.height * 0.25f);
    const float MAX_Y = (SB_WIN_SIZE.height * 0.75f) + 110;
     
    {
        auto n = LayerColor::create(Color4B::RED);
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(Vec2BC(0, MIN_Y));
        n->setContentSize(Size(SB_WIN_SIZE.width, 10));
        getScene()->addChild(n);
    }
    
    {
        auto n = LayerColor::create(Color4B::RED);
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(Vec2BC(0, MAX_Y));
        n->setContentSize(Size(SB_WIN_SIZE.width, 10));
        getScene()->addChild(n);
    }
    */
}

/**
 * 오브젝트 삭제
 */
void GiftBox::remove() {
    
    auto fadeOut = FadeOut::create(0.1f);
    auto callFunc = CallFunc::create([=]() {
        this->removeFromParent();
    });
    runAction(Sequence::create(fadeOut, callFunc, nullptr));
}

/**
 * 이동 액션
 */
void GiftBox::move(float firstDelayTime) {
    
    const Size  BOX_SIZE(getContentSize());
    const float BOX_WIDTH = BOX_SIZE.width;
    const float BOX_HEIGHT = BOX_SIZE.height;

    const float MIN_X = -BOX_WIDTH*0.5f;
    const float MAX_X = SB_WIN_SIZE.width + (BOX_WIDTH*0.5f);
    const float MIN_Y = (SB_WIN_SIZE.height * 0.25f);
    const float MAX_Y = (SB_WIN_SIZE.height * 0.75f) + 110;
    
    // 방향 설정
    const int direction = (random<int>(0,1) == 0) ? -1 : 1;
    const bool isMoveRight = (direction == 1);

    // 좌표 설정
    const float startX = isMoveRight ? MIN_X : MAX_X;
    const float endX   = isMoveRight ? MAX_X : MIN_X;
    
//    const float startY = (SB_WIN_SIZE.height * 0.25f) * (random<int>(1, 3));
    const float startY = random<int>(SB_WIN_SIZE.height*0.35f, SB_WIN_SIZE.height*0.65f);
    setPosition(Vec2(startX, startY));
    
    // 베지어 곡선
    const int   BEZIER_COUNT = random<int>(2,4);
    const float BEZIER_TOTAL_DURATION = 16.0f;
    const float BEZIER_DURATION = BEZIER_TOTAL_DURATION / BEZIER_COUNT;
    const float RANGE_X = (SB_WIN_SIZE.width / (BEZIER_COUNT * 3)) * direction;
    
    float posX = startX;
    float posY = startY;
    
    Vector<FiniteTimeAction*> beziers;
    
    for( int i = 0; i < BEZIER_COUNT; i++ ) {
        // float cpY = ( i % 2 == 0 ) ? maxY : minY;
        float diffY = 80; // random<int>(100, 400);
        float cpY = posY + (diffY * ((i % 2 == 0) ? 1 : -1));
        
        cpY = MAX(MIN_Y, cpY);
        cpY = MIN(MAX_Y, cpY);
        
        ccBezierConfig cfg;
        cfg.controlPoint_1 = Vec2(posX + RANGE_X*1, cpY);
        cfg.controlPoint_2 = Vec2(posX + RANGE_X*2, cpY);
        cfg.endPosition    = Vec2(posX + RANGE_X*3, posY);
        
        // 마지막 좌표 예외처리
        if( i == BEZIER_COUNT-1 ) {
            cfg.endPosition.x = endX;
        }
        
        auto bezier = BezierTo::create(BEZIER_DURATION, cfg);
        beziers.pushBack(bezier);
        
        posX = cfg.endPosition.x;
        // posY = cfg.endPosition.y;
        
        // DebugDraw: 베지어 곡선 좌표
        /*
        {
            string title[] = {
                "cp1",
                "cp2",
                "ep",
            };
            
            Vec2 pos[] = {
                cfg.controlPoint_1,
                cfg.controlPoint_2,
                cfg.endPosition,
            };
            
            Color4B colors[] = {
                Color4B::RED,
                Color4B::BLUE,
                Color4B::GREEN,
            };
         
            for( int j = 0; j < 3; ++j ) {
                auto n = LayerColor::create(colors[j]);
                n->setIgnoreAnchorPointForPosition(false);
                n->setAnchorPoint(ANCHOR_M);
                // n->setPosition(Vec2(pos[i].x, SB_WIN_SIZE.height*0.5f));
                n->setPosition(pos[j]);
                n->setContentSize(Size(20, 20));
                getScene()->addChild(n);
                
                auto label = Label::createWithTTF(title[j], FONT_RETRO, 30);
                label->setAnchorPoint(ANCHOR_MB);
                label->setPosition(Vec2TC(n->getContentSize(), 0, 10));
                label->setScale(n->getContentSize().height / label->getContentSize().height);
                n->addChild(label);
            }
        }
         */
    }
    
    auto firstDelay = DelayTime::create(firstDelayTime);
    auto move = Sequence::create(beziers);
    auto callbackDelay = DelayTime::create(1.0f);
    auto callFunc = CallFunc::create([=]() {
        // CCLOG("MOVE_OUT");
        this->move();
    });
    
    auto seq = Sequence::create(firstDelay, move, callbackDelay, callFunc, nullptr);
    runAction(seq);
}

/**
 * 이미지 초기화
 */
void GiftBox::initImage() {
    
    StringList files({
        DIR_IMG_GAME + "balloon_boost1.png", DIR_IMG_GAME + "balloon_boost2.png",
    });
    
    auto anim = SBNodeUtils::createAnimation(files, 0.5f);
    
    image = SBAnimationSprite::create(anim);
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(image->getContentSize(), 0, 0));
    addChild(image);
    
    setContentSize(image->getContentSize());
    
    image->runAnimation();
}

/**
 * 터치 이벤트 초기화
 */
void GiftBox::initTouch() {
 
    auto touchNode = SBNodeUtils::createTouchNode();
    touchNode->setContentSize(getContentSize());
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        
        if( onClickListener ) {
            onClickListener(this);
        }
    });
}
