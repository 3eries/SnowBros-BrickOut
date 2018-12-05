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

