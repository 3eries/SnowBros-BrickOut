//
//  TileFactory.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 27/02/2019.
//

#include "TileFactory.hpp"

#include "GameConfiguration.hpp"
#include "../../GameDefine.h"

#include "Brick_10012.hpp"
#include "NeutralBrick.hpp"
#include "ShieldBrick.hpp"
#include "GhostBrick.hpp"
#include "TeleportBrick.hpp"
#include "TornadoBrick.hpp"

USING_NS_CC;
using namespace std;

#pragma mark- Brick

Brick* TileFactory::createBrick(const BrickDef &def) {
    
    auto data = def.data;
    
    // by id
    switch( HASH_STR(data.brickId.c_str()) ) {
        case HASH_STR("brick_10012"):       return Brick_10012::create(def);
        default: break;
    }
    
    // by type
    switch( data.type ) {
        case BrickType::SPECIAL_SHIELD:     return ShieldBrick::create(def);
        case BrickType::SPECIAL_GHOST:      return GhostBrick::create(def);
        case BrickType::SPECIAL_TELEPORT:   return TeleportBrick::create(def);
        case BrickType::SPECIAL_TORNADO:    return TornadoBrick::create(def);
        case BrickType::SPECIAL_NEUTRAL:    return NeutralBrick::create(def);
        default: break;
    }
    
    // default
    return Brick::create(def);
}

Brick* TileFactory::createBrick(const PatternBrickData &patternData, const FloorData &floor) {
    
    return createBrick(BrickDef(patternData.brick, patternData.tile, patternData.hp, false, floor));
}

Brick* TileFactory::createNormalBrick(const BrickData &brickData, const TileData &tileData, const FloorData &floor) {
    
    return createBrick(BrickDef(brickData, tileData, floor.brickHp, false, floor));
}

Brick* TileFactory::createEliteBrick(const BrickData &brickData, const TileData &tileData, const FloorData &floor) {
    
    return createBrick(BrickDef(brickData, tileData, floor.brickHp*3, true, floor));
}

Brick* TileFactory::createSpecialBrick(const BrickData &brickData, const TileData &tileData, const FloorData &floor) {
    
    return createBrick(BrickDef(brickData, tileData, floor.brickHp*0.8f, false, floor));
}

/**
 * 중립 브릭 리스트의 순열을 구하여 패턴을 만듭니다
 */
PatternDataList TileFactory::createNeutralBrickPatterns(BrickDataList neutralBricks) {
    
    if( neutralBricks.size() == 0 ) {
        return PatternDataList();
    }
    
    double t = SBSystemUtils::getCurrentTimeSeconds();
    
    auto patternCompare = [](const PatternBrickData &p1, const PatternBrickData &p2) -> bool {
        if( p1.brick.isNull() && p2.brick.isNull() ) {
            return false;
        }
        
        return p1.tile.pos.x < p2.tile.pos.x;
    };

    // 패턴 순열
    PatternDataList patterns;
    PatternData patternPermutation;
    
    for( int i = 0; i < GAME_CONFIG->getTileRows(); ++i ) {
        PatternBrickData patternBrick;
        patternBrick.brick = i < neutralBricks.size() ? neutralBricks[i] : BrickData();
        patternBrick.tile.pos.x = i;
        patternBrick.tile.pos.y = 0;
        
        patternPermutation.bricks.push_back(patternBrick);
    }
    
    do {
        PatternData resultPattern;
        
        for( int i = 0; i < GAME_CONFIG->getTileRows(); ++i ) {
            auto patternBrick = patternPermutation.bricks[i];
            
            if( !patternBrick.brick.isNull() ) {
                patternBrick.tile.pos.x = i;
                resultPattern.bricks.push_back(patternBrick);
            }
        }
        
        string resultPatternStr = resultPattern.getBrickListString();
        // CCLOG("%s", resultPatternStr.c_str());
        
        // 좌표 유효성 체크
        bool isValidPattern = true;
        
        const int PADDING_X = 1;
        int x = resultPattern.bricks[0].tile.pos.x;
        
        for( int i = 0; i < resultPattern.bricks.size(); ++i ) {
            auto patternBrick = resultPattern.bricks[i];
            int min = patternBrick.tile.pos.x;
            int max = min + (patternBrick.brick.width-1);
            
            // 유효
            if( min >= x && max < GAME_CONFIG->getTileRows() ) {
                x = max;
                x += PADDING_X+1;                
            }
            // 무효
            else {
                isValidPattern = false;
                break;
            }
        }
        
        // 유효한 패턴 등록
        if( isValidPattern ) {
            // 중복 방지
            bool found = false;
            
            for( auto pattern : patterns ) {
                if( pattern.getBrickListString() == resultPatternStr ) {
                    found = true;
                    break;
                }
            }
            
            if( !found ) {
                patterns.push_back(resultPattern);
            }
        }
        
    } while( next_permutation(patternPermutation.bricks.begin(), patternPermutation.bricks.end(), patternCompare) );
    
    // 최종 패턴 검사
    CCLOG("result (patterns len: %d, dt: %f) ========>",
          (int)patterns.size(), SBSystemUtils::getCurrentTimeSeconds() - t);
    
    for( auto pattern : patterns ) {
        CCLOG("%s", pattern.getBrickListString().c_str());
    }
    
    return patterns;
}

#pragma mark- Item

Item* TileFactory::createItem(const ItemDef &def) {
    
    return Item::create(def);
}
