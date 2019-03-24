//
//  AimController.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/12/2018.
//

#include "AimController.hpp"

#include "ui/CocosGUI.h"

#include "Define.h"
#include "ContentResourceHelper.hpp"
#include "../GameDefine.h"
#include "../GameManager.hpp"

#include "Ball.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float LINE_PADDING         = 25;   // 조준선 간격
static const Color4B DEBUG_AIM_LINE_COLOR               = Color4B(0,0,255,255*0.2f);

AimController::AimController() :
onShootListener(nullptr),
startPosition(FIRST_SHOOTING_POSITION),
world(GameManager::getPhysicsManager()->getWorld()) {
    
}

AimController::~AimController() {
    
}

bool AimController::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initTouch();
    initAimObject();
    initCollisionWall();
    
    shootingObj.setVisible(false);
    touchAimLine.setVisible(false);
    setEnabled(false);
    
    return true;
}

void AimController::shoot() {
    
    if( endPosition == Vec2::ZERO ) {
        return;
    }
    
    setEnabled(false);
    
    if( onShootListener ) {
        onShootListener(endPosition);
    }
}

/**
 * 터치 시작
 */
bool AimController::onTouchBegan(Touch *touch, Event*) {
    
    if( !SBNodeUtils::hasVisibleParents(this) ) {
        return false;
    }
    
    if( !MAP_BOUNDING_BOX.containsPoint(touch->getLocation()) ) {
        return false;
    }
    
    if( superbomb::isMultiTouch(touch) ) {
        return true;
    }
    
    initCollisionBrick();
    
    isTouchCancelled = false;
    endPosition = Vec2::ZERO;
    prevAngle = 0;
    
    return true;
}

/**
 * 터치 이동
 */
void AimController::onTouchMoved(Touch *touch, Event *event) {
    
    if( superbomb::isMultiTouch(touch) ) {
        return;
    }
    
    if( isTouchCancelled ) {
        return;
    }
    
    if( !MAP_BOUNDING_BOX.containsPoint(touch->getLocation()) ) {
        onTouchCancelled(touch, event);
        return;
    }
    
    Vec2 start = touch->getStartLocation();
    Vec2 end   = touch->getLocation();
    
    const float angle = SBMath::getDegree(start, end);
    const float dist = start.getDistance(end);

    if( dist < 10 ) {
        return;
    }
    
    /*
    const float angleDiff = fabsf(angle - prevAngle);
    prevAngle = angle;
    
    if( angleDiff < 0.08f ) {
        CCLOG("ignore >>>>>>>>>> angle");
        return;
    }
    
    CCLOG("angle: %f diff: %f", angle, angleDiff);
    */
     
    // 슈팅 오브젝트
    {
        shootingObj.endMark->setVisible(true);
        shootingObj.line.setVisible(true);
        
        // Raycasting
        float shootingAngle = angle;
        shootingAngle = MIN(SHOOTING_MAX_ANGLE, shootingAngle);
        shootingAngle = MAX(-SHOOTING_MAX_ANGLE, shootingAngle);
        
        Vec2 virtualEndPosition = SBMath::getEndPosition(startPosition, shootingAngle, MAP_DIAGONAL);
        
        RayCastCallback callback;
        rayCast(callback, startPosition, virtualEndPosition);
    }
    
    // 터치 기준 조준선 업데이트
    {
        touchAimLine.setVisible(true);
        touchAimLine.updateLine(start, end, angle, dist);
    }
}

/**
 * 터치 종료
 */
void AimController::onTouchEnded(Touch *touch, Event*) {
    
    if( superbomb::isMultiTouch(touch) ) {
        return;
    }
    
    shootingObj.setVisible(false);
    touchAimLine.setVisible(false);
    
    if( !isTouchCancelled ) {
        shoot();
    }
}

/**
 * 터치 취소
 */
void AimController::onTouchCancelled(Touch *touch, Event *event) {
    
    isTouchCancelled = true;
    onTouchEnded(touch, event);
}

/**
 * 레이 캐스트
 */
void AimController::rayCast(RayCastCallback &callback,
                            const Vec2 &startPos, const Vec2 &endPos) {
    
    b2Vec2 p1 = PTM(startPos); // 시작점
    b2Vec2 p2 = PTM(endPos);   // 종료점
    
    world->RayCast(&callback, p1, p2);
    
    // 충돌 지점 설정
    this->endPosition = MTP(callback.point);
    
    // 조준선 업데이트
    const float angle = SBMath::getDegree(startPos, this->endPosition);
    const float dist = startPos.getDistance(this->endPosition);
    
    shootingObj.updateLine(startPos, this->endPosition, angle, dist);
    
    // 벽 조준 시 두번째 조준선 노출
    auto bits = callback.fixture->GetFilterData().categoryBits;
    
    if( bits == PhysicsCategory::WALL_LEFT || bits == PhysicsCategory::WALL_RIGHT ||
        bits == PhysicsCategory::WALL_TOP ) {
        Vec2  secondStartPos = this->endPosition;
        float secondDist = 300;
        float secondAngle = (bits != PhysicsCategory::WALL_TOP) ? -angle : 180-angle;
        
        auto secondEndPos = SBMath::getEndPosition(secondStartPos, secondAngle, secondDist);
        shootingObj.secondLine.setVisible(true);
        shootingObj.secondLine.updateLine(secondStartPos, secondEndPos, secondAngle, secondDist);
        
    } else {
        shootingObj.secondLine.setVisible(false);
    }
    
//    auto debugDraw = PhysicsManager::getDebugDrawView()->getDebugDraw();
//    debugDraw->DrawPoint(callback.point, 5.0f, b2Color(0.4f, 0.9f, 0.4f));
//    debugDraw->DrawSegment(p1, callback.point, b2Color(0.8f, 0.8f, 0.8f));
//    b2Vec2 head = callback.point + 0.5f * callback.normal;
//    debugDraw->DrawSegment(callback.point, head, b2Color(0.9f, 0.9f, 0.4f));
}

/**
 * 조준 활성화 여부 설정
 */
void AimController::setEnabled(bool isEnabled, BrickList bricks) {
    
    setVisible(isEnabled);
    
    // update body
    wallBody->SetActive(isEnabled);
    shootingObj.ballBody->SetActive(isEnabled);
    
    // update bricks
    auto removeBodies = [=]() {
        if( brickBodies.size() == 0 ) {
            return;
        }
        
        for( auto body : brickBodies ) {
            world->DestroyBody(body);
        }
        
        brickBodies.clear();
    };
    
    removeBodies();
    
    if( isEnabled ) {
        // 충돌 체크용 브릭 바디 생성
        for( auto brick : bricks ) {
            if( !brick->isBodyActive() ) {
                continue;
            }
            
            Size size(brick->getContentSize());
            size.width += BALL_SIZE.width;
            size.height += BALL_SIZE.height;
            
            b2BodyDef bodyDef;
            
            b2PolygonShape box;
            box.SetAsBox(PTM(size.width * 0.5f), PTM(size.height * 0.5f));
            
            auto body = world->CreateBody(&bodyDef);
            SBPhysics::syncNodeToBody(brick, body);
            brickBodies.push_back(body);
            
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
        }
        
        // 슈팅 볼 좌표 설정
        shootingObj.ballBody->SetTransform(PTM(startPosition), shootingObj.ballBody->GetAngle());
    }
}

/**
 * 발사 시작 좌표 설정
 */
void AimController::setStartPosition(const Vec2 &startPosition) {
    
    this->startPosition = startPosition;
}

/**
 * 터치 이벤트 초기화
 */
void AimController::initTouch() {
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(AimController::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(AimController::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(AimController::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(AimController::onTouchCancelled, this);
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/**
 * 조준 오브젝트 초기화
 */
void AimController::initAimObject() {
    
    // 슈팅 오브젝트
    {
        // Line
        shootingObj.line = createAimLine();
        shootingObj.line.line->setOpacity(255 * 0.4f);
        
        shootingObj.secondLine = createAimLine();
        shootingObj.secondLine.line->setOpacity(255*0.4f);
        shootingObj.secondLine.setVisible(false);
        
        // Ball
        shootingObj.ballBody = Ball::createBody();
        
        // End Mark
        shootingObj.endMark = Node::create();
        shootingObj.endMark->setAnchorPoint(ANCHOR_M);
        shootingObj.endMark->setContentSize(BALL_SIZE);
        shootingObj.endMark->setCascadeOpacityEnabled(true);
        shootingObj.endMark->setOpacity(255*0.75f);
        addChild(shootingObj.endMark);
        
        {
            auto img = Sprite::create(SELECTED_BALL_IMAGE);
            img->setAnchorPoint(ANCHOR_M);
            img->setPosition(Vec2MC(BALL_SIZE, 0, 0));
            shootingObj.endMark->addChild(img);
        }
    }
    
    // 터치 기준 조준선
    touchAimLine = createAimLine();
    touchAimLine.line->setOpacity(255 * 0.4f);
}

/**
 * 충돌 체크용 벽 초기화
 */
void AimController::initCollisionWall() {
    
    b2BodyDef bodyDef;
    bodyDef.position = PTM(MAP_POSITION);
    bodyDef.active = false;
    
    auto body = world->CreateBody(&bodyDef);
    this->wallBody = body;
    
    float left   = PTM(-MAP_CONTENT_SIZE.width*0.5f + BALL_RADIUS);
    float right  = PTM( MAP_CONTENT_SIZE.width*0.5f - BALL_RADIUS);
    float bottom = PTM(-MAP_CONTENT_SIZE.height*0.5f + BALL_RADIUS);
    float top    = PTM( MAP_CONTENT_SIZE.height*0.5f - BALL_RADIUS);
    
    b2Vec2 vectors[3][2] = {
        { b2Vec2(left, bottom), b2Vec2(left, top) },          // left
        { b2Vec2(right, bottom), b2Vec2(right, top) },        // right
        { b2Vec2(left, top), b2Vec2(right, top) },            // top
    };
    
    PhysicsCategory categorys[] = {
        PhysicsCategory::WALL_LEFT,
        PhysicsCategory::WALL_RIGHT,
        PhysicsCategory::WALL_TOP,
    };
    
    for( int i = 0; i < 3; ++i ) {
        b2Vec2 v1 = vectors[i][0];
        b2Vec2 v2 = vectors[i][1];
        
        b2EdgeShape shape;
        shape.Set(v1, v2);
        
        b2Filter filter;
        filter.categoryBits = categorys[i];
        filter.maskBits = PHYSICS_MASK_BITS_WALL;
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 0.1f;
        fixtureDef.restitution = 1;
        fixtureDef.friction = 0;
        fixtureDef.filter = filter;
        body->CreateFixture(&fixtureDef);
    }
}

/**
 * 충돌 체크용 벽돌 초기화
 */
void AimController::initCollisionBrick() {
    
}

AimController::AimLine AimController::createAimLine() {
    
    Size  LINE_SIZE = BALL_SIZE * 0.65f;
    
    AimLine aimLine;
    
    // ClippingNode 생성
    auto stencil = LayerColor::create(Color4B::RED);
    stencil->setIgnoreAnchorPointForPosition(false);
    stencil->setAnchorPoint(ANCHOR_MB);
    stencil->setContentSize(Size(LINE_SIZE.width, SB_WIN_SIZE.height));
    
    aimLine.stencil = stencil;
    
    auto clippingNode = ClippingNode::create(stencil);
    addChild(clippingNode);
    
    aimLine.clippingNode = clippingNode;
    
    // 영역 확인용
    /*
    {
        auto n = LayerColor::create(DEBUG_AIM_LINE_COLOR);
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(Vec2::ZERO);
        n->setPosition(Vec2::ZERO);
        n->setContentSize(SB_WIN_SIZE);
        clippingNode->addChild(n);
    }
    */
     
    // LineLayer 생성
    float LINE_DIST = MAP_DIAGONAL;
    int   LINE_COUNT = LINE_DIST / (LINE_SIZE.height + LINE_PADDING);
    
    auto lineLayer = Node::create();
    lineLayer->setAnchorPoint(ANCHOR_MB);
    lineLayer->setContentSize(Size(LINE_SIZE.width, MAP_CONTENT_HEIGHT));
    lineLayer->setCascadeOpacityEnabled(true);
    clippingNode->addChild(lineLayer);
    
    aimLine.line = lineLayer;
    
    // 개별 Line 생성
    auto getPosY = [=](int i) {
        return i * (LINE_SIZE.height + LINE_PADDING);
    };
    
    float MIN_Y = 0;
    float MAX_Y = getPosY(LINE_COUNT-1);
    float ACTION_DURATION = 10.0f;
    
    int cnt = 0;
    
    for( int i = 0; i < LINE_COUNT /*posY <= MAX_Y*/; ++i ) {
        auto line = Node::create();
        line->setAnchorPoint(ANCHOR_MB);
        line->setPosition(Vec2BC(LINE_SIZE, 0, getPosY(i)));
        line->setContentSize(LINE_SIZE);
        line->setCascadeOpacityEnabled(true);
        lineLayer->addChild(line);
        
        {
            auto img = Sprite::create(SELECTED_BALL_IMAGE);
            img->setAnchorPoint(ANCHOR_M);
            img->setPosition(Vec2MC(LINE_SIZE, 0, 0));
            img->setScale(LINE_SIZE.height / BALL_SIZE.height);
            line->addChild(img);
        }
        
        // action
        float diff = MAX_Y - line->getPositionY();
        float duration = ACTION_DURATION * (diff / MAX_Y);
        
        auto moveOut = MoveTo::create(duration, Vec2(line->getPositionX(), MAX_Y));
        auto callFunc = CallFunc::create([=]() {
            
            auto moveStart = MoveTo::create(0, Vec2(line->getPositionX(), MIN_Y));
            auto moveEnd = MoveTo::create(ACTION_DURATION, Vec2(line->getPositionX(), MAX_Y));
            line->runAction(RepeatForever::create(Sequence::create(moveStart, moveEnd, nullptr)));
        });
        line->runAction(Sequence::create(moveOut, callFunc, nullptr));
        
        cnt++;
    }
    
    CCLOG("line cnt: %d", cnt);
    
    return aimLine;
}

