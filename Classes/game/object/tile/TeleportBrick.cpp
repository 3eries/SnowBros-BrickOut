//
//  TeleportBrick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/04/2019.
//

#include "TeleportBrick.hpp"

#include "Define.h"
#include "../../GameManager.hpp"

#include "../TileLayer.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define ANIM_INTERVAL       0.06f

TeleportBrick* TeleportBrick::create(const BrickDef &def) {
    
    auto brick = new TeleportBrick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

TeleportBrick::TeleportBrick(const BrickDef &def) : SpecialBrick(def) {
}

TeleportBrick::~TeleportBrick() {
    
}

bool TeleportBrick::init() {
    
    if( !SpecialBrick::init() ) {
        return false;
    }
    
    return true;
}

void TeleportBrick::onEnter() {
    
    SpecialBrick::onEnter();
    
    teleportTargetPos = INVALID_TILE_POSITION;
    
//    scheduleOnce([=](float dt) {
//        this->teleport();
//    }, 0.05f, "FIRST_TELEPORT_DELAY")
}

/**
 * 층 변경
 */
void TeleportBrick::onFloorChanged(const FloorData &floor) {
    
    SpecialBrick::onFloorChanged(floor);
}

void TeleportBrick::onNextFloor(const FloorData &floor) {
    
    SpecialBrick::onNextFloor(floor);
    
    teleportTargetPos = INVALID_TILE_POSITION;
}

void TeleportBrick::updateSpecialState() {
    
    SpecialBrick::updateSpecialState();
    
    // 텔레포트 타겟 좌표 설정
    auto positions = getTeleportTargetPositions();
    
    if( positions.size() == 0 ) {
        // 이동할 좌표 없음
        return;
    }
    
    teleportTargetPos = positions[arc4random() % positions.size()];
    
    // 연출
    auto originFlyAnimFiles = ContentResourceHelper::getBrickAnimationFiles(data.image, "fly");
    auto originMoveAnimFiles = ContentResourceHelper::getBrickAnimationFiles(data.image, "move");
    
    // Step 1. fly
    StringList flyAnimFiles({
        originFlyAnimFiles[0], originFlyAnimFiles[1],
        originFlyAnimFiles[0], originFlyAnimFiles[1],
        originFlyAnimFiles[0], originFlyAnimFiles[1],
        originFlyAnimFiles[0], originFlyAnimFiles[1],
        originFlyAnimFiles[0],
    });
    
    auto flyAnimation = SBNodeUtils::createAnimation(flyAnimFiles, ANIM_INTERVAL);
    image->setAnimation(flyAnimation, 1);
    image->runAnimation([=](Node*) {
        
        // Step 2. move
        this->setLocalZOrder(data.zOrder);
        this->setBgVisible(false);
        this->setHpVisible(false);
        
        // move frame animation
        StringList moveAnimFiles({
            originMoveAnimFiles[0], originMoveAnimFiles[1],
            originMoveAnimFiles[0], originMoveAnimFiles[1],
            originMoveAnimFiles[0], originMoveAnimFiles[1],
        });
        
        auto moveAnimation = SBNodeUtils::createAnimation(moveAnimFiles, ANIM_INTERVAL);
        image->setAnimation(moveAnimation, 1);
        image->runAnimation([=](Node*) {
            
            // Step 3. idle
            this->setImage(BrickImageType::IDLE, true);
            
            this->setLocalZOrder(0);
            this->setBgVisible(true);
            this->setHpVisible(true);
        });
        
        // move action
        float duration = moveAnimFiles.size() * ANIM_INTERVAL;
        
        this->setImageFlippedX(teleportTargetPos.x - this->getTilePosition().x < 0);
        this->moveWithAction(teleportTargetPos, duration, [=]() {
            this->setImageFlippedX(false);
        });
    });
}

/**
 * 현재 위치한 Y열의 좌표 중 이동 가능한 좌표를 반환합니다
 */
TilePositions TeleportBrick::getTeleportTargetPositions() {
    
    const float POS_Y = getTilePosition().y;
    TilePositions positions;
    
    for( int x = 0; x < GAME_CONFIG->getTileRows(); ++x ) {
        positions.push_back(TilePosition(x, POS_Y));
    }
    
    // 사용할 수 없는 좌표 제거
    auto teleportBricks = tileLayer->getTiles<TeleportBrick*>(POS_Y);
    
    SBCollection::remove(positions, [=](TilePosition pos) -> bool {
        
        // 내 좌표
        if( pos == this->getTilePosition() ) {
            return true;
        }
        
        // 다른 텔레포트 브릭의 타겟 좌표
        for( auto teleportBrick : teleportBricks ) {
            if( teleportBrick != this &&
                teleportBrick->getTeleportTargetPosition() == pos ) {
                return true;
            }
        }
        
        // 다른 오브젝트
        auto tile = tileLayer->getTile(pos);
        auto brick = dynamic_cast<Brick*>(tile);
        
        if( tile && !brick ) {
            return true;
        }
        
        // 다른 브릭
        if( brick && brick->getData().type != BrickType::SPECIAL_TELEPORT ) {
            return true;
        }
        
        return false;
    });
    
    return positions;
}


/**
 * 등장 시간을 반환합니다
 */
float TeleportBrick::getEnterDuration() {
    
    return (int)getTilePosition().y == TILE_POSITION_MAX_Y ? GAME_CONFIG->getTileEnterDuration() : data.enterDuration;
}
