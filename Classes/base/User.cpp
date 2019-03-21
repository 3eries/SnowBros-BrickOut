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

#define USER_DEFAULT_KEY_OWNED_BALL_SKINS               "OWNED_BALL_SKINS"
#define USER_DEFAULT_KEY_SELECTED_BALL_SKIN             "SELECTED_BALL_SKIN"

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
    
    // 볼 스킨 초기화
    initOwnedBallSkins();
    
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
    
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(DIRECTOR_EVENT_SELECTED_BALL);
}

string User::getSelectedBallSkin() {
    
    return UserDefault::getInstance()->getStringForKey(USER_DEFAULT_KEY_SELECTED_BALL_SKIN, "");
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

