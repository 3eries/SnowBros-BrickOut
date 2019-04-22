//
//  FriendsLayer.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 30/01/2019.
//

#include "FriendsLayer.hpp"

#include "GameConfiguration.hpp"
#include "User.hpp"

#include "TileLayer.hpp"
#include "tile/Item.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

FriendsLayer* FriendsLayer::create(TileLayer *tileLayer) {
    
    auto layer = new FriendsLayer(tileLayer);
    
    if( layer && layer->init() ) {
        layer->autorelease();
        return layer;
    }
    
    CC_SAFE_DELETE(layer);
    return nullptr;
}

FriendsLayer::FriendsLayer(TileLayer *tileLayer) :
tileLayer(tileLayer),
onFallFinishedListener(nullptr),
friendsDamage(1) {
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
    
    // 덱 변경 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_FRIENDS_DECK, [=](EventCustom *event) {
        
        // 기존 프렌즈 제거
        for( auto friendNode : friends ) {
            friendNode->removeFromParent();
        }
        
        friends.clear();
        slots.clear();
        
        // 프렌즈 초기화
        this->initFriends();
        this->updatePosition(ballPosition, false);
        this->setDamageVisible(false);
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void FriendsLayer::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 프렌즈 초기화
 */
void FriendsLayer::initFriends() {
    
    auto friendDatas = User::getFriendsDeck();
    // random_shuffle(friendDatas.begin(), friendDatas.end());
    
    for( int i = 0; i < friendDatas.size(); ++i ) {
        auto friendNode = createFriend(friendDatas[i]);
        friendNode->setOnFallFinishedListener(CC_CALLBACK_1(FriendsLayer::onFallFinished, this));
        friendNode->setTileLayer(tileLayer);
        friendNode->setCascadeColorEnabled(true);
        addChild(friendNode);
        
        friends.push_back(friendNode);
    }
    
    updateFriendsDamage();
    
    // 슬롯 리스트 생성
    int w = SB_WIN_SIZE.width / GAME_FRIENDS_SLOT_COUNT;
    
    for( int i = 0; i < GAME_FRIENDS_SLOT_COUNT; ++i ) {
        Slot slot;
        slot.index = i;
        slot.pos = Friend::getSlotPosition(i);
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
    listener->onGameOver                = CC_CALLBACK_0(FriendsLayer::onGameOver, this);
    listener->onGameContinue            = CC_CALLBACK_0(FriendsLayer::onGameContinue, this);
    listener->onGameRestore             = CC_CALLBACK_1(FriendsLayer::onGameRestore, this);
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

Friend* FriendsLayer::createFriend(const FriendData &data) {
    
    FriendDef def(data);
    def.damage = friendsDamage;
    def.tileLayer = tileLayer;
    
    return Friend::create(def);
}

/**
 * 프렌즈 볼 발사
 */
void FriendsLayer::shoot() {
    
    if( shot ) {
        return;
    }
    
    Log::i("FriendsLayer::shoot");
    
    shot = true;
    
    for( int i = 0; i < friends.size(); ++i ) {
        auto friendNode = friends[i];
        friendNode->shoot(i*1.0f);
    }
}

/**
 * 프렌즈 볼 발사 정지
 */
void FriendsLayer::shootStop() {

    for( auto friendNode : friends ) {
        friendNode->shootStop();
    }
}

/**
 * 1개 프렌드의 볼 추락 완료
 */
void FriendsLayer::onFallFinished(Friend *friendNode) {
    
    // 모든 볼이 추락했으면 리스너 실행
    if( isFallFinished() && onFallFinishedListener ) {
        onFallFinishedListener();
    }
}

/**
 * 프렌즈 볼 회수
 */
void FriendsLayer::withdrawBall() {

    for( auto friendNode : friends ) {
        friendNode->withdrawBall();
    }
}

/**
 * 모든 볼이 추락했는지를 반환합니다
 */
bool FriendsLayer::isFallFinished() {
    
    for( auto friendNode : friends ) {
        if( !friendNode->isFallFinished() ) {
            return false;
        }
    }
    
    return true;
}

/**
 * 아이템 획득
 */
void FriendsLayer::eatFriendsItem(Item *item) {
    
    switch( item->getData().type ) {
        // 프렌즈 파워업
        case ItemType::FRIENDS_POWER_UP: {
            ++friendsDamage;
            tileLayer->setFriendsDamage(friendsDamage);
            
            // 연출
            for( auto friendNode : friends ) {
                auto effect = Sprite::create(DIR_IMG_GAME + "game_power_up.png");
                effect->setAnchorPoint(ANCHOR_MB);
                effect->setPosition(Vec2(friendNode->getPositionX(), SHOOTING_POSITION_Y) + Vec2(0, 30));
                addChild(effect, SBZOrder::MIDDLE);
                
                auto move = MoveBy::create(0.8f, Vec2(0, 50));
                auto remove = RemoveSelf::create();
                effect->runAction(Sequence::create(move, remove, nullptr));
            }
        } break;
            
        default:
            CCASSERT(false, "FriendsLayer::eatFriendsItem error: invalid item.");
            break;
    }
}

/**
 * 프렌즈 데미지 업데이트
 */
void FriendsLayer::updateFriendsDamage() {
    
    Log::i("FriendsLayer::updateFriendsDamage damage: %d", friendsDamage);
    
    for( auto friendNode : friends ) {
        friendNode->setDamage(friendsDamage);
    }
    
    tileLayer->setFriendsDamage(friendsDamage);
}

/**
 * 볼을 기준으로 좌표 업데이트
 */
void FriendsLayer::updatePosition(const Vec2 &ballPos, bool withAction) {
    
    this->ballPosition = ballPos;
    
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

void FriendsLayer::setDamageVisible(bool isVisible) {
    
    for( int i = 0; i < friends.size(); ++i ) {
        auto friendNode = friends[i];
        friendNode->setDamageVisible(isVisible);
    }
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
    
    shot = false;
    friendsDamage = 1;
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
 * 게임 오버
 */
void FriendsLayer::onGameOver() {
    
    for( auto friendNode : friends ) {
        friendNode->setImage(Friend::ImageType::DIE);
        
        auto damageLabel = friendNode->getDamageLabel();
        damageLabel->runAction(FadeOut::create(0.1f));
    }
}

/**
 * 게임 이어하기
 */
void FriendsLayer::onGameContinue() {
}

/**
 * 게임 리스토어
 */
void FriendsLayer::onGameRestore(const RestoreData &restoreData) {
    
    // 기존 프렌즈 제거
    for( auto friendNode : friends ) {
        friendNode->removeFromParent();
    }
    
    friends.clear();
    slots.clear();
    
    // 프렌즈 초기화
    friendsDamage = restoreData.friendsBallDamage;
    
    initFriends();
    updatePosition(restoreData.ballPosition, false);
    
    for( auto friendNode : friends ) {
        friendNode->setImage(Friend::ImageType::STAGE_START, [=]() {
            friendNode->setImage(Friend::ImageType::IDLE);
        });
    }
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
    
    shot = false;
    
    // 데미지 업데이트
    updateFriendsDamage();
    setDamageVisible(true);
}

/**
 * 다음 층
 */
void FriendsLayer::onNextFloor(const FloorData &floor) {
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

