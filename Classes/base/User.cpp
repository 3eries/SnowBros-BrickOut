//
//  User.cpp
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "User.hpp"

#include "SceneManager.h"
#include "GameConfiguration.hpp"
#include "UserDefaultKey.h"

#include "ReviewPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define USER_DEFAULT_KEY_USER_COIN                      "USER_COIN"
#define USER_DEFAULT_KEY_CLEAR_STAGE_SEQUENCE           "USER_DEFAULT_KEY_CLEAR_STAGE_SEQUENCE"

#define USER_DEFAULT_KEY_OWNED_BALL_SKINS               "OWNED_BALL_SKINS"
#define USER_DEFAULT_KEY_SELECTED_BALL_SKIN             "SELECTED_BALL_SKIN"

#define USER_DEFAULT_KEY_OWNED_FRIENDS                  "OWNED_FRIENDS"
#define USER_DEFAULT_KEY_FRIENDS_DECK                   "FRIENDS_DECK"

static User *instance = nullptr;
User* User::getInstance() {
    
    if( !instance ) {
        instance = new User();
    }
    
    return instance;
}

void User::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

User::User() {
    
}

User::~User() {
 
    iap::IAPHelper::getInstance()->removeListener(this);
}

void User::init() {
    
    initOwnedBallSkins();
    initOwnedFriends();
    
    CCLOG("User {");
    CCLOG("\tcoin: %d, clear stage seq: %d", getCoin(), getClearStageSeq());
    CCLOG("}");
    
    // IAP 리스너 초기화
    auto onRemoveAds = [=]() {
        this->removeAds();
    };
    
    // purchase listener
    auto purchaseListener = iap::PurchaseListener::create();
    purchaseListener->setForever(true);
    purchaseListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
    
    // restore listener
    auto restoreListener = iap::RestoreListener::create();
    restoreListener->setForever(true);
    restoreListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, restoreListener);
    
    if( isOwnRemoveAdsItem() ) {
        removeAds();
    }
}

/**
 * 소유한 볼을 초기화합니다
 */
void User::initOwnedBallSkins() {
 
    string json = UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_OWNED_BALL_SKINS, "");
    
    // 초기 값 설정
    if( json == "" ) {
        ownBallSkin(GAME_CONFIG->getFirstBallSkin());
        setSelectedBallSkin(GAME_CONFIG->getFirstBallSkin());
    }
    // json 파싱
    else {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            ballSkins.push_back(list[i].GetString());
        }
    }
}

/**
 * 소유한 프렌즈를 초기화합니다
 */
void User::initOwnedFriends() {
    
    string json = UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_OWNED_FRIENDS, "");
    
    // 초기 값 설정
    if( json == "" ) {
        ownFriend(GAME_CONFIG->getFirstFriend());
        setFriendsDeck(StringList({GAME_CONFIG->getFirstFriend()}));
    }
    // json 파싱
    else {
        auto parse = [](string json, StringList &strs) {
            
            rapidjson::Document doc;
            doc.Parse(json.c_str());
            
            auto list = doc.GetArray();
            
            for( int i = 0; i < list.Size(); ++i ) {
                strs.push_back(list[i].GetString());
            }
        };
        
        parse(json, friends);
        parse(UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_FRIENDS_DECK, ""), friendsDeck);
        
        
        
//        rapidjson::Document doc;
//        doc.Parse(json.c_str());
//
//        auto list = doc.GetArray();
//
//        for( int i = 0; i < list.Size(); ++i ) {
//            friends.push_back(list[i].GetString());
//        }
    }
}

/**
 * 코인을 설정합니다
 */
void User::setCoin(int i) {
    
    UserDefault::getInstance()->setIntegerForKey(USER_DEFAULT_KEY_USER_COIN, i);
    UserDefault::getInstance()->flush();
    
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(DIRECTOR_EVENT_UPDATE_USER_COIN);
}

/**
 * 코인을 추가합니다
 */
void User::addCoin(int i) {

    int coin = getCoin() + i;
    setCoin(coin);
}

/**
 * 코인을 소모합니다
 * @return 코인이 충분한지 않으면 false를 반환합니다
 */
bool User::spendCoin(int i) {
    
    int coin = getCoin() - i;
    if( coin < 0 ) {
        return false;
    }
    
    setCoin(coin);
    return true;
}

/**
 * 코인을 반환합니다
 */
int User::getCoin() {
    
    return UserDefault::getInstance()->getIntegerForKey(USER_DEFAULT_KEY_USER_COIN, GAME_CONFIG->getFirstCoin());
}

/**
 * 코인이 충분한지 반환합니다
 */
bool User::isEnoughCoin(int i) {
    
    return getCoin() >= i;
}

/**
 * 클리어한 스테이지를 설정합니다
 */
void User::setClearStageSeq(int stageSeq) {
    
    if( stageSeq <= getClearStageSeq() ) {
        return;
    }
    
    UserDefault::getInstance()->setIntegerForKey(USER_DEFAULT_KEY_CLEAR_STAGE_SEQUENCE, stageSeq);
    UserDefault::getInstance()->flush();
}

/**
 * 클리어한 스테이지를 반환합니다
 */
int User::getClearStageSeq() {
    
    return UserDefault::getInstance()->getIntegerForKey(USER_DEFAULT_KEY_CLEAR_STAGE_SEQUENCE, 0);
}

/**
 * 볼을 소유합니다
 */
void User::ownBallSkin(const string &ballId) {
    
    // 이미 소유했는지 체크
    if( isOwnedBallSkin(ballId) ) {
        return;
    }
    
    auto &ballSkins = instance->ballSkins;
    ballSkins.push_back(ballId);
    
    // update user default
    rapidjson::Document doc;
    doc.SetArray();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    for( auto ballSkin : ballSkins ) {
        doc.PushBack(SBJSON::value(ballSkin, allocator), allocator);
    }
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    string json = strbuf.GetString();
    
    UserDefault::getInstance()->setStringForKey(USER_DEFAULT_KEY_OWNED_BALL_SKINS, json);
    UserDefault::getInstance()->flush();
}

/**
 * 소유한 볼을 반환합니다
 */
StringList User::getOwnedBallSkins() {
    return instance->ballSkins;
}

bool User::isOwnedBallSkin(const string &ballId) {

    auto ballSkins = getOwnedBallSkins();
    
    for( auto ballSkin : ballSkins ) {
        if( ballSkin == ballId ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 볼스킨 설정
 */
void User::setSelectedBallSkin(const string &ballId) {
    
    if( getSelectedBallSkin() == ballId ) {
        return;
    }
    
    UserDefault::getInstance()->setStringForKey(USER_DEFAULT_KEY_SELECTED_BALL_SKIN, ballId);
    UserDefault::getInstance()->flush();
    
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(DIRECTOR_EVENT_UPDATE_BALL_SKIN);
}

string User::getSelectedBallSkin() {
    
    return UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_SELECTED_BALL_SKIN, "");
}

/**
 * 프렌드를 소유합니다
 */
void User::ownFriend(const string &friendId) {
    
    // 이미 소유했는지 체크
    if( isOwnedFriend(friendId) ) {
        return;
    }
    
    auto &friends = instance->friends;
    friends.push_back(friendId);
    
    // update user default
    rapidjson::Document doc;
    doc.SetArray();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    for( auto friendId : friends ) {
        doc.PushBack(SBJSON::value(friendId, allocator), allocator);
    }
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    string json = strbuf.GetString();
    Log::i("User::ownFriend: %s", json.c_str());
    
    UserDefault::getInstance()->setStringForKey(USER_DEFAULT_KEY_OWNED_FRIENDS, json);
    UserDefault::getInstance()->flush();
}

/**
 * 소유한 프렌즈를 반환합니다
 */
StringList User::getOwnedFriends() {
    return instance->friends;
}

bool User::isOwnedFriend(const string &friendId) {
    
    auto friends = getOwnedFriends();
    
    for( auto ownedFriendId : friends ) {
        if( ownedFriendId == friendId ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 프렌즈 덱을 설정합니다
 */
void User::setFriendsDeck(StringList friendsDeck) {
    
    CCASSERT(friendsDeck.size() <= 4, "User::setFriendsDeck error.");
    
    instance->friendsDeck = friendsDeck;
    
    // update user default
    rapidjson::Document doc;
    doc.SetArray();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    for( auto friendId : friendsDeck ) {
        doc.PushBack(SBJSON::value(friendId, allocator), allocator);
    }
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    string json = strbuf.GetString();
    Log::i("User::setFriendsDeck: %s", json.c_str());
    
    UserDefault::getInstance()->setStringForKey(USER_DEFAULT_KEY_FRIENDS_DECK, json);
    UserDefault::getInstance()->flush();
}

void User::setFriendsDeck(FriendDataList friendsDeck) {
 
    StringList friendIds;
    
    for( auto data : friendsDeck ) {
        friendIds.push_back(data.friendId);
    }
    
    setFriendsDeck(friendIds);
}

/**
 * 프렌즈 덱을 반환합니다
 */
FriendDataList User::getFriendsDeck() {
    
    /*
    string json = UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_FRIENDS_DECK, "");
    CCASSERT(json != "", "User::getFriendsDeck error.");
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    auto list = doc.GetArray();
    FriendDataList friends;
    
    for( int i = 0; i < list.Size(); ++i ) {
        friends.push_back(Database::getFriend(list[i].GetString()));
    }
    */
    
    FriendDataList friends;
    
    /*
    vector<string> friendIds({
        "friend_0001", "friend_0004", "friend_0005", "friend_0006",
    });
    
    for( string friendId : friendIds ) {
        friends.push_back(Database::getFriend(friendId));
    }
    */
    
    for( string friendId : instance->friendsDeck ) {
        friends.push_back(Database::getFriend(friendId));
    }
    
    return friends;
}

bool User::isExistFriendsDeck(const string &friendId) {
    
    for( auto deckFriendId : instance->friendsDeck ) {
        if( deckFriendId == friendId ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 광고 제거 아이템 보유 여부
 */
bool User::isOwnRemoveAdsItem() {
    
    return UserDefault::getInstance()->getBoolForKey(UserDefaultKey::OWN_REMOVE_ADS_ITEM, false);
}

void User::setOwnRemoveAdsItem(bool owned) {
    
    AdsHelper::getInstance()->setActiveBanner(!owned);
    AdsHelper::getInstance()->setActiveInterstitial(!owned);
    
    UserDefault::getInstance()->setBoolForKey(UserDefaultKey::OWN_REMOVE_ADS_ITEM, owned);
    UserDefault::getInstance()->flush();
}

/**
 * 광고 제거
 */
void User::removeAds() {

    setOwnRemoveAdsItem(true);
    AdsHelper::hideBanner();
}

/**
 * 리뷰 체크
 */
bool User::checkReview(float popupDelay) {
    
    if( !ReviewHelper::isReviewAlertEnabled() ) {
        return false;
    }
    
    auto showPopup = [=]() {
        firebase::Analytics::logEvent(FA_EVENT_REVIEW_POPUP);
        
        ReviewHelper::showReviewPopup([=]() {
            
            // 커스텀 팝업
            auto popup = ReviewPopup::create();
            popup->setOnGoListener([=]() {
                
                ReviewHelper::setReviewWrite(true);
                Application::getInstance()->openURL(GameConfiguration::getInstance()->getStoreUrl());
                
                popup->dismissWithAction();
            });
            SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
        });
    };
    
    if( popupDelay == 0 ) {
        showPopup();
    } else {
        SBDirector::postDelayed(SceneManager::getScene(), showPopup, popupDelay, true);
    }
    
    return true;
}

