//
//  BoostPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 12/05/2019.
//

#include "BoostPopup.hpp"

#include "Define.h"
#include "User.hpp"

#include "ContentManager.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"

#include "BannerView.hpp"
#include "UserCoinView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

#define LIST_CELL_SIZE          Size(608,134)

BoostPopup* BoostPopup::create() {
    
    auto popup = new BoostPopup();
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

BoostPopup::BoostPopup() : BasePopup(PopupType::BOOST),
onBoostListener(nullptr),
onClosedListener(nullptr),
currentWorldIndex(-1),
selectedCell(nullptr) {
}

BoostPopup::~BoostPopup() {
}

bool BoostPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    // 부스트 가능한 최대 스테이지 설정
    auto stageSeq = User::getClearStageSeq();
    
    if( !Database::getStageBySeq(stageSeq+1).isNull() ) {
        stageSeq += 1;
    }
    
    boostMaxStage = Database::getStageBySeq(stageSeq);
    
    // UI 초기화
    initBg();
    initWorldView();
    
    updateWorld(boostMaxStage.world-1);
    
    return true;
}

void BoostPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void BoostPopup::dismiss() {
    
    BasePopup::dismiss();
}

void BoostPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(/*Color::POPUP_BG*/Color4B(0,0,0,255*0.8f));
}

void BoostPopup::initContentView() {
    
    BasePopup::initContentView();
}

/**
 * 월드 업데이트
 */
void BoostPopup::updateWorld(int idx) {
    
    this->currentWorldIndex = idx;
    
    auto world = Database::getWorlds()[idx];
    
    // 타이틀
    auto worldLabel = getChildByTag<Label*>(Tag::WORLD_LABEL);
    worldLabel->setString(STR_FORMAT("W %d", world.world));
    
    // 화살표
    getChildByTag(Tag::BTN_WORLD_PREV)->setVisible(idx > 0);
    getChildByTag(Tag::BTN_WORLD_NEXT)->setVisible(idx < worldViews.size()-1);
    
    // 리스트
    for( auto view : worldViews ) {
        view->setVisible(false);
        // view->scrollToItem(0, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
    }

    auto view = worldViews[idx];
    view->setVisible(true);
}

/**
 * 셀 선택
 */
void BoostPopup::onSelectedCell(Boost::ListCell *cell) {
    
    if( selectedCell == cell ) {
        return;
    }
    
    auto prevCell = this->selectedCell;
    
    if( prevCell ) {
        prevCell->setSelected(false);
    }
    
    this->selectedCell = cell;
    cell->setSelected(true);
}

/**
 * 버튼 클릭
 */
void BoostPopup::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // Boost
        case Tag::BTN_BOOST: {
            auto stageData = selectedCell->getData();
            CCLOG("boost stage: %d-%d", stageData.world, stageData.stage);
            
            // 코인 체크
            if( !User::isEnoughCoin(stageData.boostCost) ) {
                PopupManager::show(PopupType::SHOP, ZOrder::POPUP_TOP);
                return;
            }
            
            User::spendCoin(stageData.boostCost);
            
            onBoostListener(stageData);
            dismiss();
            
        } break;
            
        // 이전 월드
        case Tag::BTN_WORLD_PREV: {
            updateWorld(currentWorldIndex-1);
        } break;
            
        // 다음 월드
        case Tag::BTN_WORLD_NEXT: {
            updateWorld(currentWorldIndex+1);
        } break;
            
        // 팝업 닫기
        case Tag::BTN_CLOSE: {
            onClosedListener();
            dismiss();
        } break;
            
        default: break;
    }
}


/**
 * 배경 초기화
 */
void BoostPopup::initBg() {
    
    // boost_bg.png Vec2MC(0, -62) , Size(664, 988)
    // boost_title.png Vec2TC(-1, -238) , Size(404, 92)
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2MC(0, -62),    DIR_IMG_BOOST + "boost_bg.png"),
        SBUIInfo(ANCHOR_M, Vec2TC(0, -238),   DIR_IMG_BOOST + "boost_title.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(info.file);
        info.apply(spr);
        addChild(spr);
    }
    
    // 배너
    auto bannerView = BannerView::create();
    bannerView->setTag(Tag::BANNER);
    addChild(bannerView, SBZOrder::BOTTOM);
    
    // 유저 코인뷰
    addChild(UserCoinView::create(UserCoinView::Type::NORMAL), SBZOrder::BOTTOM);
    
    // 닫기 버튼
    // common_btn_close_red.png Vec2TR(-46, -146) , Size(76, 76)
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_close_red.png");
    closeBtn->setTag(Tag::BTN_CLOSE);
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(Vec2TR(-42, -146));
    closeBtn->setZoomScale(ButtonZoomScale::HARD);
    addChild(closeBtn);
    
    closeBtn->setOnClickListener(CC_CALLBACK_1(BoostPopup::onClick, this));
    
    // boost_btn_boost.png Vec2BC(4, 222) , Size(520, 172)
    auto boostBtn = SBButton::create(DIR_IMG_BOOST + "boost_btn_boost.png");
    boostBtn->setTag(Tag::BTN_BOOST);
    boostBtn->setAnchorPoint(ANCHOR_M);
    boostBtn->setPosition(Vec2BC(0, 222));
    boostBtn->setZoomScale(ButtonZoomScale::WEAK);
    addChild(boostBtn);
    
    boostBtn->setOnClickListener(CC_CALLBACK_1(BoostPopup::onClick, this));
}

/**
 * 월드 스테이지 리스트 View 초기화
 */
void BoostPopup::initWorldView() {
    
    Vec2 cellPos[] = {
        Vec2MC(0, 161),
        Vec2MC(0, 25),
        Vec2MC(0, -111),
        Vec2MC(0, -247),
    };
    
    for( int i = 0; i < Database::getOriginWorldCount(); ++i ) {
        auto world = Database::getWorlds()[i];
        
        auto worldView = SBNodeUtils::createWinSizeNode();
        worldView->setVisible(false);
        addChild(worldView);
        
        worldViews.push_back(worldView);
        
        for( int j = 0; j < world.stages.size(); ++j ) {
            auto stage = world.stages[j];
            
            auto cell = Boost::ListCell::create(stage, j);
            cell->setAnchorPoint(ANCHOR_M);
            cell->setPosition(cellPos[j]);
            cell->setOnClickListener(CC_CALLBACK_1(BoostPopup::onSelectedCell, this));
            cell->setLocked(stage.stageSeq > boostMaxStage.stageSeq);
            worldView->addChild(cell);
            
            // 부스트 가능한 최대 스테이지인 경우 셀 선택
            if( stage.stageSeq == boostMaxStage.stageSeq ) {
                this->onSelectedCell(cell);
            }
        }
    }
    
    auto worldLabel = Label::createWithBMFont(FONT_WORLD_SILVER, "", TextHAlignment::CENTER);
    worldLabel->setTag(Tag::WORLD_LABEL);
    worldLabel->setAnchorPoint(ANCHOR_M);
    worldLabel->setPosition(Vec2MC(0, 282));
    addChild(worldLabel);
    
    // boost_btn_arrow.png Vec2MC(-232, 282) , Size(36, 60)
    auto prevBtn = SBButton::create(DIR_IMG_BOOST + "boost_btn_arrow.png");
    prevBtn->setTag(Tag::BTN_WORLD_PREV);
    prevBtn->setAnchorPoint(ANCHOR_M);
    prevBtn->setPosition(Vec2MC(-232, 282));
    prevBtn->setZoomScale(ButtonZoomScale::HARD);
    prevBtn->getContentsLayer()->setFlippedX(true);
    addChild(prevBtn);
    
    prevBtn->setOnClickListener(CC_CALLBACK_1(BoostPopup::onClick, this));
    
    // boost_btn_arrow.png Vec2MC(238, 282) , Size(36, 60)
    auto nextBtn = SBButton::create(DIR_IMG_BOOST + "boost_btn_arrow.png");
    nextBtn->setTag(Tag::BTN_WORLD_NEXT);
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setPosition(Vec2MC(232, 282));
    nextBtn->setZoomScale(ButtonZoomScale::HARD);
    addChild(nextBtn);
    
    nextBtn->setOnClickListener(CC_CALLBACK_1(BoostPopup::onClick, this));
}

/**
 * 등장 연출
 */
void BoostPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(0, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 퇴장 연출
 */
void BoostPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(0, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

/**
 * 등장 연출 완료
 */
void BoostPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}

NS_BOOST_BEGIN

#pragma mark- ListCell

ListCell* ListCell::create(const StageData &data, int cellIndex) {
    
    auto cell = new ListCell();
    
    if( cell && cell->init(data, cellIndex) ) {
        cell->autorelease();
        return cell;
    }
    
    CC_SAFE_DELETE(cell);
    return nullptr;
}

ListCell::ListCell() :
onClickListener(nullptr),
selected(false),
locked(false) {
}

ListCell::~ListCell() {
}

bool ListCell::init(const StageData &data, int cellIndex) {
    
    if( !Widget::init() ) {
        return false;
    }
    
    this->data = data;
    this->cellIndex = cellIndex;
    
    auto size = LIST_CELL_SIZE;
    setContentSize(size);
    
    // 터치 이벤트
    setTouchEnabled(true);
    
    addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        if( !this->isLocked() ) {
            switch( eventType ) {
                case Widget::TouchEventType::BEGAN:     this->setScale(1+ButtonZoomScale::WEAK); break;
                case Widget::TouchEventType::ENDED:
                case Widget::TouchEventType::CANCELED:  this->setScale(1); break;
                default: break;
            }
        }
    });
    
    addClickEventListener([=](Ref*) {
        
        if( !this->isLocked() ) {
            onClickListener(this);
        }
    });
    
    // bg
    string bgFiles[] = {
        "boost_btn_stage1.png",
        "boost_btn_stage2.png",
        "boost_btn_stage3.png",
        "boost_btn_stage_boss.png",
    };
    
    auto bg = Sprite::create(DIR_IMG_BOOST + bgFiles[cellIndex]);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(size, 0, 0));
    addChild(bg);
    
    // cost
    costLabel = Label::createWithTTF(TO_STRING(data.boostCost), FONT_COMMODORE, 48, Size::ZERO,
                                     TextHAlignment::CENTER, TextVAlignment::CENTER);
    costLabel->setAnchorPoint(ANCHOR_M);
    costLabel->setPosition(Vec2MC(size, 167, 7 + 2));
    costLabel->enableOutline(Color4B(0,0,0,255), 4);
    addChild(costLabel);
    
    // 코인 업데이트 리스너
    auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_USER_COIN, [=](EventCustom *event) {
        this->updateCostTextColor();
    });
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // coin icon
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(Vec2MC(size, 68, 7));
    addChild(coinIcon);
    
    // selected
    selectedImage = Sprite::create(DIR_IMG_BOOST + "boost_focus_stage.png");
    selectedImage->setAnchorPoint(ANCHOR_M);
    selectedImage->setPosition(Vec2MC(size, 0, 0));
    selectedImage->setVisible(false);
    addChild(selectedImage);
    
    // 잠금 레이어
    lockedLayer = Node::create();
    lockedLayer->setAnchorPoint(ANCHOR_M);
    lockedLayer->setPosition(Vec2MC(size, 0, 0));
    lockedLayer->setContentSize(size);
    lockedLayer->setVisible(false);
    addChild(lockedLayer);
    
    auto lockedBg = Sprite::create(DIR_IMG_BOOST + "boost_cover_stage_dis.png");
    lockedBg->setAnchorPoint(ANCHOR_M);
    lockedBg->setPosition(Vec2MC(size, 0, 4));
    lockedLayer->addChild(lockedBg);
    
    auto lockedIcon = Sprite::create(DIR_IMG_BOOST + "boost_icon_lock.png");
    lockedIcon->setAnchorPoint(ANCHOR_M);
    lockedIcon->setPosition(Vec2MC(size, 0, 5));
    lockedLayer->addChild(lockedIcon);
    
    return true;
}

void ListCell::updateCostTextColor() {
    
    if( isLocked() ) {
        costLabel->setTextColor(Color4B(255,255,255,255));
    } else {
        costLabel->setTextColor(User::isEnoughCoin(data.boostCost) ? Color4B(255,255,255,255) : Color4B(255,0,0,255));
    }
}

void ListCell::setSelected(bool selected) {
    
    this->selected = selected;
    selectedImage->setVisible(selected);
}

void ListCell::setLocked(bool locked) {
    
    this->locked = locked;
    lockedLayer->setVisible(locked);
    
    updateCostTextColor();
}

NS_BOOST_END
