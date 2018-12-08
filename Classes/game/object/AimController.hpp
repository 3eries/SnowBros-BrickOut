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

#include "Tile.hpp"
#include "Ball.hpp"

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
    
private:
    void shoot();
    void rayCast(const cocos2d::Vec2 &startPos, const cocos2d::Vec2 &endPos);
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
public:
    void setEnabled(bool isEnabled,
                    std::vector<Game::Tile*> bricks = std::vector<Game::Tile*>());
    void setStartPosition(const cocos2d::Vec2 &startPosition);
    
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
    
    AimLine createAimLine();
    
    /**
     * 슈팅 오브젝트
     */
    struct ShootingObject {
        Ball            *ball;      // 볼
        cocos2d::Node   *endMark;   // 최종 위치 마크
        AimLine          line;      // 조준선
        
        void setVisible(bool isVisible) {
            if( ball )            ball->setVisible(isVisible);
            if( endMark )         endMark->setVisible(isVisible);
            line.setVisible(isVisible);
        };
        
        void updateLine(const cocos2d::Vec2 &start, const cocos2d::Vec2 &end,
                        float angle, float dist) {
            
            if( endMark )         endMark->setPosition(end);
            line.updateLine(start, end, angle, dist);
        }
    };
    
private:
    CC_SYNTHESIZE(std::function<void(cocos2d::Vec2)>, onShootListener, OnShootListener);
    
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, startPosition, StartPosition); // 볼 출발 위치
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, endPosition, EndPosition);     // 볼 최종 위치
    
    bool isTouchCancelled;
    
    b2World *world;
    b2Body *wallBody;
    std::vector<b2Body*> brickBodies;
    
    ShootingObject shootingObj; // 슈팅 오브젝트
    AimLine touchAimLine;       // 터치 기준 조준선
};

#endif /* AimController_hpp */
