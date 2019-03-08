//
//  SBPhysics.cpp
//
//  Created by seongmin hwang on 15/11/2018.
//

#include "SBPhysics.hpp"

USING_NS_CC;
using namespace std;

#pragma mark- SBPhysics

/**
 * 물리 오브젝트 루프
 */
void SBPhysics::loopBodies(b2World *world, function<void(b2Body*)> callback) {
    
    if( !world ) {
        return;
    }
    
    // GetBodyList()를 이용한 루프 진행 중 바디가 제거되어,
    // dangling pointer가 발생할 수 있으므로 별도의 리스트 생성
    vector<b2Body*> bodies;
    
    for( auto b = world->GetBodyList(); b; b = b->GetNext() ) {
        bodies.push_back(b);
    }
    
    for( auto b : bodies ) {
        callback(b);
    }
}

void SBPhysics::loopUserDatas(b2World *world, function<void(void*)> callback) {
    
    loopBodies(world, [=](b2Body *b) {
        auto userData = b->GetUserData();
        
        if( userData ) {
            callback(userData);
        }
    });
}

void SBPhysics::loopObjects(b2World *world, function<void(SBPhysicsObject*)> callback) {
    
    loopUserDatas(world, [=](void *userData) {
        callback((SBPhysicsObject*)userData);
    });
}

/**
 * World의 모든 바디 제거
 */
void SBPhysics::removeBodies(b2World *world) {

    if( world ) {
        SBPhysics::loopBodies(world, [=](b2Body *body) {
            world->DestroyBody(body);
        });
    }
}

void SBPhysics::syncNodeToBody(Node *node, b2Body *body) {
    
    float radians = CC_DEGREES_TO_RADIANS(node->getRotation());
    body->SetTransform(PTM(node->getPosition()), radians);
}

void SBPhysics::syncBodyToNode(b2Body *body, cocos2d::Node *node) {
    
    node->setPosition(Vec2(MTP(body->GetPosition().x), MTP(body->GetPosition().y)));
    node->setRotation(CC_RADIANS_TO_DEGREES(body->GetAngle()));
}

Vec2 SBPhysics::getContactPoint(b2Contact *contact) {
 
    CCASSERT(contact != nullptr, "SBPhysics::getContactPoint error.");
    
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    
    return MTP(worldManifold.points[0]);
}

SBPhysics::CollisionObjects SBPhysics::findCollisionObjects(uint16 categoryBits1, uint16 categoryBits2,
                                                            b2Fixture *fixtureA, b2Fixture *fixtureB) {
    
    if( !fixtureA->GetBody()->GetUserData() ||
        !fixtureB->GetBody()->GetUserData() ) {
        return CollisionObjects();
    }
    
    auto objA = (SBPhysicsObject*)fixtureA->GetBody()->GetUserData();
    auto objB = (SBPhysicsObject*)fixtureB->GetBody()->GetUserData();
    
    if( objA->isCollisionLocked() || objB->isCollisionLocked() ) {
        return CollisionObjects();
    }
    
    CollisionObjects objs;
    
    if( fixtureA->GetFilterData().categoryBits == categoryBits1 &&
        fixtureB->GetFilterData().categoryBits == categoryBits2 ) {
        objs.obj1 = objA;
        objs.obj2 = objB;
    }
    else if( fixtureB->GetFilterData().categoryBits == categoryBits1 &&
             fixtureA->GetFilterData().categoryBits == categoryBits2 ) {
        objs.obj1 = objB;
        objs.obj2 = objA;
    }
    
    return objs;
}

SBPhysics::CollisionObjects SBPhysics::findCollisionObjects(uint16 categoryBits1, uint16 categoryBits2,
                                                            b2Contact *contact) {
    
    return findCollisionObjects(categoryBits1, categoryBits2, contact->GetFixtureA(), contact->GetFixtureB());
}

#pragma mark- SBPhysicsObject

SBPhysicsObject::SBPhysicsObject(Node *node) :
node(node),
body(nullptr),
collisionLocked(false),
syncLocked(false),
needRemove(false) {
    
}

SBPhysicsObject::~SBPhysicsObject() {
    
    removeBody();
}

void SBPhysicsObject::initPhysics() {
    
}

Size SBPhysicsObject::getPhysicsSize() {
    
    if( node ) {
        return node->getContentSize();
    }
    
    return Size::ZERO;
}

void SBPhysicsObject::removeBody() {
    
    if( body ) {
        body->GetWorld()->DestroyBody(body);
        body = nullptr;
    }
}

bool SBPhysicsObject::beforeStep() {
    return true;
}

bool SBPhysicsObject::afterStep() {
    
    if( needRemove ) {
        removeBody();
        node->removeFromParent();
        
        return false;
    }
    
    return true;
}

void SBPhysicsObject::setNeedRemove(bool isNeedRemove) {
    
    this->needRemove = isNeedRemove;
    
    if( isNeedRemove ) {
        sleep(false);
        setCollisionLocked(true);
    }
}

void SBPhysicsObject::setActive(bool isActive, bool updateVisible) {
    
    if( body ) {
        body->SetActive(isActive);
    }
    
    if( node && updateVisible ) {
        node->setVisible(isActive);
    }
}

void SBPhysicsObject::setAwake(bool isAwake, bool updateVisible) {

    if( body ) {
        body->SetAwake(isAwake);
    }

    if( node && updateVisible ) {
        node->setVisible(isAwake);
    }
}


void SBPhysicsObject::awake(bool updateVisible) {

    setAwake(true, updateVisible);
}

void SBPhysicsObject::sleep(bool updateVisible) {

    setAwake(false, updateVisible);
}

bool SBPhysicsObject::isActive() {
    return body && body->IsActive();
}

bool SBPhysicsObject::isAwake() {
    return body && body->IsAwake();
}

void SBPhysicsObject::setBodyPosition(const Vec2 &p) {

    if( body ) {
        body->SetTransform(PTM(p), body->GetAngle());
    }
}

void SBPhysicsObject::syncNodeToBody() {
    
    if( node && body && !syncLocked ) {
        SBPhysics::syncNodeToBody(node, body);
    }
}

void SBPhysicsObject::syncBodyToNode() {
    
    if( node && body && !syncLocked ) {
        SBPhysics::syncBodyToNode(body, node);
    }
}
