//
//  AimController.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 09/12/2018.
//

#ifndef AimController_hpp
#define AimController_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "tile/Brick.hpp"

class AimController : public cocos2d::Node {
public:
    CREATE_FUNC(AimController);
    ~AimController();
    
private:
    AimController();
    
    bool init() override;
    void initTouch();
    void initAimObject();
    void initCollisionWall();
    void initCollisionBrick();
    
    struct AimLine;
    AimLine createAimLine();
    
private:
    void shoot();
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
    class RayCastCallback : public b2RayCastCallback {
    public:
        RayCastCallback() {};
        ~RayCastCallback() {};
        
        float32 ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
            
            this->fixture = fixture;
            this->point = point;
            this->normal = normal;
            
            return fraction;
        }
        
        b2Fixture *fixture;
        b2Vec2 point;
        b2Vec2 normal;
    };
    
    void rayCast(RayCastCallback &callback,
                 const cocos2d::Vec2 &startPos, const cocos2d::Vec2 &endPos);
    
public:
    void setEnabled(bool isEnabled, BrickList bricks = BrickList());
    void setStartPosition(const cocos2d::Vec2 &startPosition);
    
private:
    CC_SYNTHESIZE(SBCallback, onAimingStartListener, OnAimingStartListener);
    CC_SYNTHESIZE(SBCallback, onAimingEndListener, OnAimingEndListener);
    CC_SYNTHESIZE(std::function<void(cocos2d::Vec2)>, onShootListener, OnShootListener);
    
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, startPosition, StartPosition); // 볼 출발 위치
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, endPosition, EndPosition);     // 볼 최종 위치
    
    bool isTouchCancelled;
    
    b2World *world;
    b2Body *wallBody;
    std::vector<b2Body*> brickBodies;
    
    float prevAngle;
    
private:
    /**
     * 조준선
     */
    struct AimLine {
        cocos2d::ClippingNode *clippingNode;
        cocos2d::Node         *stencil;
        cocos2d::Node         *line;
        
        AimLine() : clippingNode(nullptr), stencil(nullptr), line(nullptr) {}
        
        void setVisible(bool isVisible) {
            if( clippingNode )    clippingNode->setVisible(isVisible);
        };
        
        void updateLine(const cocos2d::Vec2 &start, const cocos2d::Vec2 &end,
                        float angle, float dist) {
            
            cocos2d::Node *nodes[] = { stencil, line, };
            
            for( auto n : nodes ) {
                if( n ) {
                    n->setPosition(start);
                    n->setRotation(angle);
                }
            }
            
            if( stencil ) stencil->setScaleY(dist / stencil->getContentSize().height);
        }
    };
    
    /**
     * 슈팅 오브젝트
     */
    struct ShootingObject {
        b2Body          *ballBody;   // 볼
        cocos2d::Node   *endMark;    // 최종 위치 마크
        AimLine          line;       // 조준선
        AimLine          secondLine; // 두번째 조준선, 벽을 조준할 경우에만 생성됨
        
        void setVisible(bool isVisible) {
            if( endMark )         endMark->setVisible(isVisible);
            line.setVisible(isVisible);
            secondLine.setVisible(isVisible);
        };
        
        void updateLine(const cocos2d::Vec2 &start, const cocos2d::Vec2 &end,
                        float angle, float dist) {
            
            if( endMark ) {
                endMark->setPosition(end);
                endMark->setRotation(angle);
            }
            line.updateLine(start, end, angle, dist);
        }
    };
    
    ShootingObject shootingObj; // 슈팅 오브젝트
    AimLine touchAimLine;       // 터치 기준 조준선
};

#endif /* AimController_hpp */
