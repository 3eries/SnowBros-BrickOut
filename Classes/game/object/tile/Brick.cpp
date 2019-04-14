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
#include "../friend/FriendBall.hpp"

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
def(def),
data(def.data),
originalHp(def.hp),
hp(def.hp),
prevHp(0),
onBreakListener(nullptr),
bg(nullptr),
image(nullptr),
imageType(BrickImageType::NONE),
isRunningDamageWhiteEffect(false) {
}

Brick::~Brick() {
 
    ballHitAnims.clear();
    friendsBallHitAnims.clear();
}

bool Brick::init() {
    
    if( !Game::Tile::init() ) {
        return false;
    }
    
    initPhysics();
    initBg();
    initImage();

    if( def.tile.pos != INVALID_TILE_POSITION ) {
        setTilePosition(def.tile.pos);
    }
    
    setImageFlippedX(def.tile.isFlippedX);
    setImageFlippedY(def.tile.isFlippedY);
    
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
    
    Size size(getPhysicsSize() + Size(BRICK_PHYSICS_PADDING_X, BRICK_PHYSICS_PADDING_Y));
    
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
    
    bg = Sprite::create(ContentResourceHelper::getBrickBackgroundFile(data, isElite(), 1));
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(getContentSize(), 0, 0));
    // bg->setColor(Color3B(0,0,0));
    // bg->setOpacity(255*0.3f);
    addChild(bg, (int)ZOrder::BG);
}

/**
 * 이미지 초기화
 */
void Brick::initImage() {
    
    image = SBAnimationSprite::create();
    image->setAnchorPoint(ANCHOR_M);
    image->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(image, (int)ZOrder::IMAGE);
    
    setImage(BrickImageType::IDLE, true);
}

/**
 * HP 게이지 초기화
 */
void Brick::initHpGage() {
    
    if( isInfinityHp() ) {
        return;
    }
    
    auto is1x1 = [=]() {
        
        return getRows() == 1 && getColumns() == 1;
    };
    
    auto addLabel = [=](int fontSize) -> Label* {
        
        hpGage.label = Label::createWithTTF(TO_STRING(hp), FONT_COMMODORE, fontSize,
                                            Size::ZERO, TextHAlignment::CENTER, TextVAlignment::TOP);
        hpGage.label->setAnchorPoint(ANCHOR_MT);
        hpGage.label->setTextColor(Color4B::WHITE);
        hpGage.label->enableOutline(Color4B::BLACK, 3);
        this->addChild(hpGage.label, (int)ZOrder::HP_LABEL);
        
        return hpGage.label;
    };
    
    auto getGageBgFile = [=]() {
      
        return DIR_CONTENT_BRICK + string(is1x1() ? "game_gage_brick_bg.png" : "game_gage_brick_big_bg.png");
    };
    
    auto getGageFile = [=]() {
        
        string file = "game_gage_brick_green.png";
        
        if( isElite() || data.isSpecial() )   file = "game_gage_brick_yellow.png";
        else if( data.isBoss() )              file = "game_gage_brick_big_red.png";
        
        return DIR_CONTENT_BRICK + file;
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
    
    // HP Gage
    string bgFile = getGageBgFile();
    auto bgTexture = Director::getInstance()->getTextureCache()->addImage(bgFile);
    auto bgSize = bgTexture->getContentSize();
    
    hpGage.bg = Scale9Sprite::create(Rect(bgSize.width*0.5f - 10, bgSize.height*0.5f- 2, 20, 4),
                                     bgFile);
    hpGage.bg->setContentSize(Size(getContentSize().width*0.9f, bgSize.height));
    hpGage.bg->setAnchorPoint(ANCHOR_MT);
    hpGage.bg->setPosition(Vec2TC(getContentSize(), 0, is1x1() ? -9 : -8));
    addChild(hpGage.bg, (int)ZOrder::HP_BAR);
    
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
        case BrickImageType::HIDE:        image->setAnimation(anim);       break;
        default:
            CCASSERT(false, "Brick::setImage error.");
            break;
    }
    
    // image->setColor(Color3B(255, 255*0.5f, 255*0.5f));
    
    if( isRunAnimation ) {
        image->runAnimation();
    }
}

void Brick::setImageFlippedX(bool flippedX) {
    
    image->setFlippedX(flippedX);
}

void Brick::setImageFlippedY(bool flippedY) {
    
    image->setFlippedY(flippedY);
}

/**
 * 타일 비활성화
 */
void Brick::inactiveTile() {
    
    Game::Tile::inactiveTile();
    
    setOnBreakListener(nullptr);
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
    
    setVisible(false);
    setNeedRemove(true);
    
    const int ROWS    = 6 * getData().width;
    const int COLUMNS = 6 * getData().height;
    
    const float PARTICLE_WIDTH = getContentSize().width / ROWS;
    const float PARTICLE_HEIGHT = getContentSize().height / COLUMNS;
    const Size  PARTICLE_SIZE(PARTICLE_WIDTH, PARTICLE_HEIGHT);
    
    const Vec2 CENTER = getPosition();
    const Vec2 ORIGIN = CENTER - getAnchorPointInPoints();
    
    random_device rd;
    mt19937 engine(rd());
    
    for( int row = 0; row < ROWS; ++row ) {
        for( int column = 0; column < COLUMNS; ++column ) {
            const Vec2 PARTICLE_ORIGIN = ORIGIN + Vec2(row * PARTICLE_WIDTH, column * PARTICLE_HEIGHT);
            const Vec2 PARTICLE_CENTER = PARTICLE_ORIGIN + (PARTICLE_SIZE*0.5f);
            
            auto particle = LayerColor::create(Color4B(data.color));
            particle->setIgnoreAnchorPointForPosition(false);
            particle->setAnchorPoint(ANCHOR_M);
            particle->setPosition(PARTICLE_CENTER);
            particle->setContentSize(PARTICLE_SIZE);
            getParent()->addChild(particle, getLocalZOrder() + SBZOrder::BOTTOM);

            const float DURATION = 0.8f;
            
            {
                float diff = PARTICLE_CENTER.x - CENTER.x;
                float ranX = SBMath::random(1,3) * 0.5f;
                float dist = diff * ranX;
                
                auto moveX = MoveBy::create(DURATION*1.1f, Vec2(dist, 0));
                particle->runAction(moveX);
                
                float ranY = 50 * (SBMath::random(1,10) * 0.1f);
                auto moveY = MoveBy::create(DURATION*1.1f, Vec2(0, -360 - ranY));
                particle->runAction(moveY);
            }
             
            // scale out
            {
                auto delay = DelayTime::create(DURATION*0.5f);
                auto scaleOut = ScaleTo::create(DURATION*0.5f, 0.5f);
                particle->runAction(Sequence::create(delay, scaleOut, nullptr));
            }
            
            // fade out
            {
                auto delay = DelayTime::create(DURATION*0.6f);
                auto fadeOut = FadeOut::create(DURATION*0.4f);
                auto remove = RemoveSelf::create();
                particle->runAction(Sequence::create(delay, fadeOut, remove, nullptr));
            }
            
            // rotate
            /*
            {
                int ran = ((arc4random() % 3)+2) * 3;
                float a;
                
                if( row < 2 )         a = -ran;
                else if( row > 3 )    a = ran;
                else                  a = ran * (arc4random() % 2 == 0 ? 1 : -1);
                particle->runAction(RotateBy::create(DURATION*0.5f, a));
            }
            */
        }
    }
    
    // particle_brick.plist
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
 * 볼 히트 연출
 */
void Brick::runBallHitAction(Ball *ball, Vec2 contactPoint) {
    
    const bool isFriendBall = dynamic_cast<FriendBall*>(ball);
    auto &anims = !isFriendBall ? ballHitAnims : friendsBallHitAnims;
    
    // 제거된 애니메이션 정리
    SBCollection::remove(anims, [](SBAnimationSprite *anim) -> bool {
        return !anim->getParent() && !anim->isRunning();
    });
    
    // 해당 좌표에 애니메이션이 없을때만 생성
    auto contactPointAnims = SBCollection::find(anims, [contactPoint](SBAnimationSprite *anim) -> bool {
        auto p = anim->getPosition();
        return (int)p.x == (int)contactPoint.x && (int)p.y == (int)contactPoint.y;
    });
    
    if( contactPointAnims.size() == 0 ) {
        auto anim = ball->createHitAnimation();
        anim->setAnchorPoint(ANCHOR_M);
        anim->setPosition(contactPoint);
        // GameManager::getInstance()->getView()->addChild(anim, SBZOrder::MIDDLE);
        getParent()->addChild(anim, getLocalZOrder() + SBZOrder::MIDDLE);
        
        anims.pushBack(anim);
        
        anim->runAnimation([=](Node*) {
            anim->removeFromParent();
        });
    }
}

/**
 * 프렌즈볼에 의한 데미지 연출
 */
void Brick::runFriendBallDamageAction(FriendBall *ball, Vec2 contactPoint) {
    
    auto anim = ball->createBrickDamageAnimation(this, contactPoint);
    if( !anim ) {
        return;
    }
    
    anim->setAnchorPoint(ANCHOR_M);
    anim->setPosition(Vec2MC(getContentSize(), 0,0));
    addChild(anim, (int)Brick::ZOrder::DAMAGE_EFFECT + ball->getData().brickDamageEffectZOrder);
}

/**
 * 볼 & 벽돌 충돌
 */
bool Brick::onContactBrick(Ball *ball, Game::Tile *brick, Vec2 contactPoint) {
    
    if( isBroken() ) {
        Log::w("이미 깨진 브릭에 충돌 이벤트 발생!!").showMessageBox();
        return false;
    }
    
    runBallHitAction(ball, contactPoint);
    sufferDamage(ball, contactPoint);
    
    return true;
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
bool Brick::sufferDamage(Ball *ball, Vec2 contactPoint, bool withBallDamageAction) {
    
    if( !canDamage() ) {
        return false;
    }
    
    const int damage = ball->getDamage();
    setHp(hp - damage);
    
    // 브릭 애니메이션 변경
    if( imageType != BrickImageType::DAMAGE ) {
        setImage(BrickImageType::DAMAGE, false);
        
        image->runAnimation([=](Node*) {
            this->setImage(BrickImageType::IDLE, true);
        });
        
        // 흰색 브릭 반짝 연출
        auto frame = image->getSpriteFrame();
        auto tex = ContentResourceHelper::getInstance()->addBrickWhiteTexture(frame->getTexture(),
                                                                              image->isFlippedX(),
                                                                              image->isFlippedY());
        auto whiteEffect = Sprite::createWithTexture(tex);
        whiteEffect->setAnchorPoint(image->getAnchorPoint());
        whiteEffect->setPosition(image->getPosition());
        addChild(whiteEffect);
        
        SBDirector::postDelayed(this, [=]() {
            whiteEffect->removeFromParent();
        }, 0.03f);
    }
    
    // 프렌즈볼 데미지 연출
    if( withBallDamageAction ) {
        auto friendBall = dynamic_cast<FriendBall*>(ball);
        
        if( friendBall ) {
            runFriendBallDamageAction(friendBall, contactPoint);
        }
    }
    
    return true;
    
    // 데미지 연출
    /*
    if( damage > 1 && !isInfinityHp() ) {
        auto gageBox = SB_BOUNDING_BOX_IN_WORLD(hpGage.label);
        
        auto label = Label::createWithTTF(TO_STRING(damage), FONT_COMMODORE, 45,
                                          Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2(gageBox.getMidX(), gageBox.getMidY()));
        label->setTextColor(Color4B::WHITE);
        label->enableOutline(Color4B::BLACK, 3);
        getParent()->addChild(label, SBZOrder::BOTTOM);
        
        label->setScale(0.2f);
        
        auto scale = ScaleTo::create(0.2f, 1);
        auto delay = DelayTime::create(0.3f);
        auto remove = RemoveSelf::create();
        label->runAction(Sequence::create(scale, delay, remove, nullptr));
    }
     */
    
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

bool Brick::sufferDamage(Ball *ball, Vec2 contactPoint) {
    
    return sufferDamage(ball, contactPoint, dynamic_cast<FriendBall*>(ball));
}

/**
 * HP 설정
 */
void Brick::setHp(int hp, bool updateUI) {
    
    if( isInfinityHp() ) {
        return;
    }
    
    this->prevHp = this->hp;
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
    
    bg->setTexture(ContentResourceHelper::getBrickBackgroundFile(data, isElite(), bgStep));
    
    // 게이지 & 라벨
    hpGage.gage->stopAllActions();
    hpGage.label->stopAllActions();
    
    const int diff = prevHp - hp;
    
    if( diff == 1 ) {
        hpGage.gage->setScaleX(hpRatio);
        hpGage.label->setString(TO_STRING(hp));
    }
    // 이전 HP와 2이상 차이나면 감소 연출
    else {
        const float DURATION = 0.1f;
        hpGage.gage->runAction(ScaleTo::create(DURATION, hpRatio, hpGage.gage->getScaleY()));
        
        auto numberAction = ActionFloat::create(DURATION, prevHp, hp, [=](float value) {
            int i = (int)value;
            hpGage.label->setString(TO_STRING(i));
        });
        hpGage.label->runAction(numberAction);
    }
}

void Brick::setBgVisible(bool isVisible) {
    
    if( bg ) {
        bg->setVisible(isVisible);
    }
}

void Brick::setHpVisible(bool isVisible) {
    
    if( isInfinityHp() ) {
        return;
    }
    
    hpGage.bg->setVisible(isVisible);
    hpGage.label->setVisible(isVisible);
}

/**
 * 데미지를 입을 수 있는 상태인지 반환합니다
 */
bool Brick::canDamage() {
    
    // 깨지지 않고 무한 HP가 아니면 데미지를 입을 수 있습니다
    return !isBroken() && !isInfinityHp();
}

/**
 * 데미지를 입을 수 있는 좌표인지 반환합니다
 */
bool Brick::canDamagePoint(Vec2 p) {
    return true;
}

bool Brick::isElite() {
    return def.elite;
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
