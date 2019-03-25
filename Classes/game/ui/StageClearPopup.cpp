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

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

#define BRICK_LAYER_DEBUG_DRAW        0

#define BRICK_LAYER_WIDTH             SB_WIN_SIZE.width /*560*/
#define BRICK_ORIGIN_Y                (SB_WIN_SIZE.height - 220)
#define BRICK_MARGIN_X                20
#define BRICK_PADDING_X               20
#define BRICK_PADDING_Y               6
#define BRICK_SCALE                   0.7f

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

StageClearPopup* StageClearPopup::create(const StageData &stage, BrickDataList bricks) {
    
    auto popup = new StageClearPopup();
    
    if( popup && popup->init(stage, bricks) ) {
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

bool StageClearPopup::init(const StageData &stage, BrickDataList brickDatas) {
    
    if( !BasePopup::init() ) {
        return false;
    }

    this->stage = stage;
    
    // 브릭 리스트 초기화
    bossBricks = SBCollection::find(brickDatas, [](const BrickData &b) -> bool {
        return b.isBoss();
    });
    
    normalBricks = SBCollection::find(brickDatas, [](const BrickData &b) -> bool {
        return b.type == BrickType::NORMAL && b.is1x1();
    });
    
    // UI 초기화
    initTitle();
    initBrick();
    initMoney();
    initMenu();
    
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
    
    auto titleLabel = Label::createWithTTF(STR_FORMAT("STAGE %d CLEAR!", stage.stage),
                                           FONT_COMMODORE, 60, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2TC(0, -130));
    addContentChild(titleLabel);
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
        line->setPosition(Vec2BC(0, BRICK_ORIGIN_Y));
        line->setContentSize(Size(BRICK_LAYER_WIDTH, 8));
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
        line->setPosition(Vec2MC(-BRICK_LAYER_WIDTH*0.5f, 0));
        line->setContentSize(Size(8, SB_WIN_SIZE.height));
        addContentChild(line);
    }
    
    // right
    {
        auto line = LayerColor::create(Color4B(255,0,0,255*0.5f));
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_M);
        line->setPosition(Vec2MC(BRICK_LAYER_WIDTH*0.5f, 0));
        line->setContentSize(Size(8, SB_WIN_SIZE.height));
        addContentChild(line);
    }
#endif
    
    auto getLayerBottom = [=]() -> float {
        
        if( brickLayers.size() == 0 ) {
            return BRICK_ORIGIN_Y;
        }
        
        return SB_BOUNDING_BOX_IN_WORLD(brickLayers[brickLayers.size()-1]).getMinY();
    };
    
    auto createLayer = [=](float paddingY) -> BrickLayer* {
      
        auto layer = BrickLayer::create();
        layer->setAnchorPoint(ANCHOR_MT);
        layer->setPosition(Vec2BC(0, getLayerBottom() - paddingY));
        this->addContentChild(layer);
        
        brickLayers.push_back(layer);
        
        return layer;
    };
    
    auto createBrick = [=](const BrickData &data) -> SBAnimationSprite* {
        
        auto brick = SBAnimationSprite::create(BRICK_ANIMATION(data, BrickImageType::IDLE));
        // auto brick = SBAnimationSprite::create(BRICK_ANIMATION(data, BrickImageType::DAMAGE), 1);
        brick->setScale(BRICK_SCALE);
        
        return brick;
    };
    
    // 1. idle, 100%
    // 2. idle, 100%, 보스 2개, 일반 브릭 2x2
    // 3. damage, 100%
    // 4. idle, 70%
    auto addBricks = [=](BrickDataList brickList, float paddingY) {
        
        auto layer = createLayer(paddingY);
        
        for( auto brickData : brickList ) {
            auto brick = createBrick(brickData);
            
            // 더 이상 추가할 수 없으면 새 레이어 생성
            if( !layer->add(brick) ) {
                layer = createLayer(BRICK_PADDING_Y);
                layer->add(brick);
            }
        }
        
        layer->addFinish();
    };
    
    // 보스 브릭
    // auto layer = addBricks(stage.bossBrickList);
    addBricks(bossBricks, 0);
    
    // 일반 브릭
//    normalBricks.push_back(Database::getBrick("brick_10003"));
//    normalBricks.push_back(Database::getBrick("brick_10009"));
    
    addBricks(normalBricks, 30);
    
    // 브릭 리스트 생성
    for( auto layer : brickLayers ) {
        SBCollection::addAll(brickNodes, layer->getBricks());
    }
}

/* BrickLayer Begin */

StageClearPopup::BrickLayer::BrickLayer() {
}

StageClearPopup::BrickLayer::~BrickLayer() {
}

bool StageClearPopup::BrickLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
 
    setContentSize(Size(BRICK_LAYER_WIDTH, 0));
    
    brickPosX = BRICK_MARGIN_X;
    
    return true;
}

/**
 * 브릭 추가
 */
bool StageClearPopup::BrickLayer::add(SBAnimationSprite *brick) {
    
    auto brickSize = getBrickSize(brick);
    
    // 정해진 너비를 초과할 경우 추가하지 않음
    float width = getBrickTotalWidth();
    width += BRICK_MARGIN_X * 2;
    width += brickSize.width + BRICK_PADDING_X;
    
    if( bricks.size() > 1 ) {
        width += BRICK_PADDING_X * (bricks.size()-1);
    }
    
//    auto childrenSize = getChildrenBoundingSize();
//    childrenSize.width += BRICK_MARGIN_X * 2;
//    childrenSize.width += brickSize.width + BRICK_PADDING_X;
    
    if( width > BRICK_LAYER_WIDTH ) {
        addFinish();
        return false;
    }
    
    // 브릭 추가
    brick->setAnchorPoint(ANCHOR_ML);
    brick->runAnimation();
    addChild(brick);
    
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
    
    bricks.push_back(brick);
    
    // 브릭 X좌표 설정
//    brick->setPositionX(brickPosX);
//    brickPosX += brickSize.width + BRICK_PADDING_X;
    
    // 레이어 높이 설정
    if( brickSize.height > getContentSize().height ) {
        setContentSize(Size(getContentSize().width, brickSize.height));
    }
    
    return true;
}

/**
 * 브릭 추가 끝
 */
void StageClearPopup::BrickLayer::addFinish() {
    
    // 레이어 너비 설정
//    auto childrenSize = getChildrenBoundingSize();
//    setContentSize(Size(childrenSize.width, getContentSize().height));

    // 브릭 X좌표 일괄 설정
//    brickPosX = 0;
//    float paddingX = (getContentSize().width - getChildrenBoundingSize().width) / (bricks.size()-1);
//
//    for( auto brick : bricks ) {
//        brick->setPositionX(brickPosX);
//        brickPosX += getBrickSize(brick).width + paddingX;
//    }

    // 브릭 X좌표 설정
    float paddingX = (getContentSize().width - getBrickTotalWidth()) / (bricks.size()+1);
    float posX = paddingX;
    
    for( auto brick : bricks ) {
        brick->setPositionX(posX);
        posX += getBrickSize(brick).width + paddingX;
    }
    
    // 브릭 Y좌표 설정
    for( auto brick : bricks ) {
        brick->setPositionY(getContentSize().height*0.5f);
    }
}

Size StageClearPopup::BrickLayer::getChildrenBoundingSize() {
    
    Vec2 min(INT_MAX, INT_MAX);
    Vec2 max(INT_MIN, INT_MIN);
    
    auto children = getChildren();
    
    for( auto child : children ) {
        Rect r = SBNodeUtils::getBoundingBoxInWorld(child, child->getScaleX(), child->getScaleY());
        
        min.x = MIN(r.getMinX(), min.x);
        min.y = MIN(r.getMinY(), min.y);
        max.x = MAX(r.getMaxX(), max.x);
        max.y = MAX(r.getMaxY(), max.y);
    }
    
    return Size(max - min);
}

float StageClearPopup::BrickLayer::getBrickTotalWidth() {
    
    float w = 0;
    
    for( auto brick : bricks ) {
        w += getBrickSize(brick).width;
    }
    
    return w;
}

Size StageClearPopup::BrickLayer::getBrickSize(Node *brick) {
    
    auto size = brick->getContentSize();
    size.width *= brick->getScaleX();
    size.height *= brick->getScaleY();
    
    return size;
}

/* BrickLayer End */

/**
 * 획득 재화 초기화
 */
void StageClearPopup::initMoney() {
    
}

/**
 * 메뉴 초기화
 */
void StageClearPopup::initMenu() {
    
    // 다음 스테이지 이동 버튼
    auto nextBtn = SBButton::create(DIR_IMG_MAIN + "RSP_btn_start.png");
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setPosition(Vec2BC(0, 150));
    addContentChild(nextBtn);
    
    nextBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onNextListener);
    });
}

/**
 * 등장 연출
 */
void StageClearPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, FADE_DURATION);
    
    // 브릭 등장 연출
    for( auto brick : brickNodes ) {
        brick->setVisible(false);
    }
    
    brickEnterIndex = 0;
    const string SCHEDULER = "BRICK_ENTER";
    
    schedule([=](float dt) {
        
        brickNodes[brickEnterIndex++]->setVisible(true);
        
        if( brickEnterIndex == brickNodes.size() ) {
            this->unschedule(SCHEDULER);
        }
    }, 0.1f, SCHEDULER);
}

/**
 * 퇴장 연출
 */
void StageClearPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, FADE_DURATION);
}

/**
 * 등장 연출 완료
 */
void StageClearPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
