//
//  CreditPopup.cpp
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#include "CreditPopup.hpp"

#include "Define.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// Member
struct Member {
    string image;
    string name;
    string job;
    
    Member(string _image, string _name, string _job) :
    image(_image), name(_name), job(_job) {}
};

static const vector<Member> MEMBERS = {
    Member("RSP_icon_character_mw9.png", "MR9", "PROGRAMMING"),
    Member("RSP_icon_character_w0n.png", "W0N", "DESIGN"),
    Member("RSP_icon_character_b30.png", "B3O", "ART"),
};

CreditPopup::CreditPopup() : BasePopup(PopupType::CREDIT) {
}

CreditPopup::~CreditPopup() {
}

bool CreditPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initCredit();
    
    return true;
}

void CreditPopup::onEnter() {
    
    BasePopup::onEnter();
}

/**
 * 등장 연출
 */
void CreditPopup::runEnterAction(SBCallback onFinished) {
    
    const float DURATION = 0.3f;
    
    BasePopup::runEnterAction(DURATION, onFinished);
    
    SBNodeUtils::recursiveCascadeOpacityEnabled(this, true);
    
    // fade in
    setOpacity(0);
    
    auto fadeIn = FadeIn::create(DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        this->retain();
        
        if( onFinished ) {
            onFinished();
        }
        
        this->onEnterActionFinished();
        this->release();
    });
    runAction(Sequence::create(fadeIn, callFunc, nullptr));
}

/**
 * 등장 연출 완료
 */
void CreditPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 화면 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode, SBZOrder::TOP);
    
    touchNode->addClickEventListener([=](Ref*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismiss();
    });
}

void CreditPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void CreditPopup::initCredit() {
    
    // Title
    auto title = Sprite::create(DIR_IMG_COMMON + "logo_3eries.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -114));
    addChild(title);
    
    // Member
    auto members = MEMBERS;
    random_shuffle(members.begin(), members.end());
    
    const float PADDING_Y = 267;
    const float ORIGIN_Y = Vec2MC(0, 290).y;
    
    for( int i = 0; i < MEMBERS.size(); ++i ) {
        float posY = ORIGIN_Y - (PADDING_Y*i);
        
        auto member = members[i];
        
        auto icon = Sprite::create(DIR_IMG_COMMON + member.image);
        icon->setAnchorPoint(ANCHOR_M);
        icon->setPosition(Vec2(SB_WIN_SIZE.width*0.5f, posY));
        addChild(icon);
        
        {
            float offset = 5 * (arc4random() % 2 == 0 ? 1 : -1);
            
            auto rotate1 = RotateTo::create(0.4f, offset);
            auto rotate2 = RotateTo::create(0.4f, -offset);
            auto seq = Sequence::create(rotate1, rotate2, nullptr);
            icon->runAction(RepeatForever::create(seq));
        }
        
        posY -= 89;
        
        auto job = Label::createWithTTF(member.job, FONT_RETRO, 50,
                                        Size::ZERO,
                                        TextHAlignment::CENTER,
                                        TextVAlignment::CENTER);
        job->setColor(Color3B(255,211,0));
        job->enableOutline(Color4B(0,0,0,255), 4);
        job->setAnchorPoint(ANCHOR_M);
        job->setPosition(Vec2(SB_WIN_SIZE.width*0.5f, posY));
        addChild(job);
        
        posY -= 52;
        
        auto name = Label::createWithTTF(member.name, FONT_RETRO, 50,
                                         Size::ZERO,
                                         TextHAlignment::CENTER,
                                         TextVAlignment::CENTER);
        name->setColor(Color3B(255,255,255));
        name->enableOutline(Color4B(0,0,0,255), 4);
        name->setAnchorPoint(ANCHOR_M);
        name->setPosition(Vec2(SB_WIN_SIZE.width*0.5f, posY));
        addChild(name);
    }
}

