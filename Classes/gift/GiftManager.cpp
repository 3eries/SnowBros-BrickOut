//
//  GiftManager.cpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#include "GiftManager.hpp"

#include "Define.h"
#include "RankingManager.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"

#include "CommonLoadingBar.hpp"

#include "ui/GiftBox.hpp"
#include "ui/GiftPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static const string SCHEDULER_CHECK_GIFT         = "GIFT_MGR_SCHEDULER_CHECK_GIFT";
static const string SCHEDULER_GIFT_CHECK_ENABLED = "GIFI_MGR_SCHEDULER_GIFT_CHECK_ENABLED";

#define BOOST_REQUIRED_SCORE                     100
#define BOOST_MAX_SCORE                          1000

static GiftManager *instance = nullptr;

GiftManager* GiftManager::getInstance() {
    
    if( !instance ) {
        instance = new GiftManager();
    }
    
    return instance;
}

void GiftManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

GiftManager::GiftManager() :
checkInterval(0.5f) {
    
    GiftType types[] = {
        GiftType::BOOST,
    };
    
    for( auto type : types ) {
        auto gift = new Gift();
        gift->type = type;
        
        gifts[type] = gift;
    }
}

GiftManager::~GiftManager() {
    
    AdsHelper::getInstance()->getEventDispatcher()->removeListener(this);
    PopupManager::getInstance()->removeListener(this);
    SBScheduleHelper::unschedule(this);
    
    for( auto it = gifts.begin(); it != gifts.end(); ++it ) {
        auto gift = it->second;
        CC_SAFE_RELEASE_NULL(gift);
    }
    
    gifts.clear();
}

/**
 * 매니저 초기화
 */
void GiftManager::init() {
    
    initAdListener();
    initPopupListener();
    
    // 선물 체크 스케줄러 실행
    SBScheduleHelper::schedule([=](float dt) {
        
        for( auto it = gifts.begin(); it != gifts.end(); ++it ) {
            auto gift = it->second;
            
            if( !gift->isEnabled || !gift->isCheckEnabled ) {
                continue;
            }
            
            this->checkGift(gift->type);
        }
        
    }, getInstance(), checkInterval, SCHEDULER_CHECK_GIFT);
}

/**
 * 광고 리스너 초기화
 */
void GiftManager::initAdListener() {
    
    auto listener = RewardedVideoAdListener::create();
    listener->setTarget(this);
    listener->setForever(true);

    listener->onAdOpened = [=]() {
        
        this->removeGiftBox(GiftType::BOOST);
        this->setCheckEnabled(GiftType::BOOST, false);
    };
    
    listener->onAdClosed = [=]() {
        
        this->setCheckEnabled(GiftType::BOOST, true);
    };
    
    AdsHelper::getInstance()->getEventDispatcher()->addListener(listener);
}

/**
 * 팝업 리스너 초기화
 */
void GiftManager::initPopupListener() {
 
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType type) {
        
        auto popup = (BasePopup*)sender;
        
        switch( type ) {
            case PopupEventType::ENTER: {
            } break;
                
            case PopupEventType::EXIT_ACTION: {
            } break;
                
            case PopupEventType::EXIT: {
            } break;
                
            default: break;
        }
    };
    
    PopupManager::getInstance()->addListener(listener);
}

/**
 * 선물 활성화 여부를 설정합니다
 */
void GiftManager::setEnabled(bool isEnabled) {
 
    auto &gifts = getInstance()->gifts;
    
    for( auto it = gifts.begin(); it != gifts.end(); ++it ) {
        auto gift = it->second;
        gift->isEnabled = isEnabled;
    }
}

void GiftManager::setEnabled(GiftType type, bool isEnabled) {
    
    auto gift = getGift(type);
    gift->isEnabled = isEnabled;
}

/**
 * 선물 체크 여부를 설정합니다
 */
void GiftManager::setCheckEnabled(bool isCheckEnabled) {
    
    auto &gifts = getInstance()->gifts;
    
    for( auto it = gifts.begin(); it != gifts.end(); ++it ) {
        auto gift = it->second;
        gift->isCheckEnabled = isCheckEnabled;
    }
}

void GiftManager::setCheckEnabled(GiftType type, bool isCheckEnabled) {
    
    auto gift = getGift(type);
    gift->isCheckEnabled = isCheckEnabled;
}

/**
 * 선물 박스 생성
 */
bool GiftManager::addGiftBox(GiftType type) {
    
    auto gift = getGift(type);
    
    if( gift->box ) {
        // 이미 박스가 존재함
        return false;
    }
    
    // int zOrder = SceneManager::isGameScene() ? ZOrder::POPUP_BOTTOM : ZOrder::GIFT_BOX;
    
    auto box = GiftBox::create(type);
    box->setOnClickListener([=](Node *sender) {
        getInstance()->onClickBox(type);
    });
    box->move(0.5f);
    SceneManager::getScene()->addChild(box, ZOrder::POPUP_BOTTOM);
    
    gift->box = box;
    gift->isCheckEnabled = false;
    
    return true;
}

/**
 * 선물 박스 제거
 */
bool GiftManager::removeGiftBox(GiftType type) {
    
    auto gift = getGift(type);
    
    if( !gift->box ) {
        // 박스 없음
        return false;
    }
    
    gift->box->remove();
    gift->box = nullptr;
    gift->isCheckEnabled = true;
    
    return true;
}

void GiftManager::removeAllGiftBox() {
    
    auto gifts = getInstance()->gifts;
    
    for( auto it : gifts ) {
        removeGiftBox(it.first);
    }
}

/**
 * 선물 팝업을 노출합니다
 */
void GiftManager::showPopup(GiftType type) {

    setCheckEnabled(type, false);
    
    switch( type ) {
        // 부스트 아이템
        case GiftType::BOOST: {
            // 보상 아이템 생성
            GiftRewardItem item;
            item.type = type;
            item.amount = 200;
            
            const int BEST_SCORE = RankingManager::getBestScore();
            
            // 400점 이상, 베스트 스코어의 52%
            // 1점 대 숫자 내림 처리
            if( BEST_SCORE >= 400 ) {
                int score = BEST_SCORE * 0.52f;
                score = (int)(score / 10.0f) * 10;
                item.amount = score;
            }
            
            item.amount = MIN(BOOST_MAX_SCORE, item.amount);
            
            // 팝업 생성
            auto popup = GiftPopup::create(item);
            SceneManager::getScene()->addChild(popup, ZOrder::POPUP_TOP);
            
            popup->runEnterAction();
            popup->setOnGetListener([=](Node*) {
                
//                popup->dismiss();
//                getInstance()->onGiftRewarded(item);
//
//                return;
                
                // 광고 노출
                auto loadingBar = CommonLoadingBar::create();
                loadingBar->setUIDelay(0.1f);
                loadingBar->show();

                auto listener = RewardedVideoAdListener::create();
                listener->setTarget(getInstance());
                
                // 로딩 실패
                listener->onAdFailedToLoad = [=](int error) {
                    firebase::Analytics::logEvent("debug_continue_rewarded_ad");
                };
                // 광고 Open
                listener->onAdOpened = [=]() {
                    loadingBar->dismissWithDelay(0);
                    popup->dismiss();
                };
                // 광고 Close
                listener->onAdClosed = [=]() {
                    if( listener->isRewarded() ) {
                        SBDirector::postDelayed(SceneManager::getScene(), [=]() {
                            getInstance()->onGiftRewarded(item);
                        }, 0.4f, true);
                    }
                };
                
                AdsHelper::getInstance()->showRewardedVideo(listener);
            });
            
            popup->setOnDismissListener([=](Node*) {
                
                SBScheduleHelper::unschedule(getInstance(), SCHEDULER_GIFT_CHECK_ENABLED);
                SBScheduleHelper::scheduleOnce([=](float dt) {
                    setCheckEnabled(type, true);
                }, getInstance(), 2.5f, SCHEDULER_GIFT_CHECK_ENABLED);
            });
            
        } break;
            
        default: break;
    }
}

/**
 * 선물 체크
 */
void GiftManager::checkGift(GiftType type) {

    // CCLOG("GiftManager::checkGift: %s", giftToString(type).c_str());
    
    auto gift = getGift(type);
    
    switch( type ) {
        // 부스트 아이템
        case GiftType::BOOST: {
            // 베스트 스코어 미달
            if( RankingManager::getBestScore() < BOOST_REQUIRED_SCORE ) {
                break;
            }
            
            // 동영상 광고 없음
            if( !AdsHelper::isRewardedVideoLoaded() ) {
                break;
            }
            
            addGiftBox(type);
            
        } break;
            
        default: break;
    }
}

/**
 * 타입에 해당하는 선물을 반환합니다
 */
GiftManager::Gift* GiftManager::getGift(GiftType type) {
    
    auto &gifts = getInstance()->gifts;
    auto it = gifts.find(type);
    
    if( it != gifts.end() ) {
        return it->second;
    }
    
    CCASSERT(false, "GiftManager::getGift error: invalid type.");
    return nullptr;
}

/**
 * 선물 박스 클릭
 */
void GiftManager::onClickBox(GiftType type) {
    
    CCLOG("GiftManager::onClickBox: %s", giftToString(type).c_str());
    
    // firebase event
    firebase::Analytics::EventParams params;
    params[FA_EVENT_PARAM_ITEM_NAME] = Value(giftToString(type));
    
    firebase::Analytics::logEvent(FA_EVENT_ITEM_BOX_CLICK, params);
    
    // 리스너 실행
    for( auto listener : listeners ) {
        if( listener->onClickBox ) {
            listener->onClickBox(type);
        }
    }
    
    // 박스 제거
    removeGiftBox(type);
    
    // 타입에 따른 이벤트 처리
    switch( type ) {
        case GiftType::BOOST: {
            showPopup(type);
        } break;
            
        default: break;
    }
}

/**
 * 선물 보상
 */
void GiftManager::onGiftRewarded(const GiftRewardItem &item) {
    
    CCLOG("GiftManager::onGiftRewarded: %s", giftToString(item.type).c_str());
    
    // firebase event
    firebase::Analytics::EventParams params;
    params[FA_EVENT_PARAM_ITEM_NAME] = Value(giftToString(item.type));
    
    firebase::Analytics::logEvent(FA_EVENT_ITEM_USE, params);
    
    // 리스너 실행
    for( auto listener : listeners ) {
        if( listener->onRewarded ) {
            listener->onRewarded(item);
        }
    }
}

/**
 * 리스너 등록
 */
void GiftManager::addListener(GiftListener *listener) {
    
    CCASSERT(listener != nullptr, "GiftManager::addListener error: listener must be not null");
    
    if( !listeners.contains(listener) ) {
        listeners.pushBack(listener);
    }
}

/**
 * 리스너 제거
 */
void GiftManager::removeListener(GiftListener *listener) {
    
    CCASSERT(listener != nullptr, "GiftManager::addListener error: listener must be not null");
    
    if( listeners.contains(listener) ) {
        listeners.eraseObject(listener);
    }
}

void GiftManager::removeListener(Ref *target) {
    
    CCASSERT(target != nullptr, "GiftManager::removeListener error: target must be not null");
    
    auto removeListeners = SBCollection::find(listeners, [=](GiftListener *listener) -> bool {
        return listener->getTarget() == target;
    });
    
    for( auto listener : removeListeners ) {
        listeners.eraseObject(listener);
    }
}
