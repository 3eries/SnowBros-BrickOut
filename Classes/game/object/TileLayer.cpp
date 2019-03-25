//
//  TileLayer.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 15/03/2019.
//

#include "TileLayer.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UserDefaultKey.h"
#include "User.hpp"
#include "UIHelper.hpp"
#include "TestHelper.hpp"

#include "Ball.hpp"

#include "tile/TileFactory.hpp"
#include "tile/Brick_10012.hpp"
#include "tile/NeutralBrick.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

TileLayer::TileLayer() :
onBrickAddedListener(nullptr),
onTileDownFinishedListener(nullptr),
onBrickBreakListener(nullptr) {
}

TileLayer::~TileLayer() {
    
    removeListeners(this);
}

bool TileLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    // FIXME: 고민
    // 클리핑 기능이 필요할 시 UILayout 상속을 고려
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);

    initGameListener();
    
    return true;
}

void TileLayer::onEnter() {
    
    Node::onEnter();
}

void TileLayer::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void TileLayer::onExit() {
    
    Node::onExit();
}

void TileLayer::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 게임 진입
 */
void TileLayer::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void TileLayer::onGameExit() {
}

/**
 * 게임 리셋
 */
void TileLayer::onGameReset() {
}

/**
 * 게임 시작
 */
void TileLayer::onGameStart() {
}

/**
 * 게임 재시작
 */
void TileLayer::onGameRestart() {
}

/**
 * 게임 오버
 */
void TileLayer::onGameOver() {
}

/**
 * 게임 이어하기
 */
void TileLayer::onGameContinue() {
}

/**
 * 스테이지 변경
 */
void TileLayer::onStageChanged(const StageData &stage) {
    
    // 랜덤 엔진 리셋
    resetRandomEngine();
    
    // 엘리트 드랍률 초기화
    eliteBrickDropRate = 0;
    isEliteDropped = false;
}

/**
 * 스테이지 클리어
 */
void TileLayer::onStageClear() {
}

/**
 * 층 변경
 */
void TileLayer::onFloorChanged(const FloorData &floor) {
    
    // 게임 오버 직전, 타일만 이동
    if( !isRowClear(1) ) {
        downTile();
        return;
    }
    
    // 데이터 있음
    if( !floor.isNull() ) {
        // 마지막 보스인 경우 연출 후 타일 추가
        if( floor.isExistBoss() && GameManager::getStage().getLastFloor().floor == floor.floor) {
            auto effectLayer = SBNodeUtils::createTouchNode();
            addChild(effectLayer, SBZOrder::BOTTOM);
            
            auto anim = spine::SkeletonAnimation::createWithJsonFile(ANIM_BOSS_WARNING);
            anim->setAnchorPoint(Vec2::ZERO);
            anim->setPosition(Vec2(SB_WIN_SIZE*0.5f));
            effectLayer->addChild(anim);
            
            auto track = anim->setAnimation(0, ANIM_NAME_RUN, false);
            anim->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
                
                this->initFloorTile(floor);
                
                anim->clearTracks();
                anim->removeFromParent();
                effectLayer->removeFromParent();
            });
        }
        // 연출 없이 타일 추가
        else {
            initFloorTile(floor);
        }
    }
    // 데이터 없음, 타일만 이동
    else {
        downTile();
    }
}

/**
 * 다음 층
 */
void TileLayer::onNextFloor(const FloorData &floor) {
}

/**
 * 층의 타일 초기화
 */
void TileLayer::initFloorTile(const FloorData &floor) {
    
    initFloorBrick(floor);
    initFloorItem(floor);
    
    // 타일 등장 연출 완료 후 이동
    SBDirector::postDelayed(this, [=]() {
        this->downTile();
    }, /*TILE_ENTER_DURATION + 0.1f*/0.3f);
}

/**
 * 층의 브릭 초기화
 */
void TileLayer::initFloorBrick(const FloorData &floor) {
    
    // 드랍 데이터 생성
    auto resetDropSpace = [=](FloorDropData &dropData) {
        dropData.bricks = getTiles<Brick*>(TILE_POSITION_MAX_Y);
        dropData.availablePositions = getEmptyPositions(TILE_POSITION_MAX_Y);
        dropData.availableTileCount = MIN((int)dropData.availablePositions.size(), dropData.availableTileCount);
    };
    
    FloorDropData dropData(floor);
    dropData.availableTileCount = floor.getRandomBrickDropCount();
    
    resetDropSpace(dropData);
    
    Log::i("TileLayer::initFloorBrick(%d-%d) availableTileCount: %d", floor.stage, GameManager::getFloorInStage(), dropData.availableTileCount);
    
    // 가용 칸수 체크
    if( dropData.isNoSpace() ) {
        return;
    }
    
    // 패턴
    if( !floor.pattern.isNull() ) {
        auto pattern = floor.pattern;
        
        for( auto patternBrick : pattern.bricks ) {
            patternBrick.tile.pos += TilePosition(0, TILE_POSITION_MAX_Y);
            addBrick(TileFactory::createBrick(patternBrick, floor));
        }
        
        // 파츠 설정
        if( pattern.isExistBrick("brick_10012") ) {
            auto bossBrick = (Brick_10012*)getBricks("brick_10012")[0];
            auto parts = bossBrick->getData().parts;
            vector<Brick*> partBricks;
            
            for( auto part : parts ) {
                SBCollection::addAll(partBricks, getBricks(part));
            }
            
            bossBrick->setParts(partBricks);
        }
        
        return;
    }
    
    // 중립 브릭
    if( floor.neutralBrickList.size() > 0 ) {
        auto patterns = TileFactory::createNeutralBrickPatterns(floor.neutralBrickList);
        auto pattern = patterns[arc4random() % patterns.size()];
        
        for( auto patternBrick : pattern.bricks ) {
            patternBrick.tile.pos += TilePosition(0, TILE_POSITION_MAX_Y);
            
            // 빈 공간일때만 브릭 추가
            if( !isExistTile(patternBrick.tile.pos,
                             patternBrick.brick.width, patternBrick.brick.height) ) {
                addBrick(TileFactory::createBrick(patternBrick, floor));
                
                dropData.availableTileCount -= patternBrick.brick.width;
            }
        }
        
        resetDropSpace(dropData);
    }
    
    if( dropData.isNoSpace() ) {
        return;
    }
    
    auto addBrickWithDropData = [=](Brick *brick, FloorDropData &dropData) -> Brick* {
        
        brick->setTilePosition(dropData.getAvailablePosition(brick->getData()));
        this->addBrick(brick);
        
        dropData.dropped(brick);
        
        return brick;
    };
    
    // 드랍 좌표 랜덤 설정
    dropData.randomPositions = dropData.availablePositions;
    shuffle(dropData.randomPositions.begin(), dropData.randomPositions.end(), randomEngine.brickPosition);
    
    // 특수 브릭, 중립 브릭 제외
    for( int i = 0; i < floor.specialBrickList.size(); ++i ) {
        auto brickData = floor.specialBrickList[i];
        
        if( brickData.type == BrickType::SPECIAL_NEUTRAL ) {
            continue;
        }
        
        if( dropData.checkSpace(brickData) ) {
            addBrickWithDropData(TileFactory::createSpecialBrick(brickData, TileData(), floor), dropData);
        }
    }
    
    // 엘리트 브릭
    auto eliteBrickData = floor.getRandomNormalBrick();
    
    if( dropData.checkSpace(eliteBrickData) ) {
        // 드랍률 업데이트
        updateEliteBrickDropRate(floor);
        
        Log::i("TileLayer::initFloorBrick(%d-%d) eliteBrickDropRate: %d", floor.stage, GameManager::getFloorInStage(), eliteBrickDropRate);
        
        uniform_int_distribution<int> dist(1,100);
        isEliteDropped = (dist(randomEngine.eliteBrickDrop) <= eliteBrickDropRate);
        
        if( isEliteDropped ) {
            addBrickWithDropData(TileFactory::createEliteBrick(eliteBrickData, TileData(), floor), dropData);
        }
    }
    
    // 일반 브릭
    if( !dropData.isNoSpace() ) {
        int len = dropData.availableTileCount;
        
        for( int i = 0; i < len; ++i ) {
            auto brickData = floor.getRandomNormalBrick();
            
            if( dropData.checkSpace(brickData) ) {
                addBrickWithDropData(TileFactory::createNormalBrick(brickData, TileData(), floor), dropData);
            }
        }
    }
}

/**
 * 층의 아이템 초기화
 */
void TileLayer::initFloorItem(const FloorData &floor) {
    
    // 비어있는 좌표 리스트 생성
    auto positions = getEmptyRandomPositions(TILE_POSITION_MAX_Y);
    if( positions.size() == 0 ) {
        return;
    }
    
    // 볼 증가 아이템
    auto stage = GameManager::getStage();
    
    int currentBallCount = (int)(ballCount + getItems().size());
    int finalBallCount = stage.finalBallCount;
    int ballDiff = finalBallCount - currentBallCount;
    int chance = stage.getLastFloor().floor - floor.floor; // 마지막층 전까지 기회있음
    
    if( chance < ballDiff ) {
        Log::w("볼 증가 아이템 드랍 오류").showMessageBox();
    }
    
    Log::i("TileLayer::initFloorItem ballDiff: %d, chance: %d", ballDiff, chance);
    
    if( ballDiff > 0 ) {
        bool isDrop = false;
        
        // 무조건 드랍
        if( chance == ballDiff ) {
            isDrop = true;
        }
        // 확률로 드랍, 고정 확률 58%
        else {
            // int len = (chance - ballDiff) + 1;
            // isDrop = (arc4random() % len) == 0;
            uniform_int_distribution<int> dist(1,100);
            isDrop = (dist(randomEngine.ballCountUp) <= 58);
        }
        
        if( isDrop ) {
            ItemData data(ItemType::POWER_UP);
            
            ItemDef def(data);
            def.floorData = floor;
            
            auto item = TileFactory::createItem(def);
            item->setTilePosition(positions[0]);
            addTile(item);
            
            positions.erase(positions.begin());
        }
    }
}

/**
 * 게임 리스너 초기화
 */
void TileLayer::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter               = CC_CALLBACK_0(TileLayer::onGameEnter, this);
    listener->onGameExit                = CC_CALLBACK_0(TileLayer::onGameExit, this);
    listener->onGameReset               = CC_CALLBACK_0(TileLayer::onGameReset, this);
    listener->onGameStart               = CC_CALLBACK_0(TileLayer::onGameStart, this);
    listener->onGameRestart             = CC_CALLBACK_0(TileLayer::onGameRestart, this);
    listener->onGameOver                = CC_CALLBACK_0(TileLayer::onGameOver, this);
    listener->onGameContinue            = CC_CALLBACK_0(TileLayer::onGameContinue, this);
    listener->onStageChanged            = CC_CALLBACK_1(TileLayer::onStageChanged, this);
    listener->onStageClear              = CC_CALLBACK_0(TileLayer::onStageClear, this);
    listener->onFloorChanged            = CC_CALLBACK_1(TileLayer::onFloorChanged, this);
    listener->onNextFloor               = CC_CALLBACK_1(TileLayer::onNextFloor, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

/**
 * 스테이지 클리어 체크
 */
bool TileLayer::checkStageClear() {
    
    auto bricks = getBricks();
    
    /*
    SBCollection::remove(bricks, [](Brick *brick) -> bool {
        return brick->getData().type == BrickType::SPECIAL_NEUTRAL;
    });
    */
     
    return bricks.size() == 0;
}

/**
 * 브릭 추가
 */
Brick* TileLayer::addBrick(Brick *brick) {
    
    CCASSERT(brick != nullptr, "TileLayer::addBrick error.");
    
    GAME_MANAGER->addBrick(brick->getData());
    
    brick->setOnBreakListener([=](Node*) {
        onBrickBreakListener(brick);
    });
    addTile(brick);
    
    return brick;
}

/**
 * 타일 추가
 */
Game::Tile* TileLayer::addTile(Game::Tile *tile) {
    
    if( getTile(tile->getTilePosition()) ) {
        Log::w("TileLayer::addTile warning: already exists.");
    }
    
    tile->enterWithAction();
    
    addChild(tile);
    tiles.push_back(tile);
    
    return tile;
}

/**
 * 타일 제거
 */
Game::Tile* TileLayer::removeTile(Game::Tile *tile) {
    
    auto it = std::find(tiles.begin(), tiles.end(), tile);
    if( it == tiles.end() ) {
        CCASSERT(false, "TileLayer::removeTile error.");
    }
    
    // remove from list
    tiles.erase(it);
    
    // remove tile
    tile->removeWithAction();
    
    return tile;
}

/**
 * 모든 타일을 한칸 아래로 이동 시킵니다
 */
void TileLayer::downTile() {
    
    // 마지막 칸의 중립 브릭은 자동 소멸
    auto neutralBricks = getTiles<NeutralBrick*>(1);
    
    for( auto brick : neutralBricks ) {
        auto fadeOut = FadeOut::create(TILE_MOVE_DURATION);
        auto callFunc = CallFunc::create([=]() {
            this->removeTile(brick);
        });
        
        brick->getImage()->runAction(Sequence::create(fadeOut, callFunc, nullptr));
    }
    
    // 타일 이동
    auto list = tiles;
    
    for( auto tile : list ) {
        tile->down();
    }
    
    // 콜백
    SBDirector::postDelayed(this, [=]() {
        
        if( onTileDownFinishedListener ) {
            onTileDownFinishedListener();
        }
    }, TILE_MOVE_DURATION + 0.1f);
}

/**
 * 엘리트 브릭 드랍률 업데이트
 */
void TileLayer::updateEliteBrickDropRate(const FloorData &floor) {
    
    if( isEliteDropped ) {
        isEliteDropped = false;
        eliteBrickDropRate = floor.eliteBrickDropRate;
    } else {
        if( eliteBrickDropRate == 0 ) {
            eliteBrickDropRate = floor.eliteBrickDropRate;
        } else {
            if( floor.eliteBrickDropRate == 0 ) {
                eliteBrickDropRate = 0;
            } else {
                eliteBrickDropRate *= 2;
            }
        }
    }
}

/**
 * 랜덤 엔진 리셋
 */
void TileLayer::resetRandomEngine() {
    
    mt19937 *engines[] {
        &randomEngine.brickPosition,
        &randomEngine.eliteBrickDrop,
        &randomEngine.ballCountUp,
    };
    
    const int SIMULATION_COUNT = 5000;
    
    for( int i = 0; i < sizeof(engines) / sizeof(mt19937*); ++i ) {
        auto engine = engines[i];
        
        random_device rd;
        (*engine) = mt19937(rd());
        
        // 균등한 난수 분포를 위해 시뮬레이션
//        double now = SBSystemUtils::getCurrentTimeSeconds();
//        map<int,int> numbers;
        
        for( int j = 0; j < SIMULATION_COUNT; ++j ) {
            uniform_int_distribution<int> dist(1,100);
            int n = dist(*engine);
            
//            if( numbers.find(n) == numbers.end() ) {
//                numbers[n] = 1;
//            } else {
//                numbers[n] = numbers[n]+1;
//            }
        }
        
//        CCLOG("simulation result time :%f", (SBSystemUtils::getCurrentTimeSeconds()-now));
//
//        // print numbers
//        for( auto it = numbers.begin(); it != numbers.end(); ++it ) {
//            CCLOG("number %d : %d (%f)", it->first, it->second, ((float)it->second / SIMULATION_COUNT) * 100.0f);
//        }
    }
}

/**
 * 타일을 반환합니다
 */
BrickList TileLayer::getBricks() {
    return getTiles<Brick*>();
}

BrickList TileLayer::getBricks(const string &brickId) {
    
    return SBCollection::find(getBricks(), [brickId](Brick *brick) -> bool {
        return brick->getData().brickId == brickId;
    });
}

ItemList TileLayer::getItems() {
    return getTiles<Item*>();
}

/**
 * 좌표에 해당하는 타일을 반환합니다
 */
Game::Tile* TileLayer::getTile(const TilePosition &pos) {
    
    for( auto tile : tiles ) {
        if( tile->isContainsPosition(pos) ) {
            return tile;
        }
    }
    
    return nullptr;
}

bool TileLayer::isExistTile(const TilePosition &pos, int rows, int columns) {
    
    for( int x = pos.x; x < pos.x + (rows-1); ++x ) {
        for( int y = pos.y; y < pos.y + (columns-1); ++y ) {
            if( getTile(TilePosition(x,y)) ) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * 행 클리어 여부를 반환합니다
 */
bool TileLayer::isRowClear(int y) {
    
    auto bricks = getTiles<Brick*>(y);
    
    SBCollection::remove(bricks, [](Brick *brick) -> bool {
        return brick->getData().type == BrickType::SPECIAL_NEUTRAL;
    });
    
    return bricks.size() == 0;
}

/**
 * y줄의 비어있는 좌표를 반환합니다
 */
TilePositions TileLayer::getEmptyPositions(int y) {
    
    const int TILE_ROWS = GAME_CONFIG->getTileRows();
    TilePositions positions;
    
    for( int x = 0; x < TILE_ROWS; ++x ) {
        TilePosition pos(x,y);
        
        if( !getTile(pos) ) {
            positions.push_back(pos);
        }
    }
    
    return positions;
}

TilePositions TileLayer::getEmptyRandomPositions(int y) {
    
    auto positions = getEmptyPositions(y);
    random_shuffle(positions.begin(), positions.end());
    
    return positions;
}

#pragma mark- FloorDropData

FloorDropData::FloorDropData(const FloorData &_floor) : floor(_floor) {
}

/**
 * 사용 가능한 공간의 유무를 반환합니다
 */
bool FloorDropData::isNoSpace() {
    return availableTileCount <= 0;
}

bool FloorDropData::checkSpace(const BrickData &data) {
    return availableTileCount >= data.width && getAvailablePosition(data) != INVALID_TILE_POSITION;
}

/**
 * 사용 가능한 좌표를 반환합니다
 */
TilePosition FloorDropData::getAvailablePosition(const BrickData &data) {
    
    if( availableTileCount < data.width || availablePositions.size() == 0 ) {
        return INVALID_TILE_POSITION;
    }
    
    // 1칸 크기는 랜덤 좌표
    if( data.width == 1 ) {
        return randomPositions[0];
    }
    
    // 2칸 크기부터는 연속된 좌표 체크
    TilePositions continuePositions;
    
    for( int i = 0; i <= GAME_CONFIG->getTileRows()-data.width; ++i ) {
        TilePosition pos(i, availablePositions[0].y);
        
        if( !isAvailablePosition(pos) ) {
            continue;
        }
        
        int continueCount = 1;
        
        for( int j = 1; j < data.width; ++j ) {
            if( !isAvailablePosition(TilePosition(pos.x+j, pos.y)) ) {
                break;
            }
            
            continueCount++;
        }
        
        if( continueCount == data.width ) {
            continuePositions.push_back(pos);
        }
    }
    
    if( continuePositions.size() == 0 ) {
        return INVALID_TILE_POSITION;
    }
    
    return continuePositions[arc4random() % continuePositions.size()];
}

bool FloorDropData::isAvailablePosition(const TilePosition &pos) {
    
    for( auto originPos : availablePositions ) {
        if( originPos.equals(pos) ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 해당 브릭의 드랍을 처리합니다
 */
void FloorDropData::dropped(Brick *brick) {
    
    bricks.push_back(brick);
    
    // 칸수 차감
    auto data = brick->getData();
    availableTileCount -= data.width;
    
    // 좌표 제거
    TilePosition beginPos = brick->getTilePosition();
    TilePosition endPos = beginPos + TilePosition(data.width-1, 0);
    
    auto func = [=](TilePosition originPos) -> bool {
        return originPos.x >= beginPos.x && originPos.x <= endPos.x &&
        originPos.y == beginPos.y;
    };
    
    SBCollection::remove(availablePositions, func);
    SBCollection::remove(randomPositions, func);
}
