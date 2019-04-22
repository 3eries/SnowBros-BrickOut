//
//  FriendsLayer.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 30/01/2019.
//

#ifndef FriendsLayer_hpp
#define FriendsLayer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../GameManager.hpp"

#include "friend/Friend.hpp"

class TileLayer;
class Item;

class FriendsLayer : public cocos2d::Node {
public:
    static FriendsLayer* create(TileLayer *tileLayer);
    ~FriendsLayer();
    
private:
    FriendsLayer(TileLayer *tileLayer);
    
    bool init() override;
    void cleanup() override;
    
    void initFriends();
    void initGameListener();
    
    Friend* createFriend(const FriendData &data);
    
public:
    void shoot();
    void shootStop();
    
    void onFallFinished(Friend *friendNode);
    bool isFallFinished();
    
    void withdrawBall();
    
    void eatFriendsItem(Item *item);
    
    void updateFriendsDamage();
    void updatePosition(const cocos2d::Vec2 &ballPos, bool withAction);
    
    void setDamageVisible(bool isVisible);
    
// Game Event
private:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGameOver();
    void onGameContinue();
    void onGameRestore(const RestoreData &restoreData);
    void onGameResult();
    
    void onBoostStart();
    void onBoostEnd();
    
    void onStageChanged(const StageData &stage);
    void onStageClear();
    
    void onMoveNextStage(const StageData &stage);
    void onMoveNextStageFinished(const StageData &stage);
    
    void onFloorChanged(const FloorData &floor);
    void onNextFloor(const FloorData &floor);
    
private:
    CC_SYNTHESIZE(SBCallback, onFallFinishedListener, OnFallFinishedListener);
    
    TileLayer *tileLayer;
    std::vector<Friend*> friends;
    
    // 볼 발사 여부
    SB_SYNTHESIZE_READONLY_BOOL(shot, Shot);
    
    // 프렌즈 데미지
    CC_SYNTHESIZE_READONLY(int, friendsDamage, FriendsDamage);
    
private:
    struct Slot {
        int index;
        cocos2d::Vec2 pos;
        Friend *friendNode;
        
        Slot() : index(-1), friendNode(nullptr) {}
        
        bool isNull() {
            return index == -1;
        }
        
        void put(Friend *friendNode) const {
            // this->friendNode = friendNode;
            friendNode->setPosition(pos);
            friendNode->setImageFlippedX(index >= 3);
        }
        
        void clear() {
            this->friendNode = nullptr;
        }
    };
    
    typedef std::vector<Slot> Slots;
    Slots slots;
    cocos2d::Vec2 ballPosition;
    
    Slot  getNearSlot(const cocos2d::Vec2 &pos);
    Slots getSortedSlots(const Slot &ballSlot);
};

#endif /* FriendsLayer_hpp */
