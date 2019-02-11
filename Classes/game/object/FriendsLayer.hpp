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

class Item;

class FriendsLayer : public cocos2d::Node {
public:
    CREATE_FUNC(FriendsLayer);
    ~FriendsLayer();
    
private:
    FriendsLayer();
    
    bool init() override;
    void cleanup() override;
    
    void initFriends();
    void initGameListener();
    
public:
    void updatePosition(const cocos2d::Vec2 &ballPos, bool withAction);
    void eatFriendsItem(Item *item);
    
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
    
    Slot  getNearSlot(const cocos2d::Vec2 &pos);
    Slots getSortedSlots(const Slot &ballSlot);
    
// Game Event
private:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameContinue();
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
    std::vector<Friend*> friends;
    Slots slots;
    
    int toAddFriendsBalls;               // 프렌즈에 추가돼야 할 볼 개수
};

#endif /* FriendsLayer_hpp */
