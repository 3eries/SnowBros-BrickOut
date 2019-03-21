//
//  BallSkinPopup.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 14/03/2019.
//

#include "BallSkinPopup.hpp"

#include "Define.h"
#include "User.hpp"

#include "ContentManager.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

#define LIST_SIZE               Size(584,754)
#define LIST_ROW_SIZE           Size(536,164)
#define LIST_CELL_SIZE          Size(152,164)
#define LIST_CELL_PADDING       6

#define SELECT_BUTTON_POS       Vec2BC(0,134)
#define BUTTON_MOVE_DURATION    0.1f

// static const float FADE_DURATION    = 0.15f;

BallSkinPopup* BallSkinPopup::create() {
    
    auto popup = new BallSkinPopup();
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

BallSkinPopup::BallSkinPopup() : BasePopup(PopupType::BALL_SKIN),
onBallSelectedListener(nullptr),
selectedCell(nullptr) {
}

BallSkinPopup::~BallSkinPopup() {
}

bool BallSkinPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void BallSkinPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void BallSkinPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void BallSkinPopup::initContentView() {
    
    BasePopup::initContentView();
    
    initBg();
    initButton();
    initListView();
}

/**
 * 셀 선택
 */
void BallSkinPopup::onSelectedCell(BallSkin::ListCell *cell) {
    
    CCASSERT(cell != nullptr, "BallSkinPopup::onSelectedCell error.");
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    // 광고 보기 셀
    if( cell->getData().isWatchAd ) {
        CCLOG("onSelectedCell watch ad (reward amount: %d)", cell->getData().amount);
        return;
    }
    
    // 일반 셀
    auto ballData = cell->getData().ballData;
    CCLOG("onSelectedCell ball id: %s", ballData.ballId.c_str());
    
    if( selectedCell == cell ) {
        return;
    }
    
    // 이전 셀 선택 해제
    auto prevCell = selectedCell;
    
    if( prevCell ) {
        prevCell->setSelected(false);
    }
    
    selectedCell = cell;
    selectedCell->setSelected(true);
    
    // 이전과 다른행인 경우 스크롤
    if( prevCell && prevCell->getRowIndex() != cell->getRowIndex() ) {
        ballListView->setBounceEnabled(false);
        ballListView->scrollToItem(cell->getRowIndex(), Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
    }
    
    // 버튼 업데이트
    bool withAction = (!unlockButton->isVisible() && !selectButton->isVisible());
    
    // 이미 소유한 볼
    if( cell->isUnlocked() ) {
        // 볼 스킨으로 설정되지 않은 경우, 선택 버튼 노출
//        if( User::getSelectedBallSkin() != ballData.ballId ) {
//        }
        
        if( prevCell ) {
            showButton(selectButton, withAction);
            hideButton(unlockButton, false);
        }
    }
    // 잠김
    else {
        unlockButton->setData(ballData);
        
        showButton(unlockButton, withAction);
        hideButton(selectButton, false);
    }
}

/**
 * 잠금 해제 버튼 클릭
 */
void BallSkinPopup::onClickUnlockButton() {
 
    CCASSERT(selectedCell != nullptr, "BallSkinPopup::onClickUnlockButton error.");
    
    auto ballData = selectedCell->getData().ballData;
    CCLOG("onClickUnlockButton ball id: %s", ballData.ballId.c_str());
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    // 코인 체크
    
    // spend the money
    // TODO: 코인 소모
    
    // 해제
    User::ownBallSkin(ballData.ballId);
    
    selectedCell->setUnlocked(true);

    showButton(selectButton, false);
    hideButton(unlockButton, false);
}

/**
 * 선택 버튼 클릭
 */
void BallSkinPopup::onClickSelectButton() {
    
    CCASSERT(selectedCell != nullptr, "BallSkinPopup::onClickSelectButton error.");
    
    auto ballData = selectedCell->getData().ballData;
    CCLOG("onClickSelectButton ball id: %s", ballData.ballId.c_str());;
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    User::setSelectedBallSkin(ballData.ballId);
    
    // 팝업 닫힘
    dismissWithAction();
}

void BallSkinPopup::showButton(Widget *btn, bool withAction) {
 
    btn->stopAllActions();
    btn->setVisible(true);
    
    if( withAction ) {
        btn->setPosition(Vec2(SELECT_BUTTON_POS.x, -btn->getAnchorPointInPoints().y));
        
        auto move = MoveTo::create(BUTTON_MOVE_DURATION, SELECT_BUTTON_POS);
        auto callFunc = CallFunc::create([=]() {
            btn->setTouchEnabled(true);
        });
        btn->runAction(Sequence::create(move, callFunc, nullptr));
        
    } else {
        btn->setTouchEnabled(true);
        btn->setPosition(SELECT_BUTTON_POS);
    }
}

void BallSkinPopup::hideButton(Widget *btn, bool withAction) {
 
    btn->stopAllActions();
    btn->setTouchEnabled(false);
    
    if( withAction ) {
        btn->setPosition(SELECT_BUTTON_POS);
        
        auto move = MoveTo::create(BUTTON_MOVE_DURATION, Vec2(SELECT_BUTTON_POS.x, -btn->getAnchorPointInPoints().y));
        auto callFunc = CallFunc::create([=]() {
            btn->setVisible(false);
        });
        btn->runAction(Sequence::create(move, callFunc, nullptr));
        
    } else {
        btn->setVisible(false);
    }
}

/**
 * 배경 초기화
 */
void BallSkinPopup::initBg() {
    
    // ball_skin_title_deco.png Vec2TC(28, -174) , Size(176, 132)
    // ball_skin_title.png Vec2TC(0, -238) , Size(376, 100)
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2TC(28, -174),    DIR_IMG_BALL_SKIN + "ball_skin_title_deco.png"),
        SBUIInfo(ANCHOR_M, Vec2TC(0, -238),     DIR_IMG_BALL_SKIN + "ball_skin_title.png"),
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
    // common_btn_close_red.png Vec2TR(-74, -138) , Size(76, 76)
    auto closeBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_close_red.png");
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(Vec2TR(-74, -138));
    closeBtn->setZoomScale(0.1f);
    addContentChild(closeBtn);
    
    closeBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
}

/**
 * 하단 버튼 초기화
 */
void BallSkinPopup::initButton() {
    
    // Unlock 버튼
    // ball_skin_btn_unlock.png Vec2BC(0, 136) , Size(496, 128)
    unlockButton = BallSkin::UnlockButton::create();
    unlockButton->setAnchorPoint(ANCHOR_M);
    addContentChild(unlockButton);
    
    unlockButton->addClickEventListener([=](Ref*) {
        this->onClickUnlockButton();
    });
    
    // Select 버튼
    // ball_skin_btn_select.png Vec2BC(0, 134) , Size(484, 124)
    selectButton = Button::create(DIR_IMG_BALL_SKIN + "ball_skin_btn_select.png");
    selectButton->setZoomScale(0.1f);
    selectButton->setAnchorPoint(ANCHOR_M);
    addContentChild(selectButton);
    
    selectButton->addClickEventListener([=](Ref*) {
        this->onClickSelectButton();
    });
    
    hideButton(unlockButton, false);
    hideButton(selectButton, false);
}

/**
 * 볼 리스트 초기화
 */
void BallSkinPopup::initListView() {
    
    vector<BallSkin::CellData> datas;
    
    // Watch Ad 데이터 초기화
    BallSkin::CellData watchAdData;
    watchAdData.isWatchAd = true;
    watchAdData.amount = 100;
    datas.push_back(watchAdData);
    
    // 볼스킨 데이터 초기화
    auto ballSkins = Database::getBallSkins();
    
    for( auto ballSkin : ballSkins ) {
        BallSkin::CellData data;
        data.ballData = ballSkin;
        data.amount = ballSkin.unlockAmount;
        datas.push_back(data);
    }
    
    // 리스트뷰 초기화
    // mask Vec2MC(0, -41) , Size(584, 754) top 976 - 960
    ballListView = ListView::create();
    ballListView->setDirection(ui::ScrollView::Direction::VERTICAL);
    ballListView->setPadding(0, 16, 0, 0);
    ballListView->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    ballListView->setItemsMargin(LIST_CELL_PADDING);
    ballListView->setBounceEnabled(true);
    ballListView->setScrollBarEnabled(true);
    ballListView->setScrollBarPositionFromCorner(Vec2(7, 0));
    ballListView->setScrollBarColor(Color3B::GRAY);
    ballListView->setAnchorPoint(ANCHOR_M);
    ballListView->setPosition(Vec2MC(0, -41));
    ballListView->setContentSize(LIST_SIZE);
    ballListView->addEventListener([=](Ref*, ScrollView::EventType eventType) {
      
        if( eventType == ScrollView::EventType::AUTOSCROLL_ENDED ) {
            // CCLOG("AUTOSCROLL_ENDED");
            ballListView->setBounceEnabled(true);
        }
    });
    
    addContentChild(ballListView);
    
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
    int rowIndex = 0;
    
    for( int i = 0; i < datas.size(); ) {
        int len = 3;
        len = MIN((int)datas.size()-i, len);
        
        BallSkin::Cells cells;
        
        for( int j = 0; j < len; ++j ) {
            auto cellData = datas[i+j];
            auto ballData = cellData.ballData;
            
            auto cell = BallSkin::ListCell::create(cellData);
            cell->setRowIndex(rowIndex);
            cell->setOnClickListener(CC_CALLBACK_1(BallSkinPopup::onSelectedCell, this));
            cell->setUnlocked(User::isOwnedBallSkin(ballData.ballId));
            
            if( User::getSelectedBallSkin() == ballData.ballId ) {
                onSelectedCell(cell);
            }
            
            cells.push_back(cell);
        }
        
        // BG_unit3 Vec2MC(0, 238) , Size(536, 164)     y = 878, 796 960
        // BG_unit3 Vec2MC(0, 68) , Size(536, 164)      y = 708, 790, 626
        // BG_unit3 Vec2MC(0, -102) , Size(536, 164)    y = 538, 620
        auto row = BallSkin::ListRow::create(cells);
        row->setRowIndex(rowIndex);
        row->setAnchorPoint(ANCHOR_M);
        ballListView->pushBackCustomItem(row);
        
        i += len;
        rowIndex++;
    }
}

/**
 * 등장 연출
 */
void BallSkinPopup::runEnterAction(SBCallback onFinished) {
    
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
void BallSkinPopup::runExitAction(SBCallback onFinished) {
    
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
void BallSkinPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}

NS_BALL_SKIN_BEGIN

#pragma mark- UnlockButton

UnlockButton::UnlockButton() {
    
}

UnlockButton::~UnlockButton() {
    
}

bool UnlockButton::init() {
    
    if( !Widget::init() ) {
        return false;
    }
    
    // 배경
    // BG Vec2MC(0, 0) , Size(496, 128)
    // ball_skin_btn_unlock.png Vec2MC(0, 0) , Size(496, 128)
    auto bg = Sprite::create(DIR_IMG_BALL_SKIN + "ball_skin_btn_unlock.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(bg->getContentSize(), 0, 0));
    addChild(bg);
    
    auto size = bg->getContentSize();
    setContentSize(size);
    
    // 터치 이벤트
    setTouchEnabled(true);
    
    addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN:         setScale(1.1f);     break;
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED:      setScale(1.0f);     break;
            default: break;
        }
    });
    
    // ball_skin_text_unlock.png Vec2MC(-106, 6) , Size(212, 44)
    auto btnTitle = Sprite::create(DIR_IMG_BALL_SKIN + "ball_skin_text_unlock.png");
    btnTitle->setAnchorPoint(ANCHOR_M);
    btnTitle->setPosition(Vec2MC(size, -106, 6));
    addChild(btnTitle);
    
    // common_icon_coin_small.png Vec2MC(40, 7) , Size(45, 45)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(Vec2MC(size, 40, 7));
    addChild(coinIcon);
    
    // 9999 size:48 stroke:4 Vec2MC(146, 7) , Size(148, 45)
    coinLabel = Label::createWithTTF("0", FONT_COMMODORE, 48, Size::ZERO,
                                     TextHAlignment::CENTER, TextVAlignment::CENTER);
    coinLabel->setAnchorPoint(ANCHOR_M);
    coinLabel->setPosition(Vec2MC(size, 146, 7 + 4));
    coinLabel->setTextColor(Color4B::WHITE);
    coinLabel->enableShadow(Color4B(0,0,0,255), Size(0, -4));
    addChild(coinLabel);
    
    return true;
}

void UnlockButton::setData(const BallSkinData &data) {
    
    this->data = data;
    
    coinLabel->setString(TO_STRING(data.unlockAmount));
}

#pragma mark- ListRow

ListRow* ListRow::create(const Cells &cells) {
    
    auto row = new ListRow();
    
    if( row && row->init(cells) ) {
        row->autorelease();
        return row;
    }
    
    CC_SAFE_DELETE(row);
    return nullptr;
}

ListRow::ListRow() {
}

ListRow::~ListRow() {
}

bool ListRow::init(const Cells &cells) {
    
    if( !Widget::init() ) {
        return false;
    }
    
    CCASSERT(cells.size() <= 3, "ListRow::init error.");
    
    auto size = LIST_ROW_SIZE;
    setContentSize(size);
    
    // BG Vec2ML(76, 0) , Size(152, 164)
    // BG Vec2MC(0, 0) , Size(152, 164)
    // BG Vec2MR(-76, 0) , Size(152, 164)
    Vec2 pos[] = {
        Vec2ML(size, 76, 0),
        Vec2MC(size, 0, 0),
        Vec2MR(size, -76, 0),
    };
    
    for( int i = 0; i < cells.size(); ++i ) {
        auto cell = cells[i];
        cell->setAnchorPoint(ANCHOR_M);
        cell->setPosition(pos[i]);
        addChild(cell);
    }
    
    return true;
}

#pragma mark- ListCell

ListCell* ListCell::create(const CellData &data) {
    
    auto cell = new ListCell();
    
    if( cell && cell->init(data) ) {
        cell->autorelease();
        return cell;
    }
    
    CC_SAFE_DELETE(cell);
    return nullptr;
}

ListCell::ListCell() :
onClickListener(nullptr),
unlocked(false), selected(false),
bg(nullptr), selectedImage(nullptr), ball(nullptr), coinLayer(nullptr), lockedLayer(nullptr) {
}

ListCell::~ListCell() {
}

bool ListCell::init(const CellData &data) {
    
    if( !Widget::init() ) {
        return false;
    }
    
    this->data = data;
    
    auto size = LIST_CELL_SIZE;
    setContentSize(size);
    
    // 클릭 이벤트
    setTouchEnabled(true);
    addClickEventListener([=](Ref*) {
        if( onClickListener ) {
            onClickListener(this);
        }
    });
    
    // UI 초기화
    coinLayer = Node::create();
    coinLayer->setAnchorPoint(Vec2::ZERO);
    coinLayer->setPosition(Vec2::ZERO);
    coinLayer->setContentSize(size);
    addChild(coinLayer, ZOrder::COIN);
    
    // bg
    auto addBg = [=](string file) -> Sprite* {
        bg = Sprite::create(DIR_IMG_BALL_SKIN + file);
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(Vec2MC(size, 0, 6));
        addChild(bg, ZOrder::BG);
        
        return bg;
    };
    
    // coin bg
    auto addCoinBg = [=](string file) -> Sprite* {
        auto coinBg = Sprite::create(DIR_IMG_BALL_SKIN + file);
        coinBg->setAnchorPoint(ANCHOR_M);
        coinBg->setPosition(Vec2MC(size, 0, -60));
        coinLayer->addChild(coinBg);
        
        return coinBg;
    };
    
    // coin label
    auto addCoinLabel = [=](string str) -> Label* {
        auto coinLabel = Label::createWithTTF(str, FONT_COMMODORE, 34, Size::ZERO,
                                              TextHAlignment::CENTER, TextVAlignment::CENTER);
        coinLabel->setAnchorPoint(ANCHOR_M);
        coinLabel->setPosition(Vec2MC(size, 18, -59));
        coinLabel->setTextColor(Color4B::WHITE);
        coinLayer->addChild(coinLabel);
        
        return coinLabel;
    };
    
    // Watch Ad
    if( data.isWatchAd ) {
        addBg("ball_skin_bg_circle_ad.png");
        addCoinBg("ball_skin_bg_coin_ad.png");
        
        // +999 size:34 RGB:0,255,172 Vec2MC(17, -59) , Size(92, 23)
        auto label = addCoinLabel(STR_FORMAT("+%d", data.amount));
        label->setTextColor(Color4B(0,255,172,255));
        
        // ball_skin_icon_ad.png Vec2MC(0, 12) , Size(104, 84)
        auto icon = Sprite::create(DIR_IMG_BALL_SKIN + "ball_skin_icon_ad.png");
        icon->setAnchorPoint(ANCHOR_M);
        icon->setPosition(Vec2MC(size, 0, 12));
        addChild(icon, ZOrder::ICON);
    }
    // Ball
    else {
        addBg("ball_skin_bg_circle.png");
        addCoinBg("ball_skin_bg_coin.png");
        addCoinLabel(TO_STRING(data.amount));
        
        // ball
        // ball_00001_large.png Vec2MC(0, 6) , Size(104, 104)
        ball = Sprite::create(BALL_LARGE_IMAGE(data.ballData.ballId));
        ball->setAnchorPoint(ANCHOR_M);
        ball->setPosition(Vec2MC(size, 0, 6));
        addChild(ball, ZOrder::ICON);
        
        // selected
        // ball_skin_effect_focus.png Vec2MC(0, 6) , Size(152, 152)
        selectedImage = Sprite::create(DIR_IMG_BALL_SKIN + "ball_skin_effect_focus.png");
        selectedImage->setAnchorPoint(ANCHOR_M);
        selectedImage->setPosition(Vec2MC(size, 0, 6));
        selectedImage->setVisible(false);
        addChild(selectedImage, ZOrder::SELECTED);
        
        // 잠금 레이어
        auto clone = [](Sprite *spr) -> Sprite* {
            auto cloneSpr = Sprite::createWithTexture(spr->getTexture());
            cloneSpr->setAnchorPoint(spr->getAnchorPoint());
            cloneSpr->setPosition(spr->getPosition());
            return cloneSpr;
        };
        
        lockedLayer = Node::create();
        lockedLayer->setAnchorPoint(Vec2::ZERO);
        lockedLayer->setPosition(Vec2::ZERO);
        lockedLayer->setContentSize(size);
        addChild(lockedLayer, ZOrder::LOCKED);
        
        auto dimmedBg = clone(bg);
        dimmedBg->setColor(Color3B::BLACK);
        dimmedBg->setOpacity(255*0.5f);
        lockedLayer->addChild(dimmedBg);
        
        auto dimmedBall = clone(ball);
        dimmedBall->setColor(Color3B::BLACK);
        dimmedBall->setOpacity(255*0.4f);
        lockedLayer->addChild(dimmedBall);
    }
    
    // coin icon
    // common_icon_coin_tiny.png Vec2MC(-50, -60) , Size(36, 36)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(Vec2MC(size, -50, -60));
    coinLayer->addChild(coinIcon);
    
    setUnlocked(false);
    setSelected(false);
    
    return true;
}

void ListCell::setUnlocked(bool isUnlocked) {
 
    if( data.isWatchAd ) {
        return;
    }
    
    this->unlocked = isUnlocked;
    
    coinLayer->setVisible(!isUnlocked);
    lockedLayer->setVisible(!isUnlocked);
}

void ListCell::setSelected(bool isSelected) {
    
    if( this->selected == isSelected ) {
        return;
    }
    
    this->selected = isSelected;
    
    if( data.isWatchAd ) {
        return;
    }
    
    selectedImage->stopAllActions();
    selectedImage->setScale(1);
    selectedImage->setVisible(isSelected);
    
    // 선택
    if( isSelected ) {
        auto scale1 = ScaleTo::create(0.8f, 1.1f);
        auto scale2 = ScaleTo::create(0.9f, 1.0f);
        selectedImage->runAction(RepeatForever::create(Sequence::create(scale1, scale2, nullptr)));
    }
}

NS_BALL_SKIN_END
