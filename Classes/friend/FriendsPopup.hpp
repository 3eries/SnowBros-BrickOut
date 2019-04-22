//
//  FriendsPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 18/04/2019.
//

#ifndef FriendsPopup_hpp
#define FriendsPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "../content/data/model/FriendsPackData.h"

#define NS_FRIENDS_BEGIN         namespace Friends {
#define NS_FRIENDS_END           }
#define USING_NS_FRIENDS         using namespace Friends

class FriendInfoView;
class FriendCell;

NS_FRIENDS_BEGIN

class ListCell;

NS_FRIENDS_END

typedef std::vector<FriendCell*> FriendCells;
typedef std::function<void(FriendCell*)> OnFriendCellListener;

class FriendsPopup : public BasePopup {
public:
    static FriendsPopup* create();
    ~FriendsPopup();

private:
    enum Tag {
        USER_COIN_LABEL = 10,
        
        BTN_OK          = 100,
        BTN_BUY,
        BTN_SELECT,
        BTN_UNSELECT,
    };
    
private:
    FriendsPopup();
    
    bool init() override;
    void onEnter() override;
    void dismiss() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initBg();
    void initUserCoin();
    void initInfoView();
    void initListView();
    void initDeck();
    void initButton();
    
    void updateUserCoinUI(bool withAction);
    void updateDeckUI();
    
    FriendCell* getFriendCellOfList(const std::string &friendId);
    FriendCell* getFriendCellOfDeck(const std::string &friendId);

    void onSelectedFriend(const FriendData &friendData);
    void onSelectedListFriendCell(FriendCell *cell);
    void onSelectedDeckFriendCell(FriendCell *cell);
    
    void onClick(Tag tag);
    void onClick(cocos2d::Node *sender) override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    FriendData              selectedFriend;
    
    FriendInfoView*         infoView;
    cocos2d::ui::ListView*  listView;
    
    std::vector<Friends::ListCell*> listCells;
    FriendCell*             selectedListCell;
    
    FriendCells             deckCells;
};

NS_FRIENDS_BEGIN

/**
 * ListCell
 */
class ListCell : public cocos2d::ui::Widget {
public:
    static ListCell* create(const FriendsPackData &data);
    ~ListCell();
    
private:
    ListCell();
    
    bool init(const FriendsPackData &data);
    
    CC_SYNTHESIZE_READONLY(FriendsPackData, data, Data);
    CC_SYNTHESIZE(std::function<void(ListCell*,FriendCell*)>, onClickListener,
                  OnClickListener);
    CC_SYNTHESIZE(int, cellIndex, CellIndex);
    CC_SYNTHESIZE_READONLY(FriendCells, friendCells, FriendCells);
};

NS_FRIENDS_END

#endif /* FriendsPopup_hpp */
