//
//  Brick.cpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 06/12/2018.
//

#include "Brick.hpp"

#include "Define.h"
#include "../../GameDefine.h"
#include "../../GameManager.hpp"

#include "../Ball.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

Brick* Brick::create(const BrickDef &def) {
    
    auto brick = new Brick(def);
    
    if( brick && brick->init() ) {
        brick->autorelease();
        return brick;
    }
    
    delete brick;
    return nullptr;
}

Brick::Brick(const BrickDef &def) : Game::Tile(def.data.width, def.data.height, def.floorData),
data(def.data),
originalHp(def.hp),
hp(def.hp),
elite(false),
onBreakListener(nullptr),
bg(nullptr),
image(nullptr),
imageType(BrickImageType::NONE),
isRunningDamageWhiteEffect(false) {
}

Brick::~Brick() {
    
}

bool Brick::init() {
    
    if( !Game::Tile::init() ) {
        return false;
    }
    
    initPhysics();
    initBg();
    initImage();
    
    return true;
}

void Brick::onEnter() {
    
    Game::Tile::onEnter();
    
    initHpGage();
}

/**
 * 물리 객체 초기화
 */
void Brick::initPhysics() {
    
    Size size(getPhysicsSize());
    
    b2BodyDef bodyDef;
    bodyDef.userData = (SBPhysicsObject*)this;
    
    b2PolygonShape box;
    box.SetAsBox(PTM(size.width*0.5f), PTM(size.height*0.5f));
    
    auto body = GameManager::getPhysicsManager()->getWorld()->CreateBody(&bodyDef);
    setBody(body);
    
    b2Filter filter;
    filter.categoryBits = PhysicsCategory::BRICK;
    filter.maskBits = PhysicsCategory::BALL;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.1f;
    fixtureDef.restitution = 1;
    fixtureDef.friction = 0;
    fixtureDef.filter = filter;
    body->CreateFixture(&fixtureDef);
    
    // 리스너 등록
    auto listener = GamePhysicsListener::create();
    listener->setTarget(this);
    listener->setContactTarget(this);
    listener->onContactBrick = CC_CALLBACK_3(Brick::onContactBrick, this);
    GameManager::getPhysicsManager()->addListener(listener);
}

/**
 * 배경 초기화
 */
void Brick::initBg() {
    
    bg = Sprite::create(ContentResourceHelper::getBrickBackgroundFile(getRows(), getColumns(), 1));
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(getContentSize(), 0, 0));
    // bg->setColor(Color3B(0,0,0));
    // bg->setOpacity(255*0.3f);
    addChild(bg);
}

/**
 * 이미지 초기화
 */
void Brick::initImage() {
    
    image = SBAnimationSprite::create();
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(image);
    
    setImage(BrickImageType::IDLE, true);
}

/**
 * HP 게이지 초기화
 */
void Brick::initHpGage() {
    
    auto is1x1 = [=]() {
        
        return getRows() == 1 && getColumns() == 1;
    };
    
    auto addLabel = [=](int fontSize) -> Label* {
        
        hpGage.label = Label::createWithTTF(TO_STRING(hp), FONT_COMMODORE, fontSize,
                                            Size::ZERO, TextHAlignment::CENTER, TextVAlignment::TOP);
        hpGage.label->setAnchorPoint(ANCHOR_MT);
        hpGage.label->setTextColor(Color4B::WHITE);
        hpGage.label->enableOutline(Color4B::BLACK, 3);
        this->addChild(hpGage.label, 2);
        
        return hpGage.label;
    };
    
    auto getGageBgFile = [=]() {
      
        return DIR_BRICK + string(is1x1() ? "game_gage_brick_bg.png" : "game_gage_brick_big_bg.png");
    };
    
    auto getGageFile = [=]() {
        
        string file = "game_gage_brick_green.png";
        
        if( elite )                       file = "game_gage_brick_yellow.png";
        else if( data.isBoss() )          file = "game_gage_brick_big_red.png";
        else if( this->isInfinityHp() )   file = "game_gage_brick_big_blue.png";
        
        return DIR_BRICK + file;
    };
    
    auto addGage = [=]() -> Sprite* {
        
        auto bgSize = hpGage.bg->getContentSize();
        
        string gageFile = getGageFile();
        auto gageTexture = Director::getInstance()->getTextureCache()->addImage(gageFile);
        auto gageSize = gageTexture->getContentSize();
        
        hpGage.gage = Scale9Sprite::create(Rect(gageSize.width*0.5f - 10, gageSize.height*0.5f- 2, 20, 4),
                                           gageFile);
        hpGage.gage->setContentSize(hpGage.bg->getContentSize() + Size(-8, -8));
        hpGage.gage->setAnchorPoint(ANCHOR_ML);
        hpGage.gage->setPosition(Vec2ML(bgSize, (bgSize.width-hpGage.gage->getContentSize().width)*0.5f, 0));
        hpGage.gage->setScaleX(1);
        hpGage.bg->addChild(hpGage.gage);
        
        return hpGage.gage;
    };
    
    // HP Label
    if( is1x1() ) {
        addLabel(20);
        hpGage.label->setPosition(Vec2TC(getContentSize(), 0, 5));
    } else {
        addLabel(25);
        hpGage.label->setPosition(Vec2TC(getContentSize(), 0, 6));
    }
    
    if( isInfinityHp() ) {
        hpGage.label->setTextColor(Color4B(254,239,12,255));
        hpGage.label->setString("MAX");
    }
    
    // HP Gage
    string bgFile = getGageBgFile();
    auto bgTexture = Director::getInstance()->getTextureCache()->addImage(bgFile);
    auto bgSize = bgTexture->getContentSize();
    
    hpGage.bg = Scale9Sprite::create(Rect(bgSize.width*0.5f - 10, bgSize.height*0.5f- 2, 20, 4),
                                     bgFile);
    hpGage.bg->setContentSize(Size(getContentSize().width*0.9f, bgSize.height));
    hpGage.bg->setAnchorPoint(ANCHOR_MT);
    hpGage.bg->setPosition(Vec2TC(getContentSize(), 0, is1x1() ? -9 : -8));
    addChild(hpGage.bg, 1);
    
    addGage();

    // gage effect
    hpGage.gageEffect = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    hpGage.gageEffect->setVisible(false);
    hpGage.gageEffect->setAnchorPoint(hpGage.gage->getAnchorPoint());
    hpGage.gageEffect->setPosition(hpGage.gage->getPosition());
    hpGage.gageEffect->setScaleY(hpGage.gage->getContentSize().height / hpGage.gageEffect->getContentSize().height);
    hpGage.bg->addChild(hpGage.gageEffect);
}

/**
 * 벽돌 이미지 설정
 */
void Brick::setImage(BrickImageType type, bool isRunAnimation) {
    
    this->imageType = type;
    
    auto anim = BRICK_ANIMATION(data, type);
    
    switch( type ) {
        case BrickImageType::IDLE:        image->setAnimation(anim);       break;
        case BrickImageType::DAMAGE:      image->setAnimation(anim, 1);    break;
        default:
            CCASSERT(false, "Brick::setImage error.");
            break;
    }
    
    // image->setColor(Color3B(255, 255*0.5f, 255*0.5f));
    
    if( isRunAnimation ) {
        image->runAnimation();
    }
}

/**
 * 볼 & 벽돌 충돌
 */
void Brick::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    if( isBroken() ) {
        Log::w("이미 깨진 벽돌 충돌 이벤트 발생!!").showMessageBox();
        return;
    }
    
    sufferDamage(ball->getDamage());
}

/**
 * 등장
 */
void Brick::enterWithAction() {
    
    Game::Tile::enterWithAction();
}

/**
 * 제거
 */
void Brick::removeWithAction() {
    
    Game::Tile::removeWithAction();
 
    setOnBreakListener(nullptr);
    
    setVisible(false);
    setNeedRemove(true);
    
    // 연출
    const int ROWS = 6;
    const float W = getContentSize().width / ROWS;
    const int COLUMNS = getContentSize().height / W;
    const float H = getContentSize().height / COLUMNS;
    
    const Vec2 ORIGIN = getPosition() - getAnchorPointInPoints();
    
    for( int row = 0; row < ROWS; ++row ) {
        for( int column = 0; column < COLUMNS; ++column ) {
            auto particle = LayerColor::create(Color4B(data.color));
            particle->setIgnoreAnchorPointForPosition(false);
            particle->setAnchorPoint(ANCHOR_BL);
            particle->setPosition(ORIGIN + Vec2(row*W, column*H));
            particle->setContentSize(Size(W,H));
            getParent()->addChild(particle, SBZOrder::BOTTOM);
            
            const float DURATION = 0.7f;
            
            // move out
            {
                int ran = ((arc4random() % 3)+3) * 5;
                float x;
                
                if( row < 2 )         x = -ran;
                else if( row > 3 )    x = ran;
                else                  x = ((arc4random() % 3) * 2) * (arc4random() % 2 == 0 ? 1 : -1);
                
                auto moveX = MoveBy::create(DURATION*0.9f, Vec2(x, 0));
                particle->runAction(moveX);
                
                auto moveY1 = MoveBy::create(DURATION*0.2f, Vec2(0, ((arc4random() % 4)+3) * 3));
                auto moveY2 = MoveBy::create(DURATION*0.8f, Vec2(0, -320));
                auto remove = RemoveSelf::create();
                particle->runAction(Sequence::create(moveY1, moveY2, remove, nullptr));
            }
            
            // fade out
            {
                auto delay = DelayTime::create(DURATION*0.6f);
                auto fadeOut = FadeOut::create(DURATION*0.4f);
                particle->runAction(Sequence::create(delay, fadeOut, nullptr));
            }
            
            // rotate
            {
                int ran = ((arc4random() % 3)+2) * 3;
                float a;
                
                if( row < 2 )         a = -ran;
                else if( row > 3 )    a = ran;
                else                  a = ran * (arc4random() % 2 == 0 ? 1 : -1);
                particle->runAction(RotateBy::create(DURATION*0.5f, a));
            }
        }
    }
    
    /*
    auto particle = ParticleSystemQuad::create(DIR_IMG_GAME + "particle_brick.plist");
    particle->setAnchorPoint(ANCHOR_M);
    particle->setPosition(getPosition());
    particle->setStartColor(Color4F(data.color));
    particle->setEndColor(Color4F(data.color));
    particle->setAutoRemoveOnFinish(true);
    // particle->setScale(0.2f);
    getParent()->addChild(particle, SBZOrder::BOTTOM);
    
    if( data.isBoss() ) {
        particle->setScale(particle->getScale() * 1.5f);
    }
    */
}

/**
 * 벽돌이 깨졌습니다
 */
void Brick::onBreak() {
    
    setHpVisible(false);
    
    if( onBreakListener ) {
        onBreakListener(this);
    }
}

/**
 * 데미지를 입습니다
 */
void Brick::sufferDamage(int damage) {
    
    setHp(hp - damage);
    
    // 브릭 애니메이션 변경
    if( imageType != BrickImageType::DAMAGE ) {
        setImage(BrickImageType::DAMAGE, false);
        
        image->runAnimation([=](Node*) {
            this->setImage(BrickImageType::IDLE, true);
        });
    
        // 흰색 브릭 반짝 연출
        auto whiteEffect = createWhiteBrickEffect();
        addChild(whiteEffect);
        
        SBDirector::postDelayed(this, [=]() {
            whiteEffect->removeFromParent();
        }, 0.03f);
    }
    
    // 흰색 브릭 반짝 연출
    /*
    if( !isRunningDamageWhiteEffect ) {
        isRunningDamageWhiteEffect = true;
        
        auto whiteEffect = createWhiteBrickEffect();
        addChild(whiteEffect);
        
        SBDirector::postDelayed(this, [=]() {
            isRunningDamageWhiteEffect = false;
            whiteEffect->removeFromParent();
        }, 0.03f);
    }
    */
     
    // 게이지 반짝 연출
    /*
    if( hpGage.gageEffect->getNumberOfRunningActions() == 0 ) {
        float scaleX = hpGage.gage->getContentSize().width / hpGage.gageEffect->getContentSize().width;
        scaleX *= getHpRatio();
        
        hpGage.gageEffect->setVisible(true);
        hpGage.gageEffect->setScaleX(scaleX);
        
        auto delay = DelayTime::create(0.04f);
        auto hide = Hide::create();
        hpGage.gageEffect->runAction(Sequence::create(delay, hide, nullptr));
    }
     */
}

/**
 * HP 설정
 */
void Brick::setHp(int hp, bool updateUI) {
    
    if( isInfinityHp() ) {
        return;
    }
    
    this->hp = MAX(0, hp);
    
    if( updateUI ) {
        updateHpUI();
    }
    
    if( isBroken() ) {
        onBreak();
    }
}

void Brick::updateHpUI() {
    
    if( isInfinityHp() ) {
        return;
    }
    
    const float hpRatio = getHpRatio();
    
    // 벽돌 배경
    int bgStep = 0;
    
    if( hpRatio >= 0.5f )        bgStep = 1;
    else if( hpRatio >= 0.25f )  bgStep = 2;
    else                         bgStep = 3;
    
    bg->setTexture(ContentResourceHelper::getBrickBackgroundFile(getRows(), getColumns(), bgStep));
    
    // 게이지
    hpGage.gage->setScaleX(hpRatio);
    hpGage.label->setString(TO_STRING(hp));
}

void Brick::setBgVisible(bool isVisible) {
    
    bg->setVisible(isVisible);
}

void Brick::setHpVisible(bool isVisible) {
    
    hpGage.bg->setVisible(isVisible);
    hpGage.label->setVisible(isVisible);
}

bool Brick::isBoss() {
 
    return data.isBoss();
}

bool Brick::isInfinityHp() {
    
    return hp == BRICK_INFINITY_HP;
}

bool Brick::isBroken() {
    
    return hp == 0;
}

float Brick::getHpRatio() {
    
    return (float)hp / originalHp;
}

Node* Brick::createWhiteBrickEffect() {
    
    auto size = getContentSize();
    
    auto stencil = Sprite::createWithSpriteFrame(image->getSpriteFrame());
    stencil->setAnchorPoint(ANCHOR_M);
    stencil->setPosition(Vec2MC(size, 0,0));
    stencil->setFlippedX(image->isFlippedX());
    stencil->setFlippedY(image->isFlippedY());
    
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setAnchorPoint(ANCHOR_M);
    clippingNode->setPosition(Vec2MC(size, 0,0));
    clippingNode->setContentSize(size);
    clippingNode->setAlphaThreshold(0);

    auto drawNode = DrawNode::create();
    drawNode->drawSolidRect(Vec2::ZERO, stencil->getContentSize(), Color4F::WHITE);
    clippingNode->addChild(drawNode);
    
    return clippingNode;
}
