//
//  FriendsLayer.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 30/01/2019.
//

#include "FriendsLayer.hpp"

#include "tile/Item.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define        SLOT_COUNT          5
#define        FRIENDS_POS_Y       (SB_WIN_SIZE.height*0.5f)

FriendsLayer::FriendsLayer() {
    
}

FriendsLayer::~FriendsLayer() {
    
}

bool FriendsLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initFriends();
    initGameListener();
    
    return true;
}

void FriendsLayer::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 볼을 기준으로 좌표 업데이트
 */
void FriendsLayer::updatePosition(const Vec2 &ballPos, bool withAction) {
    
    // 모든 슬롯 클리어
    for( auto slot : slots ) {
        slot.clear();
    }
    
    // 프렌즈 슬롯 업데이트
    auto ballSlot = getNearSlot(ballPos);
    CCASSERT(!ballSlot.isNull(), "FriendsLayer::updatePosition error: invalid ball position.");
    
    auto sortedSlots = getSortedSlots(ballSlot);
    
    for( int i = 0; i < friends.size(); ++i ) {
        auto friendNode = friends[i];
        auto slot = sortedSlots[i];
        
        if( withAction ) {
            int diff = abs(slot.index - getNearSlot(friendNode->getPosition()).index);
            if( diff == 0 ) {
                slot.put(friendNode);
                continue;
            }
            
            friendNode->setImage(Friend::ImageType::MOVE);
            friendNode->setImageFlippedX(slot.pos.x < friendNode->getPositionX());
            
            auto move = MoveTo::create(diff * 0.25f, slot.pos);
            auto callFunc = CallFunc::create([=]() {
                friendNode->setImage(Friend::ImageType::IDLE);
                slot.put(friendNode);
            });
            friendNode->runAction(Sequence::create(move, callFunc, nullptr));
            
        } else {
            slot.put(friendNode);
        }
    }
}

/**
 * 아이템 획득
 */
void FriendsLayer::eatFriendsItem(Item *item) {
    
    switch( item->getData().type ) {
        // 프렌즈 볼 개수 증가
        case ItemType::FRIENDS_POWER_UP: {
            ++toAddFriendsBalls;
        } break;
            
        default:
            CCASSERT(false, "FriendsLayer::eatFriendsItem error: invalid item.");
            break;
    }
}

/**
 * 좌표와 가까운 슬롯을 반환합니다
 */
FriendsLayer::Slot FriendsLayer::getNearSlot(const Vec2 &pos) {
    
    int savedDist = INT_MAX;
    Slot nearSlot;
    
    for( auto slot : slots ) {
        float dist = slot.pos.getDistance(pos);
        
        // CCLOG("slotIndex: %d dist: %f", slot.index, dist);
        
        if( dist < savedDist ) {
            nearSlot = slot;
            savedDist = dist;
        }
    }
    
    return nearSlot;
}

/**
 * 볼 슬롯을 기준으로하여 정렬된 슬롯 리스트를 반환합니다
 */
FriendsLayer::Slots FriendsLayer::getSortedSlots(const Slot &ballSlot) {
    
    switch( ballSlot.index ) {
        case 0: return Slots({ slots[1], slots[2], slots[3], slots[4], });
        case 1: return Slots({ slots[0], slots[2], slots[3], slots[4], });
        case 2: return Slots({ slots[1], slots[3], slots[0], slots[4], });
        case 3: return Slots({ slots[2], slots[4], slots[1], slots[0], });
        case 4: return Slots({ slots[3], slots[2], slots[1], slots[0], });
        default:
            CCASSERT(false, "FriendsLayer::getSortedSlots error.");
            break;
    }
    
    return Slots();
}

/**
 * 게임 진입
 */
void FriendsLayer::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void FriendsLayer::onGameExit() {
}

/**
 * 게임 리셋
 */
void FriendsLayer::onGameReset() {
    
    toAddFriendsBalls = 0;
}

/**
 * 게임 시작
 */
void FriendsLayer::onGameStart() {
}

/**
 * 게임 재시작
 */
void FriendsLayer::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void FriendsLayer::onGamePause() {
}

/**
 * 게임 재게
 */
void FriendsLayer::onGameResume() {
}

/**
 * 게임 오버
 */
void FriendsLayer::onGameOver() {
    
    for( auto friendNode : friends ) {
        friendNode->setImage(Friend::ImageType::DIE);
    }
}

/**
 * 게임 이어하기
 */
void FriendsLayer::onGameContinue() {
}

/**
 * 게임 결과
 */
void FriendsLayer::onGameResult() {
}

/**
 * 부스트 시작
 */
void FriendsLayer::onBoostStart() {
}

/**
 * 부스트 종료
 */
void FriendsLayer::onBoostEnd() {
}

/**
 * 스테이지 변경
 */
void FriendsLayer::onStageChanged(const StageData &stage) {
    
    for( auto friendNode : friends ) {
        friendNode->setImage(Friend::ImageType::STAGE_START, [=]() {
            friendNode->setImage(Friend::ImageType::IDLE);
        });
    }
}

/**
 * 스테이지 클리어
 */
void FriendsLayer::onStageClear() {
}

/**
 * 다음 스테이지로 이동
 */
void FriendsLayer::onMoveNextStage(const StageData &stage) {
    
    for( auto friendNode : friends ) {
        friendNode->setImage(Friend::ImageType::STAGE_CLEAR);
    }
}

/**
 * 다음 스테이지로 이동 완료
 */
void FriendsLayer::onMoveNextStageFinished(const StageData &stage) {
}

/**
 * 층 변경
 */
void FriendsLayer::onFloorChanged(const FloorData &floor) {
}

/**
 * 다음 층
 */
void FriendsLayer::onNextFloor(const FloorData &floor) {
    
    // 획득한 아이템 반영
    if( toAddFriendsBalls > 0 ) {
        // TODO:
        toAddFriendsBalls = 0;
    }
}

/**
 * 프렌즈 초기화
 */
void FriendsLayer::initFriends() {
 
    // game_friends_02_idle1.png Vec2BL(46, 81) , Size(63, 75)
    Color3B colors[] = {
        Color3B(255,255,255),
        Color3B(0,255,0),
        Color3B(0,0,255),
        Color3B(0,0,0),
    };
    
    for( int i = 0; i < 1/*보유 프렌즈 수*/; ++i ) {
        auto friendNode = Friend::create();
        friendNode->setCascadeColorEnabled(true);
        friendNode->setColor(colors[i]);
        addChild(friendNode);
        
        friends.push_back(friendNode);
    }
    
    // 슬롯 리스트 생성
    int w = SB_WIN_SIZE.width / SLOT_COUNT;
    
    for( int i = 0; i < SLOT_COUNT; ++i ) {
        Slot slot;
        slot.index = i;
        slot.pos = Vec2BL((w*i) + (w*0.5f), FRIENDS_POS_Y);
        slots.push_back(slot);
        
        /*
        auto n = LayerColor::create(Color4B(0,0,255,255*0.5f));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(slot.pos);
        n->setContentSize(Size(70, 75));
        addChild(n);
        */
    }
}

/**
 * 게임 리스너 초기화
 */
void FriendsLayer::initGameListener() {
    
    auto listener = GameEventListener::create(this);
    listener->onGameEnter               = CC_CALLBACK_0(FriendsLayer::onGameEnter, this);
    listener->onGameExit                = CC_CALLBACK_0(FriendsLayer::onGameExit, this);
    listener->onGameReset               = CC_CALLBACK_0(FriendsLayer::onGameReset, this);
    listener->onGameStart               = CC_CALLBACK_0(FriendsLayer::onGameStart, this);
    listener->onGameRestart             = CC_CALLBACK_0(FriendsLayer::onGameRestart, this);
    listener->onGamePause               = CC_CALLBACK_0(FriendsLayer::onGamePause, this);
    listener->onGameResume              = CC_CALLBACK_0(FriendsLayer::onGameResume, this);
    listener->onGameOver                = CC_CALLBACK_0(FriendsLayer::onGameOver, this);
    listener->onGameContinue            = CC_CALLBACK_0(FriendsLayer::onGameContinue, this);
    listener->onGameResult              = CC_CALLBACK_0(FriendsLayer::onGameResult, this);
    listener->onBoostStart              = CC_CALLBACK_0(FriendsLayer::onBoostStart, this);
    listener->onBoostEnd                = CC_CALLBACK_0(FriendsLayer::onBoostEnd, this);
    listener->onStageChanged            = CC_CALLBACK_1(FriendsLayer::onStageChanged, this);
    listener->onStageClear              = CC_CALLBACK_0(FriendsLayer::onStageClear, this);
    listener->onMoveNextStage           = CC_CALLBACK_1(FriendsLayer::onMoveNextStage, this);
    listener->onMoveNextStageFinished   = CC_CALLBACK_1(FriendsLayer::onMoveNextStageFinished, this);
    listener->onFloorChanged            = CC_CALLBACK_1(FriendsLayer::onFloorChanged, this);
    listener->onNextFloor               = CC_CALLBACK_1(FriendsLayer::onNextFloor, this);
    GameManager::getEventDispatcher()->addListener(listener);
}
