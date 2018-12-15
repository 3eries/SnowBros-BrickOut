//
//  GameScene.cpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "GameScene.hpp"

#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "GiftManager.hpp"
#include "UIHelper.hpp"

#include "GameView.hpp"

#include "ui/PausePopup.hpp"
#include "ui/ContinuePopup.hpp"
#include "ui/ResultPopup.hpp"

#include "RankingManager.hpp"
#include "CommonLoadingBar.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

GameScene* GameScene::create() {
    
    auto scene = new GameScene();
    
    if( scene && scene->init() ) {
        scene->autorelease();
        return scene;
    }
    
    delete scene;
    return nullptr;
}

GameScene::GameScene() :
gameView(nullptr) {
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    firebase::Analytics::setScreenName(FA_SCREEN_GAME);
    
    initBg();
    initBanner();
    initGameView();
    initMenu();
    initGameListener();
    initIAPListener();
    initGiftListener();
    
    return true;
}

void GameScene::onEnter() {
    
    BaseScene::onEnter();
    
    GameManager::onGameEnter();
    GameManager::onGameStart();
}

void GameScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // bgm
    SBAudioEngine::playBGM(DIR_SOUND + "bgm_stage_01.mp3");
    
//    scheduleOnce([=](float) {
//        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
//    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
}

void GameScene::onExit() {
    
    BaseScene::onExit();
}

void GameScene::cleanup() {
    
    BaseScene::cleanup();
}

bool GameScene::onApplicationEnterBackground() {
    
    if( !BaseScene::onApplicationEnterBackground() ) {
        return false;
    }
    
    // 게임 일시정지
    if( GameManager::isPlaying() ) {
        GameManager::onGamePause();
    }
    
    /*
    if( SceneManager::getSceneType() == SceneType::GAME ) {
        if(!GameManager::getInstance()->isGamePaused() &&
           !GameManager::getInstance()->isPreGameOver() &&
           !GameManager::getInstance()->isGameOver() ) {
            showPausePopup();
        }
    }
    */
    
    return true;
}

bool GameScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    // 게임 일시정지
    if( GameManager::isPlaying() && PopupManager::getInstance()->getPopupCount() == 0 ) {
        GameManager::onGamePause();
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        return true;
    }
    
    return false;
}

/**
 * 게임 진입
 */
void GameScene::onGameEnter() {
    
    firebase::Analytics::setScreenName(FA_SCREEN_GAME);
}

/**
 * 게임 퇴장
 */
void GameScene::onGameExit() {
}

/**
 * 게임 리셋
 */
void GameScene::onGameReset() {
 
    setSceneTouchLocked(false);
}

/**
 * 게임 시작
 */
void GameScene::onGameStart() {
}

/**
 * 게임 재시작
 */
void GameScene::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameScene::onGamePause() {
    
    setSceneTouchLocked(true);
    showPausePopup();
}

/**
 * 게임 재게
 */
void GameScene::onGameResume() {
    
    setSceneTouchLocked(false);
}

/**
 * 게임 오버
 */
void GameScene::onGameOver() {
    
    setSceneTouchLocked(true);
    
    SBDirector::postDelayed(this, [=]() {

        GameManager::onGameResult();
//        // 이어하기
//        if( GameManager::isContinuable() ) {
//            this->showContinuePopup();
//        }
//        // 게임 결과
//        else {
//            GameManager::onGameResult();
//        }
    }, GAME_RESULT_DELAY, true);
}

/**
 * 게임 이어하기
 */
void GameScene::onGameContinue() {
}

/**
 * 게임 결과
 */
void GameScene::onGameResult() {
    
    // firebase::Analytics::setScreenName(FA_SCREEN_GAME_RESULT);
    
    showResultPopup();
}

/**
 * 부스트 시작
 */
void GameScene::onBoostStart() {
}

/**
 * 부스트 종료
 */
void GameScene::onBoostEnd() {
}

/**
 * 스코어 변경
 */
void GameScene::onScoreChanged(int score) {
}

/**
 * 레벨 변경
 */
void GameScene::onLevelChanged(const LevelData &level) {
}

/**
 * Scene 전환
 */
void GameScene::replaceScene(SceneType type, const GiftRewardItems &items) {
    
    removeChildByTag(Tag::BANNER_LOADING);
    
    GameManager::onGameExit();
    GameManager::destroyInstance();
    
    BaseScene::replaceScene(type, items);
}

/**
 * 일시정지 팝업 노출
 */
void GameScene::showPausePopup() {
    
    CCASSERT(!PopupManager::getPopup(PopupType::PAUSE), "GameScene::showPausePopup invalid called.");
    
    auto popup = PausePopup::create();
    
    // 메뉴 클릭 리스너
    popup->setOnClickMenuListener([=](PausePopup::Tag tag) {
        
        switch( tag ) {
            // resume
            case PausePopup::Tag::RESUME: {
                popup->dismissWithAction([=]() {
                    GameManager::onGameResume();
                });
            } break;
                
            // main
            case PausePopup::Tag::MAIN: {
                this->replaceScene(SceneType::MAIN);
            } break;
                
            // remove ads
            case PausePopup::Tag::REMOVE_ADS: {
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
            case PausePopup::Tag::HOW_TO_PLAY: {
            } break;
                
            default: break;
        }
    });

    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

/**
 * 이어하기 팝업 노출
 */
void GameScene::showContinuePopup() {
    
    bool isAdsLoaded = AdsHelper::getInstance()->isRewardedVideoLoaded();
//    bool isAdsLoaded = false;
    
    auto popup = ContinuePopup::create(isAdsLoaded);
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
    
    // 팝업 종료
    popup->setOnDismissListener([=](Node*){
        
    });
    
    // 이어하기
    popup->setOnContinueListener([=]() {
        
        auto darkLayer = LayerColor::create(Color4B::BLACK);
        this->addChild(darkLayer, SBZOrder::TOP);
        
        auto continueWithAction = [=](FiniteTimeAction *firstAction) {
            
            SBDirector::getInstance()->setScreenTouchLocked(true);
            
            Vector<FiniteTimeAction*> actions;
            
            if( firstAction ) {
                actions.pushBack(firstAction);
            }
            
            actions.pushBack(CallFunc::create([=]() {
                GameManager::onGameContinue();
            }));
            actions.pushBack(DelayTime::create(0.1f));
            actions.pushBack(FadeOut::create(0.4f));
            actions.pushBack(CallFunc::create([=]() {
                SBDirector::getInstance()->setScreenTouchLocked(false);
            }));
            actions.pushBack(RemoveSelf::create());
            
            darkLayer->runAction(Sequence::create(actions));
        };

        // VIP or 광고 없음, 무료 이어하기
        if( User::isOwnRemoveAdsItem() || !isAdsLoaded ) {
            darkLayer->setOpacity(0);
            
            auto fadeIn = FadeIn::create(0.4f);
            auto callFunc = CallFunc::create([=]() {
                popup->dismiss();
            });
            auto firstAction = Sequence::create(fadeIn, callFunc, nullptr);
            continueWithAction(firstAction);
            
            return;
        }
        
        // 광고 있음, 팝업 퇴장 연출 후 광고 재생
        popup->dismissWithAction([=]() {
            
            auto loadingBar = CommonLoadingBar::create();
            loadingBar->setUIDelay(0.1f);
            loadingBar->show();
            
            // 동영상 광고 노출
            CCLOG("Continue showRewardedVideo");
            
            auto listener = RewardedVideoAdListener::create();
            listener->setTarget(this);
            
            // 로딩 실패
            listener->onAdFailedToLoad = [=](int error) {
                
                firebase::Analytics::logEvent("debug_continue_rewarded_ad");
            };
            // 보상 완료
            listener->onRewarded = [=](string type, int amount) {
            };
            // 광고 open
            listener->onAdOpened = [=]() {
                loadingBar->dismissWithDelay(0);
            };
            // 광고 close
            listener->onAdClosed = [=]() {
                
                if( listener->isRewarded() ) {
                    continueWithAction(nullptr);
                } else {
                    darkLayer->removeFromParent();
                    GameManager::onGameResult();
                }
            };
            
            AdsHelper::getInstance()->showRewardedVideo(listener);
        });
    });
    
    // 타임 오버
    popup->setOnTimeOutListener([=]() {
        GameManager::onGameResult();
    });
}

/**
 * 게임 결과 팝업 노출
 */
void GameScene::showResultPopup() {

    // 연출용 레이어 생성
    auto layer = SBNodeUtils::createTouchNode();
    addChild(layer, SBZOrder::TOP);
    
    layer->addChild(LayerColor::create(Color4B::BLACK));
    
    // 1st animation
    const int LEN = 13;
    vector<string> animFiles1;
    
    for( int i = 0; i < LEN; ++i ) {
        animFiles1.push_back(DIR_IMG_GAME + STR_FORMAT("ui_gameover01_%02d.png", i+1));
    }
    
    auto anim1 = SBAnimationSprite::create(animFiles1, 0.1f, 1);
    anim1->setAnchorPoint(ANCHOR_MB);
    anim1->setPosition(Vec2MC(0, 20));
    layer->addChild(anim1);
    
    // 2nd animation
    vector<string> animFiles2({
        DIR_IMG_GAME + "ui_gameover02_01.png",
        DIR_IMG_GAME + "ui_gameover02_02.png",
    });
    
    auto anim2 = SBAnimationSprite::create(animFiles2, 0.1f, 5);
    anim2->setAnchorPoint(ANCHOR_MT);
    anim2->setPosition(Vec2MC(0, -20));
    anim2->setVisible(false);
    layer->addChild(anim2);
    
    // run
    anim1->runAnimation([=](Node *sender) {
        
        anim2->setVisible(true);
        anim2->runAnimation([=](Node *sender) {
            
            anim1->setVisible(false);
            anim2->setVisible(false);
            
            this->replaceScene(SceneType::MAIN);
            // onCompletedListener();
        });
    });
    
//    // 선물 활성화
//    // GiftManager::setEnabled(GiftType::BOOST, true);
//    // GiftManager::setCheckEnabled(true);
//
//    // 팝업
//    auto popup = ResultPopup::create(GameManager::getScore());
//    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
//
//        switch( eventType ) {
//            case PopupEventType::ENTER_ACTION_FINISHED: {
//                auto onAdFinished = [=]() {
//                    // 광고 닫힌 후 리뷰 유도
//                    this->checkReview();
//                };
//
//                // 전면 광고 로딩됨, 1초 후 노출
//                if( !User::isOwnRemoveAdsItem() && AdsHelper::isInterstitialLoaded() ) {
//                    SBDirector::postDelayed(this, [=]() {
//                        auto listener = AdListener::create(AdType::INTERSTITIAL);
//                        listener->setTarget(this);
//                        listener->onAdOpened = [=]() {
//                        };
//                        listener->onAdClosed = onAdFinished;
//                        AdsHelper::getInstance()->showInterstitial(listener);
//                    }, 1.0f, true);
//                }
//                // 전면 광고 로딩되지 않음
//                else {
//                    onAdFinished();
//                }
//
//            } break;
//
//            case PopupEventType::EXIT_ACTION_FINISHED: {
//            } break;
//
//            default: break;
//        }
//    });
//    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_BOTTOM);
}

/**
 * 리뷰 체크
 * 조건 충족 시 리뷰 작성 알림 팝업 노출
 */
void GameScene::checkReview() {
    
    if( GameManager::getScore() >= 400 ||       // 스코어
        GameManager::getPlayCount() >= 15 ) {   // 플레이 횟수
        
        User::checkReview(0.5f);
    }
}

/**
 * 버튼 클릭
 */
void GameScene::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        // 게임 재시작
        case Tag::BTN_START: {
            // 방법 1. onGameRestart
            GameManager::onGameRestart();
            
            // 방법 2. replace scene
            // replaceScene(SceneType::GAME);
        } break;
            
        case Tag::BTN_PAUSE: {
            GameManager::onGamePause();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
}

/**
 * 배너 초기화
 */
void GameScene::initBanner() {
    
#if SB_PLUGIN_USE_ADS
    // 배너 로딩중 이미지
    // 로딩된 배너 광고의 ZOrder가 높기 때문에 로딩중 이미지를 따로 제거하지 않는다.
    if( User::isOwnRemoveAdsItem() ) {
        return;
    }
    
    const Size bannerSize(SB_WIN_SIZE.width, AdsHelper::getBannerHeight());
    
    auto bannerLayer = Node::create();
    bannerLayer->setTag(Tag::BANNER_LOADING);
    bannerLayer->setAnchorPoint(ANCHOR_MT);
    bannerLayer->setPosition(Vec2TC(0, 0));
    bannerLayer->setContentSize(bannerSize);
    addChild(bannerLayer, INT_MAX);
    
    {
        auto bg = Sprite::create(DIR_IMG_GAME + "RSP_ad_top_bg.png");
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(Vec2MC(bannerSize, 0, 0));
        bg->setScaleX(bannerSize.width / bg->getContentSize().width);
        bg->setScaleY(bannerSize.height / bg->getContentSize().height);
        bannerLayer->addChild(bg);
        
        auto text = Sprite::create(DIR_IMG_GAME + "RSP_ad_top_text.png");
        text->setAnchorPoint(ANCHOR_M);
        text->setPosition(Vec2MC(bannerSize, 0, 0));
        bannerLayer->addChild(text);
    }
#endif
}

/**
 * 게임뷰 초기화
 */
void GameScene::initGameView() {
    
    gameView = GameView::create();
    addChild(gameView);
    
    GameManager::getInstance()->setView(gameView);
}

/**
 * 메뉴 초기화
 */
void GameScene::initMenu() {
    
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_PAUSE,      ANCHOR_MR,   Vec2TR(-10, -37),     "btn_pause.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        info.apply(btn);
        btn->setZoomScale(0.05f);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
    }
}

/**
 * 게임 리스너 초기화
 */
void GameScene::initGameListener() {
 
    auto listener = GameEventListener::create(this);
    listener->onGameEnter            = CC_CALLBACK_0(GameScene::onGameEnter, this);
    listener->onGameExit             = CC_CALLBACK_0(GameScene::onGameExit, this);
    listener->onGameReset            = CC_CALLBACK_0(GameScene::onGameReset, this);
    listener->onGameStart            = CC_CALLBACK_0(GameScene::onGameStart, this);
    listener->onGameRestart          = CC_CALLBACK_0(GameScene::onGameRestart, this);
    listener->onGamePause            = CC_CALLBACK_0(GameScene::onGamePause, this);
    listener->onGameResume           = CC_CALLBACK_0(GameScene::onGameResume, this);
    listener->onGameOver             = CC_CALLBACK_0(GameScene::onGameOver, this);
    listener->onGameContinue         = CC_CALLBACK_0(GameScene::onGameContinue, this);
    listener->onGameResult           = CC_CALLBACK_0(GameScene::onGameResult, this);
    listener->onBoostStart           = CC_CALLBACK_0(GameScene::onBoostStart, this);
    listener->onBoostEnd             = CC_CALLBACK_0(GameScene::onBoostEnd, this);
    listener->onScoreChanged         = CC_CALLBACK_1(GameScene::onScoreChanged, this);
    listener->onLevelChanged         = CC_CALLBACK_1(GameScene::onLevelChanged, this);
    GameManager::getEventDispatcher()->addListener(listener);
}

/**
 * IAP 리스너 초기화
 */
void GameScene::initIAPListener() {
    
    auto onRemoveAds = [=]() {
        // 배너 제거
        removeChildByTag(Tag::BANNER_LOADING);
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
void GameScene::initGiftListener() {
    
    auto listener = GiftListener::create();
    listener->setTarget(this);
    listener->onRewarded = [=](const GiftRewardItem &item) {
        
        if( item.type == GiftType::BOOST ) {
            this->replaceScene(SceneType::GAME, GiftRewardItems({item}));
        }
    };
    
    GiftManager::getInstance()->addListener(listener);
}
