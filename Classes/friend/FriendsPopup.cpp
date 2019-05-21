//
//  FriendsPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 18/04/2019.
//

#include "FriendsPopup.hpp"

#include "Define.h"
#include "User.hpp"

#include "ContentManager.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"

#include "FriendInfoView.hpp"
#include "FriendCell.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

#define LIST_ORIGIN             Vec2MC(0, -99)        // mask Vec2MC(-1, -99) , Size(634, 412)
#define LIST_SIZE               Size(634,412)
#define LIST_CELL_SIZE          Size(608,134)          // bg Vec2MC(0, 0) , Size(608, 134)
#define LIST_CELL_PADDING       2

// static const float FADE_DURATION    = 0.15f;

FriendsPopup* FriendsPopup::create() {
    
    auto popup = new FriendsPopup();
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

FriendsPopup::FriendsPopup() : BasePopup(PopupType::FRIENDS),
selectedListCell(nullptr) {
}

FriendsPopup::~FriendsPopup() {
}

bool FriendsPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void FriendsPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void FriendsPopup::dismiss() {

    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(DIRECTOR_EVENT_UPDATE_FRIENDS_DECK);
    
    BasePopup::dismiss();
}

void FriendsPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(/*Color::POPUP_BG*/Color4B(0,0,0,255*0.8f));
}

void FriendsPopup::initContentView() {
    
    BasePopup::initContentView();
    
    initBg();
    initUserCoin();
    initInfoView();
    initListView();
    initDeck();
    initButton();
    
    updateDeckUI();
    
    // 최초 프렌즈 선택
    auto deck = User::getFriendsDeck();
    
    if( deck.size() > 0 ) {
        onSelectedFriend(deck[0]);
    } else {
        onSelectedFriend(Database::getFriends()[0]);
    }
}

/**
 * 유저 코인 UI 업데이트
 */
void FriendsPopup::updateUserCoinUI(bool withAction) {
    
    auto coinLabel = getChildByTag<Label*>(Tag::USER_COIN_LABEL);
    int  coin = User::getCoin();
    
    if( !withAction ) {
        coinLabel->setString(TO_STRING(coin));
        return;
    }
    
    // 액션
    auto onChanged = [=](float value) {
        
        int i = (int)value;
        coinLabel->setString(TO_STRING(i));
    };
    
    auto numberAction = ActionFloat::create(0.3f, TO_INTEGER(coinLabel->getString()), coin, onChanged);
    coinLabel->runAction(numberAction);
}

/**
 * 덱 UI 업데이트
 */
void FriendsPopup::updateDeckUI() {
    
    auto deck = User::getFriendsDeck();
    
    for( int i = 0; i < deck.size(); ++i ) {
        auto data = deck[i];
        auto cell = deckCells[i];
        cell->setFriend(data);
        cell->setVisible(true);
        cell->setSelected(selectedFriend.friendId == data.friendId);
    }
    
    for( int i = (int)deck.size(); i < deckCells.size(); ++i ) {
        auto cell = deckCells[i];
        cell->setFriend(FriendData());
        cell->setVisible(false);
    }
}

/**
 * 리스트의 프렌드 셀을 반환합니다
 */
FriendCell* FriendsPopup::getFriendCellOfList(const string &friendId) {
    
    for( auto listCell : listCells ) {
        auto friendCells = listCell->getFriendCells();
        
        for( auto friendCell : friendCells ) {
            if( friendCell->getData().friendId == friendId ) {
                return friendCell;
            }
        }
    }
    
    return nullptr;
}

/**
 * 덱의 프렌드 셀을 반환합니다
 */
FriendCell* FriendsPopup::getFriendCellOfDeck(const string &friendId) {
    
    for( auto cell : deckCells ) {
        if( cell->getData().friendId == friendId ) {
            return cell;
        }
    }
    
    return nullptr;
}

/**
 * 버튼 클릭
 */
void FriendsPopup::onClick(Tag tag) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( tag ) {
        // OK
        case Tag::BTN_OK: {
            this->dismissWithAction();
        } break;
            
        // 프렌즈 구매
        case Tag::BTN_BUY: {
            CCLOG("FRIENDS BUY: %s", selectedFriend.friendId.c_str());
            
            // 코인 체크
            if( !User::isEnoughCoin(selectedFriend.price) ) {
                PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
                return;
            }
            
            User::spendCoin(selectedFriend.price);
            User::ownFriend(selectedFriend.friendId);
            
            infoView->updateSelf();
            
        } break;
            
        // 프렌즈 선택
        case Tag::BTN_SELECT: {
            CCLOG("FRIENDS SELECT: %s", selectedFriend.friendId.c_str());
            
            // update deck
            auto deck = User::getFriendsDeck();
            
            // 이미 4개인 경우, 첫번째 프렌드 제거 (FIFO)
            if( deck.size() == 4 ) {
                SBCollection::removeAtIndex(deck, 0);
            }
            
            deck.push_back(selectedFriend);
            
            User::setFriendsDeck(deck);
            
            // update ui
            infoView->updateSelf();
            updateDeckUI();
            
        } break;
            
        // 프렌즈 선택 해제
        case Tag::BTN_UNSELECT: {
            CCLOG("FRIENDS UNSELECT: %s", selectedFriend.friendId.c_str());
            
            // update deck
            auto deck = User::getFriendsDeck();
            SBCollection::remove(deck, [=](const FriendData &friendData) {
                return friendData.friendId == selectedFriend.friendId;
            });
            
            User::setFriendsDeck(deck);
            
            // update ui
            infoView->updateSelf();
            updateDeckUI();
            
        } break;
            
        default: break;
    }
}

void FriendsPopup::onClick(Node *sender) {
    
    onClick((Tag)sender->getTag());
}

/**
 * 프렌드 선택
 */
void FriendsPopup::onSelectedFriend(const FriendData &friendData) {
    
    string friendId = friendData.friendId;
    
    if( selectedFriend.friendId == friendId ) {
        return;
    }
    
    selectedFriend = friendData;
    
    infoView->setFriend(Database::getFriend(friendId));
    onSelectedListFriendCell(getFriendCellOfList(friendId));
    onSelectedDeckFriendCell(getFriendCellOfDeck(friendId));
}

/**
 * 리스트의 프렌드 셀 선택
 */
void FriendsPopup::onSelectedListFriendCell(FriendCell *cell) {

    CCASSERT(cell != nullptr, "FriendsPopup::onSelectedListCell error.");
    // CCLOG("onSelectedListFriendCell friend id: %s", cell->getData().friendId.c_str());

    if( selectedListCell == cell ) {
        return;
    }

    // 이전 셀 선택 해제
    auto prevCell = selectedListCell;

    if( prevCell ) {
        prevCell->setSelected(false);
    }

    // 셀 선택
    selectedListCell = cell;
    selectedListCell->setSelected(true);

    // 이전과 다른행인 경우 스크롤
    if( prevCell ) {
        auto prevListCell = (Friends::ListCell*)prevCell->getUserObject();
        auto currListCell = (Friends::ListCell*)cell->getUserObject();
        
        if( prevListCell->getCellIndex() != currListCell->getCellIndex() ) {
            listView->setBounceEnabled(false);
            listView->scrollToItem(currListCell->getCellIndex(), Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
        }
    }
}

/**
 * 덱의 프렌드 셀 선택
 */
void FriendsPopup::onSelectedDeckFriendCell(FriendCell *cell) {
    
    // 이전 셀 선택 해제
    for( auto cell : deckCells  ) {
        cell->setSelected(false);
    }
    
    // 셀 선택
    if( cell ) {
        cell->setSelected(true);
    }
}

/**
 * 배경 초기화
 */
void FriendsPopup::initBg() {
    
    // common_bg_full.png Vec2MC(0, -8) , Size(712, 1280)
    // friends_title.png Vec2TC(0, -84) , Size(440, 76)
    // friends_bg_deck.png Vec2BC(0, 242) , Size(616, 148)
    // friends_title_deck.png Vec2BC(0, 306) , Size(196, 44)
    SBUIInfo infos[] = {
        SBUIInfo(Node::INVALID_TAG, 0, ANCHOR_M, Vec2MC(0, -8),    DIR_IMG_COMMON + "common_bg_full.png"),
        SBUIInfo(Node::INVALID_TAG, 0, ANCHOR_M, Vec2TC(0, -84),   DIR_IMG_FRIEND + "friends_title.png"),
        SBUIInfo(Node::INVALID_TAG, 0, ANCHOR_M, Vec2BC(0, 242),   DIR_IMG_FRIEND + "friends_bg_deck.png"),
        SBUIInfo(Node::INVALID_TAG, 1, ANCHOR_M, Vec2BC(0, 306),   DIR_IMG_FRIEND + "friends_title_deck.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(info.file);
        info.apply(spr);
        addBackgroundChild(spr);
    }
    
    // 리스트 바깥 영역 터치 시 팝업 종료
    /*
     auto touchNode = SBNodeUtils::createTouchNode();
     addContentChild(touchNode, -1);
     
     touchNode->addClickEventListener([=](Ref*) {
     SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
     this->dismissWithAction();
     });
     */
    
    // 닫기 버튼
    // common_btn_close_red.png Vec2TR(-42, -42) , Size(68, 68)
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_close_red.png");
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(Vec2TR(-42, -42));
    closeBtn->setZoomScale(ButtonZoomScale::HARD);
    addContentChild(closeBtn);
    
    closeBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
}

/**
 * 유저 코인 초기화
 */
void FriendsPopup::initUserCoin() {
    
    // bg
    // common_bg_coin_user.png Vec2TR(-184, -166) , Size(272, 52)
    auto bg = Sprite::create(DIR_IMG_FRIEND + "friends_bg_coin_user.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2TR(-184, -166));
    addChild(bg);
    
    // icon
    auto icon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
    icon->setAnchorPoint(ANCHOR_M);
    icon->setPosition(Vec2TR(-294, -166));
    addChild(icon);
    
    // amount
    auto amount = Label::createWithTTF("", FONT_COMMODORE, 36, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    amount->setTag(Tag::USER_COIN_LABEL);
    amount->setAnchorPoint(ANCHOR_M);
    amount->setPosition(Vec2TR(-192, -166 + 2));
    amount->setTextColor(Color4B(255,255,255,255));
    addChild(amount);
    
    auto shopBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_shop.png");
    shopBtn->setAnchorPoint(ANCHOR_M);
    shopBtn->setPosition(Vec2TR(-78, -166));
    shopBtn->setZoomScale(ButtonZoomScale::HARD);
    addChild(shopBtn);
    
    shopBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
    });
    
    updateUserCoinUI(false);
    
    // 코인 업데이트 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_USER_COIN, [=](EventCustom *event) {
        this->updateUserCoinUI(true);
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/**
 * 프렌즈 정보 View 초기화
 */
void FriendsPopup::initInfoView() {
    
    infoView = FriendInfoView::create();
    infoView->setOnClickButtonListener([=](FriendInfoView::Button btn) {
        
        Tag tag;
        
        switch( btn ) {
            case FriendInfoView::Button::BUY:          tag = Tag::BTN_BUY;       break;
            case FriendInfoView::Button::SELECT:       tag = Tag::BTN_SELECT;    break;
            case FriendInfoView::Button::UNSELECT:     tag = Tag::BTN_UNSELECT;  break;
            default: break;
        }
        
        this->onClick(tag);
    });
    addChild(infoView);
}

/**
 * 프렌즈 리스트 View 초기화
 */
void FriendsPopup::initListView() {
    
    // 리스트뷰 초기화
    // mask Vec2MC(-1, -99) , Size(634, 412)
    listView = ListView::create();
    listView->setDirection(ui::ScrollView::Direction::VERTICAL);
    listView->setPadding(0, 5, 0, 0);
    listView->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    listView->setItemsMargin(LIST_CELL_PADDING);
    listView->setBounceEnabled(true);
    listView->setScrollBarEnabled(false);
    listView->setAnchorPoint(ANCHOR_M);
    listView->setPosition(LIST_ORIGIN);
    listView->setContentSize(LIST_SIZE);
    listView->addEventListener([=](Ref*, ScrollView::EventType eventType) {
        
        if( eventType == ScrollView::EventType::AUTOSCROLL_ENDED ) {
            // CCLOG("AUTOSCROLL_ENDED");
            listView->setBounceEnabled(true);
        }
    });
    
    addContentChild(listView);
    
    // debug draw
    /*
     {
     auto n = LayerColor::create(Color4B(255,255,255,255*0.7f));
     n->setIgnoreAnchorPointForPosition(false);
     n->setAnchorPoint(ballListView->getAnchorPoint());
     n->setPosition(ballListView->getPosition());
     n->setContentSize(ballListView->getContentSize());
     ballListView->getParent()->addChild(n, -1);
     }
     */
    
    // Cell 초기화
    // pack_cell Vec2MC(5, 134) , Size(608, 134)
    // pack_cell Vec2MC(5, -2) , Size(608, 134)
    // pack_cell Vec2MC(5, -138) , Size(608, 134)
    auto packs = Database::getFriendsPacks();
    packs.push_back(FriendsPackData()); // 커밍순 데이터 추가
    
    for( int i = 0; i < packs.size(); ++i ) {
        auto pack = packs[i];
        
        auto cell = Friends::ListCell::create(pack);
        cell->setCellIndex(i);
        cell->setAnchorPoint(ANCHOR_M);
        cell->setOnClickListener([=](Friends::ListCell *listCell, FriendCell *friendCell) {
            this->onSelectedFriend(friendCell->getData());
        });
        listView->pushBackCustomItem(cell);
        
        listCells.push_back(cell);
    }
}

/**
 * 덱 초기화
 */
void FriendsPopup::initDeck() {
    
    // friends_00001_idle_01.png Vec2BC(-192, 272) , Size(162, 150)
    // friends_00001_idle_01.png Vec2BC(-64, 272) , Size(162, 150)
    // friends_00001_idle_01.png Vec2BC(64, 272) , Size(162, 150)
    // friends_00001_idle_01.png Vec2BC(192, 272) , Size(162, 150)
    
    // friends_focus_deck.png Vec2BC(-192, 246) , Size(112, 92)
    // friends_focus_deck.png Vec2BC(-64, 246) , Size(112, 92)
    // friends_focus_deck.png Vec2BC(64, 246) , Size(112, 92)
    // friends_focus_deck.png Vec2BC(192, 246) , Size(112, 92)
    Vec2 pos[] = {
        Vec2BC(-192, 246),
        Vec2BC(-64, 246),
        Vec2BC(64, 246),
        Vec2BC(192, 246),
    };
    
    for( int i = 0; i < 4; ++i ) {
        auto friendCell = FriendCell::create(DIR_IMG_FRIEND + "friends_focus_deck.png");
        friendCell->setVisible(false);
        friendCell->setAnchorPoint(ANCHOR_M);
        friendCell->setPosition(pos[i]);
        friendCell->setFriendPaddingY(26);
        friendCell->setOnClickListener([=](FriendCell *friendCell) {
            this->onSelectedFriend(friendCell->getData());
        });
        addChild(friendCell);
        
        deckCells.push_back(friendCell);
    }
}

/**
 * 버튼 초기화
 */
void FriendsPopup::initButton() {
 
    auto okBtn = SBButton::create(DIR_IMG_FRIEND + "friends_btn_ok.png");
    okBtn->setTag(Tag::BTN_OK);
    okBtn->setAnchorPoint(ANCHOR_M);
    okBtn->setPosition(Vec2BC(0, 100));
    okBtn->setZoomScale(ButtonZoomScale::WEAK);
    addChild(okBtn);
    
    okBtn->setOnClickListener([=](Node *sender) {
        this->onClick(sender);
    });
}

/**
 * 등장 연출
 */
void FriendsPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(0, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
    
    // 배경 fade in
    /*
     runBackgroundFadeInAction([=]() {
     
     this->onEnterActionFinished();
     SB_SAFE_PERFORM_LISTENER(this, onFinished);
     
     }, FADE_DURATION);
     */
}

/**
 * 퇴장 연출
 */
void FriendsPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
    
    // 배경 fade out
    /*
     runBackgroundFadeOutAction([=]() {
     
     this->onExitActionFinished();
     SB_SAFE_PERFORM_LISTENER(this, onFinished);
     
     }, FADE_DURATION);
     */
}

/**
 * 등장 연출 완료
 */
void FriendsPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}

NS_FRIENDS_BEGIN

#pragma mark- ListCell

ListCell* ListCell::create(const FriendsPackData &data) {
    
    auto cell = new ListCell();
    
    if( cell && cell->init(data) ) {
        cell->autorelease();
        return cell;
    }
    
    CC_SAFE_DELETE(cell);
    return nullptr;
}

ListCell::ListCell() :
onClickListener(nullptr) {
}

ListCell::~ListCell() {
}

bool ListCell::init(const FriendsPackData &data) {
    
    if( !Widget::init() ) {
        return false;
    }
    
    this->data = data;
    
    auto size = LIST_CELL_SIZE;
    setContentSize(size);
    
    // bg
    // friends_bg_pack.png Vec2MC(0, -7) , Size(608, 120)
    auto bg = Sprite::create(DIR_IMG_FRIEND + "friends_bg_pack.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(size, 0, -7));
    addChild(bg);
    
    // 패키지 아이디가 없으면 커밍순으로 처리
    // friends_text_coming_soon.png
    if( data.packId == "" ) {
        auto comingSoon = Sprite::create(DIR_IMG_FRIEND + "friends_text_coming_soon.png");
        comingSoon->setAnchorPoint(ANCHOR_M);
        comingSoon->setPosition(Vec2MC(size, 0, 0));
        addChild(comingSoon);
        
        return true;
    }
    
    // title
    // SNOW BROS2 size:30 stroke:4 Vec2TC(-3, -14) , Size(223, 33)
    auto titleLabel = Label::createWithTTF(data.packName, FONT_COMMODORE, 30, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2TC(size, 0, -14));
    titleLabel->setTextColor(Color4B(255,255,255,255));
    titleLabel->enableOutline(Color4B(0,0,0,255), 4);
    addChild(titleLabel);
    
    // friends_focus_friend.png Vec2MC(-164, -9) , Size(88, 84)
    // friends_focus_friend.png Vec2MC(-60, -9) , Size(88, 84)
    // friends_focus_friend.png Vec2MC(44, -9) , Size(88, 84)
    // friends_focus_friend.png Vec2MC(148, -9) , Size(88, 84)
    Vec2 pos[] = {
        Vec2MC(size, -164, -9),
        Vec2MC(size, -60, -9),
        Vec2MC(size, 44, -9),
        Vec2MC(size, 148, -9),
    };
    
    for( int i = 0; i < data.friends.size(); ++i ) {
        auto friendCell = FriendCell::create(DIR_IMG_FRIEND + "friends_focus_friend.png");
        friendCell->setUserObject(this);
        friendCell->setAnchorPoint(ANCHOR_M);
        friendCell->setPosition(pos[i]);
        friendCell->setFriendPaddingY(28);
        friendCell->setFriend(data.friends[i]);
        friendCell->setOnClickListener([=](FriendCell*) {
            if( onClickListener ) {
                onClickListener(this, friendCell);
            }
        });
        addChild(friendCell);
        
        friendCells.push_back(friendCell);
    }
    
    return true;
}

NS_FRIENDS_END
