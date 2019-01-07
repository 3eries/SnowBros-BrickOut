//
//  PausePopup.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "PausePopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "UserDefaultKey.h"

#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

PausePopup::PausePopup() : BasePopup(PopupType::PAUSE),
onClickMenuListener(nullptr) {
    
}

PausePopup::~PausePopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool PausePopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }

    // IAP 리스너
    {
        auto onRemoveAds = [=]() {
            // TODO:
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
    
    return true;
}

void PausePopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void PausePopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void PausePopup::initContentView() {
    
    BasePopup::initContentView();
    
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_pause.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 0));
    addContentChild(stoneBg);
    
    // title
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_pause.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 175));
    addContentChild(title);
    
    // 효과음
    auto effectBtn = SBToggleButton::create(DIR_IMG_COMMON + "RSP_btn_effect_off.png",
                                            DIR_IMG_COMMON + "RSP_btn_effect.png");
    effectBtn->setZoomScale(0.07f);
    effectBtn->setAnchorPoint(ANCHOR_M);
    effectBtn->setPosition(Vec2MC(-172, 44));
    addContentChild(effectBtn);
    
    auto audioEngine = SBAudioEngine::getInstance();
    
    effectBtn->setSelected(!audioEngine->isEffectMute());
    effectBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        audioEngine->setEffectMute(!isSelected);
        
        return false;
    });
    
    // 배경음
    auto bgmBtn = SBToggleButton::create(DIR_IMG_COMMON + "RSP_btn_bgm_off.png",
                                         DIR_IMG_COMMON + "RSP_btn_bgm.png");
    bgmBtn->setZoomScale(0.07f);
    bgmBtn->setAnchorPoint(ANCHOR_M);
    bgmBtn->setPosition(Vec2MC(0, 44));
    addContentChild(bgmBtn);
    
    bgmBtn->setSelected(!audioEngine->isBGMMute());
    bgmBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        audioEngine->setBGMMute(!isSelected);
        
        return false;
    });
    
    // 닫기 버튼
    /*
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "RSP_btn_go_back.png");
    closeBtn->setZoomScale(0.07f);
    closeBtn->setAnchorPoint(ANCHOR_MR);
    closeBtn->setPosition(Vec2TR(-66 + (100*0.5f), -62));
    addChild(closeBtn, 1);
    
    closeBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->performListener(Tag::RESUME);
    });
    */
    
    // 기타 메뉴
    {
        SBUIInfo infos[] = {
            SBUIInfo(Tag::MAIN,          ANCHOR_M,   Vec2MC(172, 44),    "RSP_btn_home.png"),
            SBUIInfo(Tag::REMOVE_ADS,    ANCHOR_M,   Vec2MC(0, -87),     "RSP_btn_remove_ads.png"),
            SBUIInfo(Tag::HOW_TO_PLAY,   ANCHOR_M,   Vec2MC(0, -174),    "RSP_btn_how_to_play.png"),
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto btn = SBButton::create(DIR_IMG_COMMON + info.file);
            btn->setZoomScale(0.07f);
            info.apply(btn);
            addContentChild(btn, info.zOrder, info.tag);
            
            btn->setOnClickListener([=](Node*) {
                
                SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
                this->performListener((Tag)info.tag);
            });
        }
        
        if( User::isOwnRemoveAdsItem() ) {
            getContentView()->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
        }
    }
}

void PausePopup::performListener(Tag tag) {
    
    retain();
    onClickMenuListener(tag);
    release();
}

/**
 * 등장 연출
 */
void PausePopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void PausePopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
     // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void PausePopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->performListener(Tag::RESUME);
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
}
