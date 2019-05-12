//
//  StageClearPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#include "StageClearPopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"
#include "../GameDefine.h"

#include "BannerView.hpp"
#include "UserCoinView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;

StageClearPopup* StageClearPopup::create(const StageData &stage, BrickDataList bricks, int rewardCoin) {
    
    auto popup = new StageClearPopup();
    
    if( popup && popup->init(stage, bricks, rewardCoin) ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

StageClearPopup::StageClearPopup() : BasePopup(PopupType::STAGE_CLEAR),
onFriendsListener(nullptr),
onNextListener(nullptr) {
    
}

StageClearPopup::~StageClearPopup() {
}

bool StageClearPopup::init(const StageData &stage, BrickDataList brickDatas, int rewardCoin) {
    
    if( !BasePopup::init() ) {
        return false;
    }

    this->stage = stage;
    this->rewardCoin = rewardCoin;
    
    // 브릭 리스트 초기화
    bossBrickDatas = SBCollection::find(brickDatas, [](const BrickData &b) -> bool {
        return b.isBoss();
    });
    
    normalBrickDatas = SBCollection::find(brickDatas, [](const BrickData &b) -> bool {
        return b.type == BrickType::NORMAL && b.is1x1();
    });
    
    // UI 초기화
    initBg();
    initBrick();
    initRewardCoin();
    initTopMenu();
    initBottomMenu();
    
    // 배너
    addChild(BannerView::create());
    
    // 코인 획득
    userCoinView->setUpdateLocekd(true);
    User::addCoin(rewardCoin);
    userCoinView->setUpdateLocekd(false);
    
    return true;
}

void StageClearPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void StageClearPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void StageClearPopup::initContentView() {
    
    BasePopup::initContentView();
}

/**
 * 배경 초기화
 */
void StageClearPopup::initBg() {
    
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2MC(0, 16), DIR_IMG_STAGE_CLEAR + "clear_text_clear.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(info.file);
        info.apply(spr);
        addContentChild(spr);
    }
    
    // 스테이지
    string stageStr = stage.isWorldLastStage ? "B" : TO_STRING(stage.stage);
    
    auto stageLabel = Label::createWithBMFont(FONT_WORLD_SILVER,
                                              STR_FORMAT("W %d-%s", stage.world, stageStr.c_str()),
                                              TextHAlignment::CENTER);
    stageLabel->setAnchorPoint(ANCHOR_M);
    stageLabel->setPosition(Vec2MC(0, 92));
    addContentChild(stageLabel);
    
    // 스코어
    // TODO:
    // SCORE font:sabo size:50 color:150,150,150 stroke:4 Vec2BC(1, 141) , Size(302, 46)
    auto scoreLabel = Label::createWithTTF("SCORE: 123", FONT_SABO, 50, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2BC(0, 141));
    scoreLabel->setTextColor(Color4B(150,150,150,255));
    scoreLabel->enableOutline(Color4B::BLACK, 4);
    addContentChild(scoreLabel);
}

/**
 * 브릭 초기화
 */
void StageClearPopup::initBrick() {
    
    auto data = (bossBrickDatas.size() > 0) ? bossBrickDatas[0] : normalBrickDatas[0];
    
    auto bg = Sprite::create(ContentResourceHelper::getBrickBackgroundFile(data, false, 1));
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,250));
    addContentChild(bg);
    
    auto brick = SBAnimationSprite::create(BRICK_ANIMATION(data, BrickImageType::IDLE));
    brick->setAnchorPoint(ANCHOR_M);
    brick->setPosition(Vec2MC(bg->getContentSize(), 0, 0));
    brick->runAnimation();
    bg->addChild(brick);
    
    // 스탬프
    auto stamp = Sprite::create(DIR_IMG_STAGE_CLEAR + "clear_stamp_win.png");
    stamp->setAnchorPoint(ANCHOR_M);
    stamp->setPosition(Vec2MR(bg->getContentSize(), 12, -25));
    stamp->setScale(data.is1x1() ? 0.7f : 1);
    bg->addChild(stamp);
    
    // 스탬프 연출
    stamp->setScale(0.8f);
    stamp->runAction(Sequence::create(ScaleTo::create(0.1f, 1.1f),
                                      ScaleTo::create(0.1f, 0.95f),
                                      ScaleTo::create(0.1f, 1.0f),
                                      nullptr));
}

/**
 * 획득 코인 초기화
 */
void StageClearPopup::initRewardCoin() {
    
    StringList coinIconAnimFiles;
    coinIconAnimFiles.push_back(DIR_IMG_COMMON + "common_icon_coin_big.png");
    
    for( int i = 2; i <= 6; ++i ) {
        coinIconAnimFiles.push_back(DIR_IMG_COMMON + STR_FORMAT("common_icon_coin_big%d.png", i));
    }
    
    auto coinIcon = SBAnimationSprite::create(coinIconAnimFiles, 0.1f);
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(Vec2BC(-119, 408));
    coinIcon->runAnimation();
    addContentChild(coinIcon);
    
    auto coinLabel = Label::createWithBMFont(FONT_COIN_BLUE, "x 0", TextHAlignment::LEFT);
    coinLabel->setAnchorPoint(ANCHOR_ML);
    coinLabel->setPosition(Vec2BC(51 - 224*0.5f, 406));
    addContentChild(coinLabel);
    
    // 가운데 정렬
    SBNodeUtils::alignCenterHorizontal(vector<Node*>({ coinIcon, coinLabel }));
    
    // 코인 획득 연출
    // Step 1. 숫자 증가
    auto onChanged = [=](float value) {
        
        int i = (int)value;
        coinLabel->setString(STR_FORMAT("x %d", i));
    };
    
    auto numberAction = ActionFloat::create(0.3f, 0, rewardCoin, onChanged);
    auto callFunc = CallFunc::create([=]() {
        
        // Step 2. 아이콘이 유저 코인 영역으로 날아감
        auto effect = Sprite::create(coinIconAnimFiles[0]);
        effect->setAnchorPoint(coinIcon->getAnchorPoint());
        effect->setPosition(coinIcon->getPosition());
        this->addChild(effect, SBZOrder::TOP);
        
        auto pos = userCoinView->getCoinLabel()->getPosition();
        
        ccBezierConfig cfg;
        cfg.controlPoint_1 = effect->getPosition();
        cfg.controlPoint_2 = Vec2(pos.x - 70, effect->getPosition().y + 170);
        cfg.endPosition    = pos;
        
        auto bezier = BezierTo::create(0.8f, cfg);
        auto scale = ScaleTo::create(0.8f, 0.5f);
        auto callFunc = CallFunc::create([=]() {
            userCoinView->updateCoin(true);
        });
        effect->runAction(Sequence::create(Spawn::create(bezier, scale, nullptr), callFunc,
                                           RemoveSelf::create(),
                                           nullptr));
    });
    coinLabel->runAction(Sequence::create(numberAction, callFunc, nullptr));
}

/**
 * 상단 메뉴 초기화
 */
void StageClearPopup::initTopMenu() {
    
    // 유저 코인
    userCoinView = UserCoinView::create(UserCoinView::Type::NORMAL);
    addChild(userCoinView, SBZOrder::BOTTOM);
    
    // 프렌즈
    auto friendsBtn = SBButton::create(DIR_IMG_STAGE_CLEAR + "clear_btn_friend.png");
    friendsBtn->setZoomScale(ButtonZoomScale::HARD);
    friendsBtn->setAnchorPoint(ANCHOR_M);
    friendsBtn->setPosition(Vec2TR(-60, -158));
    addContentChild(friendsBtn);
    
    friendsBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onFriendsListener);
    });
}

/**
 * 하단 메뉴 초기화
 */
void StageClearPopup::initBottomMenu() {
    
    // 다음 스테이지 이동 버튼
    auto nextBtn = SBButton::create(DIR_IMG_STAGE_CLEAR + "clear_btn_next_stage.png");
    nextBtn->setZoomScale(ButtonZoomScale::WEAK);
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setPosition(Vec2BL(196, 265));
    addContentChild(nextBtn);
    
    nextBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onNextListener);
    });
    
    // 광고 보기 버튼
    auto adBtn = Button::create(DIR_IMG_STAGE_CLEAR + "clear_btn_ad.png");
    adBtn->setZoomScale(0);
    adBtn->setAnchorPoint(ANCHOR_M);
    adBtn->setPosition(Vec2BR(-196, 265));
    addContentChild(adBtn);
    
    adBtn->addClickEventListener([=](Ref*) {
        
    });
    
    // coin icon
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(adBtn->convertToNodeSpace(Vec2BR(-270, 223)));
    adBtn->addChild(coinIcon);
    
    // coin label
    auto coinLabel = Label::createWithTTF(STR_FORMAT("+%d", rewardCoin*2), FONT_COMMODORE, 48, Size::ZERO,
                                          TextHAlignment::LEFT, TextVAlignment::CENTER);
    coinLabel->setAnchorPoint(ANCHOR_ML);
    coinLabel->setPosition(adBtn->convertToNodeSpace(Vec2BR(-167 - 149*0.5f, 223)));
    coinLabel->setTextColor(Color4B::WHITE);
    coinLabel->enableOutline(Color4B(0,0,0,255), 4);
    adBtn->addChild(coinLabel);
    
    // 코인 UI를 버튼의 가운데로 정렬
    SBNodeUtils::alignCenterHorizontal(adBtn->getContentSize(), vector<Node*>({ coinIcon, coinLabel }));
    
    // bonus badge
    auto badge = Sprite::create(DIR_IMG_STAGE_CLEAR + "clear_badge_bonus.png");
    badge->setAnchorPoint(ANCHOR_M);
    badge->setPosition(adBtn->convertToNodeSpace(Vec2BR(-90, 346)));
    adBtn->addChild(badge);
}

/**
 * 등장 연출
 */
void StageClearPopup::runEnterAction(SBCallback onFinished) {
    
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
void StageClearPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
    
    // 배경 fade out
    /*
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);

    runBackgroundFadeOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, FADE_DURATION);
     */
}

/**
 * 등장 연출 완료
 */
void StageClearPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
