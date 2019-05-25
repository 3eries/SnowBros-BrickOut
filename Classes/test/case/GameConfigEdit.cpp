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
    
    auto addEditBox = [=]() -> EditBoxComponent {
        
        EditBoxComponent comp;
        
        auto titleLabel = Label::createWithSystemFont("", "", fontSize, Size(0, editBoxSize.height),
                                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->setAnchorPoint(ANCHOR_TL);
        titleLabel->setPosition(Vec2TL(10, posY));
        titleLabel->setTextColor(Color4B::WHITE);
        this->addChild(titleLabel);
        
        auto editBox = EditBox::create(editBoxSize, DIR_IMG_COMMON + "orange_edit.png");
        editBox->setAnchorPoint(ANCHOR_TR);
        editBox->setPosition(Vec2TR(-10, posY));
        editBox->setFontColor(Color3B::BLACK);
        editBox->setFontSize(fontSize);
        editBox->setDelegate(this);
        this->addChild(editBox);
        
        posY -= (editBoxSize.height+10);
        
        comp.titleLabel = titleLabel;
        comp.editBox = editBox;
        
        return comp;
    };

    // 1. start world
    {
        auto comp = addEditBox();
        comp.titleLabel->setString(STR_FORMAT("start world(last:%d)", Database::getLastWorld().world));
        
        comp.editBox->setTag(Tag::EDIT_BOX_WORLD);
        comp.editBox->setText(TO_STRING(TEST_HELPER->getStartWorld()).c_str());
        comp.editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    }
    
    // 2. start stage
    {
        auto comp = addEditBox();
        comp.titleLabel->setTag(Tag::TITLE_STAGE);
        updateStartStageTitle();
        
        comp.editBox->setTag(Tag::EDIT_BOX_STAGE);
        comp.editBox->setText(TO_STRING(TEST_HELPER->getStartStage()).c_str());
        comp.editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    }
    
    // 3. first ball count
    {
        auto comp = addEditBox();
        comp.titleLabel->setString("first ball count");
        
        comp.editBox->setTag(Tag::EDIT_BOX_BALL_COUNT);
        comp.editBox->setText(TO_STRING(TEST_HELPER->getFirstBallCount()).c_str());
        comp.editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    }
    
    return true;
}

void GameConfigEdit::updateStartStageTitle() {
    
    string title = STR_FORMAT("start stage(last:%d)",
                              Database::getWorldLastStage(TEST_HELPER->getStartWorld()).stage);
    getChildByTag<Label*>(Tag::TITLE_STAGE)->setString(title);
}

void GameConfigEdit::editBoxReturn(EditBox *editBox) {
 
    string text = editBox->getText();
    int i = TO_INTEGER(text);
    
    switch( editBox->getTag() ) {
        case Tag::EDIT_BOX_WORLD: {
            // restore
            if( text == "" ) {
                editBox->setText(TO_STRING(TEST_HELPER->getStartWorld()).c_str());
            }
            // update world
            else {
                int world = MIN(Database::getLastWorld().world, i);
                TEST_HELPER->setStartWorld(world);
                
                editBox->setText(TO_STRING(world).c_str());
                updateStartStageTitle();
            }
        } break;
            
        case Tag::EDIT_BOX_STAGE: {
            // restore
            if( text == "" ) {
                editBox->setText(TO_STRING(TEST_HELPER->getStartStage()).c_str());
            }
            // update stage
            else {
                int stage = MIN(Database::getWorldLastStage(TEST_HELPER->getStartWorld()).stage, i);
                TEST_HELPER->setStartStage(stage);
                
                editBox->setText(TO_STRING(stage).c_str());
            }
            
            // update ball count
            int ballCount = Database::getStage(TEST_HELPER->getStartWorld(),
                                               TEST_HELPER->getStartStage()).firstBallCount;
            TEST_HELPER->setFirstBallCount(ballCount);
            
            auto ballCountBox = getChildByTag<EditBox*>(Tag::EDIT_BOX_BALL_COUNT);
            ballCountBox->setText(TO_STRING(ballCount).c_str());
        } break;
            
        case Tag::EDIT_BOX_BALL_COUNT: {
            // restore
            if( text == "" ) {
                editBox->setText(TO_STRING(TEST_HELPER->getFirstBallCount()).c_str());
            }
            // update ball count
            else {
                TEST_HELPER->setFirstBallCount(i);
            }
        } break;
            
        default: break;
    }
}
