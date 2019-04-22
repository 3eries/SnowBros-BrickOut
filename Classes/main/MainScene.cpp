//
//  MainScene.cpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "GiftManager.hpp"
#include "ContentManager.hpp"
#include "UIHelper.hpp"
#include "TestList.hpp"

#include "../game/GameManager.hpp"
#include "../game/object/friend/Friend.hpp"

#include "CommonLoadingBar.hpp"
#include "BannerView.hpp"
#include "UserCoinView.hpp"
#include "ExitAlertPopup.hpp"
#include "SettingPopup.hpp"

#include "demo/DemoView.hpp"

#include "BallSkinPopup.hpp"
#include "FriendsPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
}

bool MainScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    firebase::Analytics::setScreenName(FA_SCREEN_MAIN);
    
    initBg();
    initMenu();
    
    updateFriendsUI();
    
    initIAPListener();
    initGiftListener();
    initPopupListener();
    
    // 덱 변경 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_FRIENDS_DECK, [=](EventCustom *event) {
        this->updateFriendsUI();
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void MainScene::onEnter() {
    
    BaseScene::onEnter();
    
    // bgm
    SBAudioEngine::stopBGM();
    
    /*
    scheduleOnce([=](float) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
     */
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
    }
}

void MainScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // 선물 활성화
    GiftManager::setEnabled(GiftType::BOOST, true);
    GiftManager::setCheckEnabled(true);
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        ConsentForm::create()
        ->setTitle("Important")
        ->setMessage("We don't collect your personal data,\nhowever our ads suppliers may use\nthem to personalise ads for you. You'll\nfind links to our partner's privacy\npolicies in our Privacy Policy.\n\nYou can opt-out ads tracking from\ngame & device settings.")
        ->setPolicyUrl("http://www.3eries.com/privacy_policy")
        ->show();
    }
}

void MainScene::onExit() {
    
    BaseScene::onExit();
}

bool MainScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    // 앱 종료 알림 팝업 생성
    if( PopupManager::getInstance()->getPopupCount() == 0 ) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        auto popup = (ExitAlertPopup*)PopupManager::show(PopupType::EXIT_APP);
        popup->setOnExitAppListener([=]() {
            SBSystemUtils::exitApp();
        });
        
        return true;
    }
    
    return false;
}

/**
 * 하단 프렌즈 UI 업데이트
 */
void MainScene::updateFriendsUI() {
    
    // 기존 프렌즈 제거
    for( auto friendImage : friends ) {
        friendImage->removeFromParent();
    }
    friends.clear();
    
    // 프렌즈 초기화
    auto deck = User::getFriendsDeck();
    int slotIndexes[] = { 1,3,0,4 };
    
    for( int i = 0; i < deck.size(); ++i ) {
        string file = ContentResourceHelper::getFriendAnimationFile(deck[i].friendId);
        int slotIndex = slotIndexes[i];
        
        auto image = spine::SkeletonAnimation::createWithJsonFile(file);
        image->setAnchorPoint(Vec2::ZERO);
        image->setPosition(Friend::getSlotPosition(slotIndex));
        image->setAnimation(0, FRIEND_ANIM_NAME_IDLE, true);
        image->setScaleX(slotIndex >= 3 ? -1 : 1);
        addChild(image);
        
        friends.push_back(image);
    }
}

/**
 * 버튼 클릭
 */
void MainScene::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 크레딧
        case Tag::BTN_TITLE: {        
            firebase::Analytics::logEvent(FA_EVENT_CREDIT);
            
            auto popup = PopupManager::show(PopupType::CREDIT);
            popup->setLocalZOrder(ZOrder::POPUP_TOP);
            popup->setOnDismissListener([=](Node*) {
            });
        } break;
            
        // 게임 시작
        case Tag::BTN_START: {
            removeChildByTag(Tag::TAP_TO_START);
            
#if ENABLE_TEST_MENU
            GameManager::getInstance()->setStartStage(TEST_HELPER->getStartStage());
#endif
            replaceScene(SceneType::GAME);
        } break;
            
        // 설정
        case Tag::BTN_SETTING: {
            showSettingPopup();
        } break;
            
        // 볼 스킨
        case Tag::BTN_BALL_SKIN: {
            showBallSkinPopup();
        } break;
            
        // 프렌즈
        case Tag::BTN_FRIENDS: {
            showFriendsPopup();
        } break;

        // 상점
        case Tag::BTN_SHOP: {
            PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
        } break;
            
        // 리더보드
        case Tag::BTN_LEADERBOARD: {
            if( superbomb::PluginPlay::isSignedIn() ) {
                superbomb::PluginPlay::showAllLeaderboards();
            } else {
                superbomb::PluginPlay::signIn();
            }
        } break;
            
        // test
        case Tag::BTN_TEST: {
            // Director::getInstance()->pushScene(TestList::create());
            replaceScene(SceneType::TEST);
        } break;
            
        default:
            break;
    }
}

/**
 * 설정 팝업 노출
 */
void MainScene::showSettingPopup() {
    
    auto popup = SettingPopup::create();
    popup->setOnClickMenuListener([=](SettingPopup::Tag tag) {
        
        switch( tag ) {
            // restore purchase
            case SettingPopup::Tag::RESTORE_PURCHASE: {
                if( iap::IAPHelper::isReady() ) {
                    iap::IAPHelper::restore(nullptr);
                }
            } break;
                
            // remove ads
            case SettingPopup::Tag::REMOVE_ADS: {
                if( iap::IAPHelper::isReady() ) {
                    auto loadingBar = CommonLoadingBar::create();
                    loadingBar->setUIDelay(0.1f);
                    loadingBar->show();
                    
                    auto listener = iap::PurchaseListener::create();
                    listener->setTarget(this);
                    listener->onPurchased = [=](const iap::Item &item) {
                    };
                    listener->onFinished = [=](bool result) {
                        loadingBar->dismissWithDelay(0);
                    };
                    
                    iap::IAPHelper::purchaseRemoveAds(listener);
                }
            } break;
                
            // how to play
            case SettingPopup::Tag::HOW_TO_PLAY: {
            } break;
                
            default: break;
        }
    });
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

/**
 * 볼 스킨 팝업 노출
 */
void MainScene::showBallSkinPopup() {
    
    auto popup = BallSkinPopup::create();
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

/**
 * 프렌즈 팝업 노출
 */
void MainScene::showFriendsPopup() {
    
    auto popup = FriendsPopup::create();
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

/**
 * 배경 초기화
 */
void MainScene::initBg() {
    
    addChild(LayerColor::create(Color4B::BLACK));
    
    auto demoView = DemoView::create();
    addChild(demoView);
    
    // 유저 코인
    addChild(UserCoinView::create(UserCoinView::Type::NORMAL), SBZOrder::BOTTOM);
    
    // 배너
    auto bannerView = BannerView::create();
    bannerView->setTag(Tag::BANNER);
    addChild(bannerView, SBZOrder::BOTTOM);
    
    // title
    auto title = SBButton::create(DIR_IMG_MAIN + "main_title.png");
    title->setZoomScale(0);
    title->setTag(Tag::BTN_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 162));
    addChild(title);
    
    // title->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
     
    // tap to start
    auto tapToStart = Sprite::create(DIR_IMG_MAIN + "main_tap_to_start.png");
    tapToStart->setTag(Tag::TAP_TO_START);
    tapToStart->setAnchorPoint(ANCHOR_M);
    tapToStart->setPosition(Vec2MC(0, -124));
    addChild(tapToStart);
    
    {
        auto blink = Blink::create(1.0f, 1);
        tapToStart->runAction(RepeatForever::create(blink));
    }
    
    auto btn = SBNodeUtils::createTouchNode();
    btn->setTag(Tag::BTN_START);
    btn->setAnchorPoint(ANCHOR_MT);
    btn->setPosition(Vec2MC(0, 350));
    addChild(btn);
    
    btn->addClickEventListener([=](Ref*) {
        this->onClick(btn);
    });
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {
    
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_SETTING,      ANCHOR_M,    Vec2TR(-46, -146),     DIR_IMG_MAIN + "main_btn_setting.png"),
        SBUIInfo(Tag::BTN_BALL_SKIN,    ANCHOR_M,    Vec2BC(-192, 274),     DIR_IMG_MAIN + "main_btn_ball_skin.png"),
        SBUIInfo(Tag::BTN_FRIENDS,      ANCHOR_M,    Vec2BC(-64, 274),      DIR_IMG_MAIN + "main_btn_friends.png"),
        SBUIInfo(Tag::BTN_SHOP,         ANCHOR_M,    Vec2BC(64, 274),       DIR_IMG_MAIN + "main_btn_shop.png"),
        SBUIInfo(Tag::BTN_LEADERBOARD,  ANCHOR_M,    Vec2BC(192, 274),      DIR_IMG_MAIN + "main_btn_leader_board.png"),
#if ENABLE_TEST_MENU
        SBUIInfo(Tag::BTN_TEST,         ANCHOR_M,    Vec2MC(0, 10),         DIR_IMG_COMMON + "RSP_btn_test.png"),
#endif
    };
    
    if( SBDirector::isPadResolution() ) {
        infos[0].file = SBStringUtils::replaceAll(infos[0].file, DIR_IMG_MAIN, DIR_IMG_MAIN_IPAD);
    }
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(info.file);
        btn->setZoomScale(-0.07f);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
    }
    
#if ENABLE_TEST_MENU
    auto testBtn = getChildByTag(Tag::BTN_TEST);
    testBtn->setOpacity(255*0.1f);
#endif
    
    if( User::isOwnRemoveAdsItem() ) {
        // getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
    }
}

/**
 * 인앱 결제 리스너 초기화
 */
void MainScene::initIAPListener() {
    
    auto onRemoveAds = [=]() {
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
}

/**
 * 선물 리스너 초기화
 */
void MainScene::initGiftListener() {
    
    auto listener = GiftListener::create();
    listener->setTarget(this);
    listener->onRewarded = [=](const GiftRewardItem &item) {
        
        if( item.type == GiftType::BOOST ) {
            this->replaceScene(SceneType::GAME, GiftRewardItems({item}));
        }
    };
    
    GiftManager::getInstance()->addListener(listener);
}

/**
 * 팝업 리스너 초기화
 */
void MainScene::initPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType type) {
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        // 팝업 연출에 따른 메인화면 메뉴 처리
        // TODO:
    };
    
    PopupManager::getInstance()->addListener(listener);
}
