//
//  AimController.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/12/2018.
//

#include "AimController.hpp"

#include "ui/CocosGUI.h"

#include "Define.h"
#include "../GameDefine.h"
#include "../GameManager.hpp"

#include "Ball.hpp"
#include "tile/Brick.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float LINE_PADDING         = 10;   // 조준선 간격
static const float MAX_ANGLE            = 80;   // 최대 각도
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

//void AimController::rayCast(const Vec2 &startPos, const Vec2 &endPos) {
//
//    // The ray extends from p1 to p1 + maxFraction * (p2 - p1).
//    b2RayCastInput input;
//    input.p1 = PTM(startPos);      // 시작점
//    input.p2 = PTM(endPos); // 종료점
//    input.maxFraction = 1;
//
//    //check every fixture of every body to find closest
//    float closestFraction = 1; //start with end of line as p2
//    b2Vec2 intersectionNormal(0,0);
//
//    b2RayCastOutput output;
//
//    auto fixture = shootingObj.ball->getBody()->GetFixtureList();
//    if( !fixture->RayCast(&output, input, 0) ) {
//        CCLOG("AimController RayCast error.");
//        // CCASSERT(false, "AimController RayCast error.");
//    }
//
//    if( output.fraction < closestFraction ) {
//        closestFraction = output.fraction;
//        intersectionNormal = output.normal;
//    }
//
//    CCLOG("output.fraction: %f", output.fraction);
//
//    /*
//     for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext()) {
//     for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {
//
//     b2RayCastOutput output;
//     if ( ! f->RayCast( &output, input ) )
//     continue;
//     if ( output.fraction < closestFraction ) {
//     closestFraction = output.fraction;
//     intersectionNormal = output.normal;
//     }
//     }
//     }
//     */
//
//    b2Vec2 intersectionPoint = input.p1 + closestFraction * (input.p2 - input.p1);
//    debugPoint->setPosition(MTP(intersectionPoint));
//
//    //draw this part of the ray
//    /*
//     glBegin(GL_LINES);
//     glVertex2f( p1.x, p1.y );
//     glVertex2f( intersectionPoint.x, intersectionPoint.y );
//     glEnd();
//
//     if ( closestFraction == 1 )
//     return; //ray hit nothing so we can finish here
//     if ( closestFraction == 0 )
//     return;
//
//     //still some ray left to reflect
//     b2Vec2 remainingRay = (p2 - intersectionPoint);
//     b2Vec2 projectedOntoNormal = b2Dot(remainingRay, intersectionNormal) * intersectionNormal;
//     b2Vec2 nextp2 = p2 - 2 * projectedOntoNormal;
//
//     //recurse
//     drawReflectedRay(intersectionPoint, nextp2);
//     */
//}

class RayCastCallback : public b2RayCastCallback {
public:
    RayCastCallback() {};
    ~RayCastCallback() {};
    
    float32 ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
        
        this->point = point;
        this->normal = normal;
        
        return fraction;
    }
    
    b2Vec2 point;
    b2Vec2 normal;
};

/**
 * 레이 캐스트
 */
void AimController::rayCast(const Vec2 &startPos, const Vec2 &endPos) {
    
    b2Vec2 p1 = PTM(startPos);  // 시작점
    b2Vec2 p2 = PTM(endPos);    // 종료점
    
    RayCastCallback callback;
    world->RayCast(&callback, p1, p2);
    
    // 충돌 지점 설정
    this->endPosition = MTP(callback.point);
    
    // 조준선 업데이트
    const float angle = SBMath::getDegree(startPos, this->endPosition);
    const float dist = startPos.getDistance(this->endPosition);
    
    shootingObj.updateLine(startPos, this->endPosition, angle, dist);
    
//    auto debugDraw = PhysicsManager::getDebugDrawView()->getDebugDraw();
//    debugDraw->DrawPoint(callback.point, 5.0f, b2Color(0.4f, 0.9f, 0.4f));
//    debugDraw->DrawSegment(p1, callback.point, b2Color(0.8f, 0.8f, 0.8f));
//    b2Vec2 head = callback.point + 0.5f * callback.normal;
//    debugDraw->DrawSegment(callback.point, head, b2Color(0.9f, 0.9f, 0.4f));
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
    
    // 슈팅 오브젝트
    {
        shootingObj.endMark->setVisible(true);
        shootingObj.line.setVisible(true);
        
        // Raycasting
        float shootingAngle = angle;
        shootingAngle = MIN(MAX_ANGLE, shootingAngle);
        shootingAngle = MAX(-MAX_ANGLE, shootingAngle);
        
        Vec2 virtualEndPosition = startPosition;
        virtualEndPosition += (Vec2::forAngle(-CC_DEGREES_TO_RADIANS(shootingAngle-90)) * MAP_DIAGONAL/*dist*/);
        
        rayCast(startPosition, virtualEndPosition);
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
 * 조준 활성화 여부 설정
 */
void AimController::setEnabled(bool isEnabled, vector<Game::Tile*> bricks) {
    
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
        // 충돌 체크용 벽돌 바디 생성
        for( auto brick : bricks ) {
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
        
        // Ball
        shootingObj.ballBody = Ball::createBody();
        
        // End Mark
        shootingObj.endMark = Sprite::create(BALL_IMAGE);
        shootingObj.endMark->setAnchorPoint(ANCHOR_M);
        shootingObj.endMark->setOpacity(255*0.75f);
        addChild(shootingObj.endMark);
        
        SBNodeUtils::scale(shootingObj.endMark, BALL_SIZE);
    }
    
    // 터치 기준 조준선
    touchAimLine = createAimLine();
    touchAimLine.line->setOpacity(255 * 0.5f);
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
        PhysicsCategory::WALL,
        PhysicsCategory::WALL,
        PhysicsCategory::WALL,
    };
    
    for( int i = 0; i < 3; ++i ) {
        b2Vec2 v1 = vectors[i][0];
        b2Vec2 v2 = vectors[i][1];
        
        b2EdgeShape shape;
        shape.Set(v1, v2);
        
        b2Filter filter;
        filter.categoryBits = categorys[i];
        filter.maskBits = PhysicsCategory::BALL;
        
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
    
    Size  LINE_SIZE = BALL_SIZE * 0.5f;
    
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
    lineLayer->setCascadeOpacityEnabled(true);
    lineLayer->setAnchorPoint(ANCHOR_MB);
    lineLayer->setContentSize(Size(LINE_SIZE.width, MAP_CONTENT_HEIGHT));
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
        auto line = Sprite::create(DIR_IMG_COMMON + "circle_white.png");
        line->setAnchorPoint(ANCHOR_MB);
        line->setPosition(Vec2BC(LINE_SIZE, 0, getPosY(i)));
        lineLayer->addChild(line);
        
        SBNodeUtils::scale(line, LINE_SIZE);
        
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

