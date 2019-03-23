//
//  GameConfigEdit.cpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#include "GameConfigEdit.hpp"

#include "Define.h"
#include "ContentManager.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameConfigEdit::GameConfigEdit() {
}

GameConfigEdit::~GameConfigEdit() {
}

bool GameConfigEdit::init() {
    
    if( !TestCase::init() ) {
        return false;
    }
    
    setTitle("Game Config Edit");
    
    // 메뉴 초기화
    posY = -110;
    
    auto editBoxSize = Size(SB_WIN_SIZE.width*0.4f, 50);
    auto fontSize = 45;
    
    auto addEditBox = [=](string title) -> EditBox* {
        
        auto editBox = EditBox::create(editBoxSize, DIR_IMG_COMMON + "orange_edit.png");
        editBox->setAnchorPoint(ANCHOR_TR);
        editBox->setPosition(Vec2TR(-10, posY));
        editBox->setFontColor(Color3B::BLACK);
        editBox->setFontSize(fontSize);
        editBox->setDelegate(this);
        this->addChild(editBox);
        
        auto titleLabel = Label::createWithSystemFont(title, "", fontSize,
                                                      Size(0, editBoxSize.height),
                                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->setAnchorPoint(ANCHOR_TL);
        titleLabel->setPosition(Vec2TL(10, posY));
        titleLabel->setTextColor(Color4B::WHITE);
        addChild(titleLabel);
        
        posY -= (editBoxSize.height+10);
        
        return editBox;
    };
    
    // 1. start stage
    {
        auto editBox = addEditBox(STR_FORMAT("start stage(last:%d)", Database::getLastStage().stage));
        editBox->setTag(Tag::EDIT_BOX_STAGE);
        editBox->setText(TO_STRING(TEST_HELPER->getStartStage()).c_str());
        editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    }
    
    // 2. first ball count
    {
        auto editBox = addEditBox("first ball count");
        editBox->setTag(Tag::EDIT_BOX_BALL_COUNT);
        editBox->setText(TO_STRING(TEST_HELPER->getFirstBallCount()).c_str());
        editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    }
    
    return true;
}

void GameConfigEdit::editBoxReturn(EditBox *editBox) {
 
    string text = editBox->getText();
    
    switch( editBox->getTag() ) {
        case Tag::EDIT_BOX_STAGE: {
            
            if( text == "" ) {
                editBox->setText(TO_STRING(TEST_HELPER->getStartStage()).c_str());
            } else {
                // update stage
                int stage = TO_INTEGER(text);
                stage = MIN(Database::getLastStage().stage, stage);
                
                TEST_HELPER->setStartStage(stage);
                editBox->setText(TO_STRING(stage).c_str());
            }
            
            // update ball count
            TEST_HELPER->setFirstBallCount(Database::getStageFirstBallCount(TEST_HELPER->getStartStage()));
            
            auto ballCountBox = getChildByTag<EditBox*>(Tag::EDIT_BOX_BALL_COUNT);
            ballCountBox->setText(TO_STRING(TEST_HELPER->getFirstBallCount()).c_str());
        } break;
            
        case Tag::EDIT_BOX_BALL_COUNT: {
            if( text == "" ) {
                editBox->setText(TO_STRING(TEST_HELPER->getFirstBallCount()).c_str());
            } else {
                TEST_HELPER->setFirstBallCount(TO_INTEGER(text));
            }
        } break;
            
        default: break;
    }
}
