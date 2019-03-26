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

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

#define BRICK_LAYER_DEBUG_DRAW        0

#define BRICK_PADDING_X               20
#define BRICK_PADDING_Y               20
#define BRICK_SCALE                   0.7f
#define BRICK_WIDTH                   (TILE_CONTENT_WIDTH*BRICK_SCALE)
#define BRICK_HEIGHT                  (TILE_CONTENT_HEIGHT*BRICK_SCALE)

#define BOSS_BRICK_ORIGIN             Vec2MC(0, 51)

#define NORMAL_BRICK_COUNT_IN_ROW     5
#define NORMAL_BRICK_ROW_WIDTH \
((NORMAL_BRICK_COUNT_IN_ROW*BRICK_WIDTH) + ((NORMAL_BRICK_COUNT_IN_ROW-1)*BRICK_PADDING_X))

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
    initTitle();
    initBrick();
    initCoin();
    initMenu();
    
    // 배너
    addChild(BannerView::create());
    
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
 * 타이틀 초기화
 */
void StageClearPopup::initTitle() {
    
    // clear_bg_stage.png Vec2TC(0, -314) , Size(352, 316)
    // clear_text_clear.png Vec2TC(-1, -390) , Size(304, 76)
    // clear_text_stage.png Vec2TC(-1, -208) , Size(192, 56)
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2TC(0, -314), DIR_IMG_STAGE_CLEAR + "clear_bg_stage.png"),
        SBUIInfo(ANCHOR_M, Vec2TC(0, -390), DIR_IMG_STAGE_CLEAR + "clear_text_clear.png"),
        SBUIInfo(ANCHOR_M, Vec2TC(0, -208), DIR_IMG_STAGE_CLEAR + "clear_text_stage.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(info.file);
        info.apply(spr);
        addContentChild(spr);
    }
    
    // stage_big_yellow Vec2TC(0, -294) , Size(48, 100)
    auto stageLabel = Label::createWithBMFont(FONT_STAGE_LARGE_Y, TO_STRING(stage.stage), TextHAlignment::CENTER);
    stageLabel->setAnchorPoint(ANCHOR_M);
    stageLabel->setPosition(Vec2TC(0, -294));
    addContentChild(stageLabel);
}

/**
 * 브릭 초기화
 */
void StageClearPopup::initBrick() {
    
#if BRICK_LAYER_DEBUG_DRAW
    // 시작 라인 확인용
    {
        auto line = LayerColor::create(Color4B::BLUE);
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_M);
        line->setPosition(Vec2BC(0, BOSS_BRICK_ORIGIN.y));
        line->setContentSize(Size(NORMAL_BRICK_ROW_WIDTH, 8));
        addContentChild(line);
    }
    
    // center
    {
        auto line = LayerColor::create(Color4B(255,0,0,255*0.5f));
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_M);
        line->setPosition(Vec2MC(0, 0));
        line->setContentSize(Size(8, SB_WIN_SIZE.height));
        addContentChild(line);
    }

    // left
    {
        auto line = LayerColor::create(Color4B(255,0,0,255*0.5f));
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_M);
        line->setPosition(Vec2MC(-NORMAL_BRICK_ROW_WIDTH*0.5f, 0));
        line->setContentSize(Size(8, SB_WIN_SIZE.height));
        addContentChild(line);
    }
    
    // right
    {
        auto line = LayerColor::create(Color4B(255,0,0,255*0.5f));
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_M);
        line->setPosition(Vec2MC(NORMAL_BRICK_ROW_WIDTH*0.5f, 0));
        line->setContentSize(Size(8, SB_WIN_SIZE.height));
        addContentChild(line);
    }
#endif
    
    auto createBrick = [=](const BrickData &data) -> SBAnimationSprite* {
        
        auto brick = SBAnimationSprite::create(BRICK_ANIMATION(data, BrickImageType::IDLE));
        // auto brick = SBAnimationSprite::create(BRICK_ANIMATION(data, BrickImageType::DAMAGE), 1);
        brick->setScale(BRICK_SCALE);
        brick->runAnimation();
        
#if BRICK_LAYER_DEBUG_DRAW
        {
            auto n = LayerColor::create(Color4B(0,255,0,255*0.3f));
            n->setIgnoreAnchorPointForPosition(false);
            n->setAnchorPoint(ANCHOR_M);
            n->setPosition(Vec2MC(brick->getContentSize(), 0, 0));
            n->setContentSize(brick->getContentSize());
            brick->addChild(n, -1);
        }
#endif
        return brick;
    };
    
    auto getBrickSize = [](Node *brick) -> Size {
        
        auto size = brick->getContentSize();
        size.width *= brick->getScaleX();
        size.height *= brick->getScaleY();
        
        return size;
    };
    
    // 보스 브릭
    // brick_10006_idle_01.png Vec2MC(0, 51) , Size(160, 154)
    float originY = BOSS_BRICK_ORIGIN.y + (normalBrickDatas.size() <= NORMAL_BRICK_COUNT_IN_ROW ? -40 : 0);
    
    auto bossBrickLayer = Node::create();
    bossBrickLayer->setAnchorPoint(ANCHOR_M);
    bossBrickLayer->setPosition(Vec2(BOSS_BRICK_ORIGIN.x, originY));
    bossBrickLayer->setContentSize(Size(0,0));
    addContentChild(bossBrickLayer);
    
    {
        vector<Node*> bossBricks;
        float posX = 0;
        
        for( auto brickData : bossBrickDatas ) {
            auto brick = createBrick(brickData);
            auto brickSize = getBrickSize(brick);
            
            brick->setAnchorPoint(ANCHOR_M);
            brick->setPosition(Vec2(posX + brickSize.width*0.5f, 0));
            bossBrickLayer->addChild(brick);
            
            bossBricks.push_back(brick);
            bricks.push_back(brick);
            
            // X 좌표 업데이트
            posX += brickSize.width + BRICK_PADDING_X;
            
            // 레이어 높이 설정
            if( brickSize.height > bossBrickLayer->getContentSize().height ) {
                bossBrickLayer->setContentSize(Size(bossBrickLayer->getContentSize().width, brickSize.height));
            }
        }
        
        // 보스 브릭 레이어 가운데 정렬
        auto firstBrick = bossBricks[0];
        auto lastBrick  = bossBricks[bossBricks.size()-1];
        
        float width = SBNodeUtils::getBoundingBoxInWorld(lastBrick,true).getMaxX() -
                      SBNodeUtils::getBoundingBoxInWorld(firstBrick,true).getMinX();
        bossBrickLayer->setContentSize(Size(width, bossBrickLayer->getContentSize().height));
        
        // 보스 브릭 Y좌표 설정
        for( auto brick : bossBricks ) {
            brick->setPositionY(bossBrickLayer->getContentSize().height*0.5f);
        }
    }
    
    // 일반 브릭 좌표
    const float ORIGIN_X = (SB_WIN_SIZE.width - NORMAL_BRICK_ROW_WIDTH) * 0.5f;
    const float ORIGIN_Y = SB_BOUNDING_BOX_IN_WORLD(bossBrickLayer).getMinY() - BRICK_PADDING_Y - 10;
    
    auto getNormalBrickPosition = [=](int i) -> Vec2 {
        
        int x = i % NORMAL_BRICK_COUNT_IN_ROW;
        int y = i / NORMAL_BRICK_COUNT_IN_ROW;
        
        Vec2 p(ORIGIN_X, ORIGIN_Y);
        p.x += x * BRICK_WIDTH;
        p.y -= y * BRICK_HEIGHT;
        p.x += x * BRICK_PADDING_X;
        p.y -= y * BRICK_PADDING_Y;
        
        return p;
    };
    
    for( int i = 0; i < normalBrickDatas.size(); ++i ) {
        auto brickData = normalBrickDatas[i];
        
        auto brick = createBrick(brickData);
        brick->setAnchorPoint(ANCHOR_TL);
        brick->setPosition(getNormalBrickPosition(i));
        addContentChild(brick);
        
        bricks.push_back(brick);
    }
}

/**
 * 획득 코인 초기화
 */
void StageClearPopup::initCoin() {
    
    // 보유 코인
    {
        // clear_bg_coin_user.png Vec2TL(106, -138) , Size(188, 52)
        auto bg = Sprite::create(DIR_IMG_STAGE_CLEAR + "clear_bg_coin_user.png");
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(Vec2TL(106, -138));
        addContentChild(bg);
        
        // common_icon_coin_tiny.png Vec2TL(38, -138) , Size(36, 36)
        auto icon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_tiny.png");
        icon->setAnchorPoint(ANCHOR_M);
        icon->setPosition(Vec2TL(38, -138));
        addContentChild(icon);
        
        // 9999 size:34 Vec2TL(119, -137) , Size(92, 23)
        auto coinLabel = Label::createWithTTF("9999", FONT_COMMODORE, 34, Size::ZERO,
                                              TextHAlignment::CENTER,  TextVAlignment::CENTER);
        coinLabel->setAnchorPoint(ANCHOR_M);
        coinLabel->setPosition(Vec2TL(119, -137));
        addContentChild(coinLabel);
    }
    
    // common_icon_coin_big.png Vec2MC(-119, -294) , Size(84, 84)
    auto icon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_big.png");
    icon->setAnchorPoint(ANCHOR_M);
    icon->setPosition(Vec2MC(-119, -294));
    addContentChild(icon);
    
    // coin_blue Vec2MC(51, -296) , Size(224, 70)
    auto coinLabel = Label::createWithBMFont(FONT_COIN_BLUE, "x 0", TextHAlignment::LEFT);
    coinLabel->setAnchorPoint(ANCHOR_ML);
    coinLabel->setPosition(Vec2MC(51 - 224*0.5f, -296));
    addContentChild(coinLabel);
    
    // 코인 증가 액션
    auto onChanged = [=](float value) {
        
        int i = (int)value;
        coinLabel->setString(STR_FORMAT("x %d", i));
    };
    
    auto numberAction = ActionFloat::create(0.3f, 0, rewardCoin, onChanged);
    coinLabel->runAction(numberAction);
}

/**
 * 메뉴 초기화
 */
void StageClearPopup::initMenu() {
    
    // 프렌즈
    // clear_btn_friend.png Vec2TR(-60, -158) , Size(104, 100)
    auto friendsBtn = SBButton::create(DIR_IMG_STAGE_CLEAR + "clear_btn_friend.png");
    friendsBtn->setZoomScale(0.1f);
    friendsBtn->setAnchorPoint(ANCHOR_M);
    friendsBtn->setPosition(Vec2TR(-60, -158));
    addContentChild(friendsBtn);
    
    friendsBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    });
    
    // clear_btn_next_stage.png Vec2BL(196, 190) , Size(312, 188)
    // 다음 스테이지 이동 버튼
    auto nextBtn = SBButton::create(DIR_IMG_STAGE_CLEAR + "clear_btn_next_stage.png");
    nextBtn->setZoomScale(0.1f);
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setPosition(Vec2BL(196, 190));
    addContentChild(nextBtn);
    
    nextBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onNextListener);
    });
    
    // 광고 보기 버튼
    // clear_btn_ad.png Vec2BR(-196, 190) , Size(312, 188)
    auto adBtn = Button::create(DIR_IMG_STAGE_CLEAR + "clear_btn_ad.png");
    adBtn->setZoomScale(0);
    adBtn->setAnchorPoint(ANCHOR_M);
    adBtn->setPosition(Vec2BR(-196, 190));
    addContentChild(adBtn);
    
    adBtn->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN: {
                
            } break;
            
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED: {
                
            } break;
                
            default: break;
        }
    });
    
    adBtn->addClickEventListener([=](Ref*) {
        
    });
    
    // coin label
    // +240 size:48 stroke:4 Vec2BR(-167, 148) , Size(149, 45)
    auto coinLabel = Label::createWithTTF(STR_FORMAT("+%d", rewardCoin*2), FONT_COMMODORE, 48, Size::ZERO,
                                          TextHAlignment::RIGHT, TextVAlignment::CENTER);
    coinLabel->setAnchorPoint(ANCHOR_MR);
    coinLabel->setPosition(adBtn->convertToNodeSpace(Vec2BR(-167 + 149*0.5f, 148)));
    coinLabel->setTextColor(Color4B::WHITE);
    coinLabel->enableShadow(Color4B(0,0,0,255), Size(0, -4));
    adBtn->addChild(coinLabel);
    
    // coin icon
    // common_icon_coin_small.png Vec2BR(-270, 148) , Size(45, 45)
    auto coinIcon = Sprite::create(DIR_IMG_COMMON + "common_icon_coin_small.png");
    coinIcon->setAnchorPoint(ANCHOR_M);
    coinIcon->setPosition(adBtn->convertToNodeSpace(Vec2BR(-270, 148)));
    adBtn->addChild(coinIcon);
    
    // x2 badge
    // clear_badge_x2.png Vec2BR(-64, 264) , Size(104, 104)
    auto badge = Sprite::create(DIR_IMG_STAGE_CLEAR + "clear_badge_x2.png");
    badge->setAnchorPoint(ANCHOR_M);
    badge->setPosition(adBtn->convertToNodeSpace(Vec2BR(-64, 264)));
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
     
    // 브릭 등장 연출
    for( auto brick : bricks ) {
        brick->setVisible(false);
    }
    
    brickEnterIndex = 0;
    const string SCHEDULER = "BRICK_ENTER";
    
    schedule([=](float dt) {
        
        bricks[brickEnterIndex++]->setVisible(true);
        
        if( brickEnterIndex == bricks.size() ) {
            this->unschedule(SCHEDULER);
        }
    }, 0.1f, SCHEDULER);
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
