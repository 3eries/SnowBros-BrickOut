//
//  SBPhysics.cpp
//
//  Created by seongmin hwang on 15/11/2018.
//

#include "SBPhysics.hpp"

USING_NS_CC;

#pragma mark- SBPhysics

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
