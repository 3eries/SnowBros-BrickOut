//
//  GameConfigEdit.hpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#ifndef GameConfigEdit_hpp
#define GameConfigEdit_hpp

#include <stdio.h>

#include "TestCase.hpp"

class GameConfigEdit : public TestCase, public cocos2d::ui::EditBoxDelegate {
public:
    CREATE_FUNC(GameConfigEdit);
    ~GameConfigEdit();
    
private:
    enum Tag {
        TITLE_STAGE         = 100,
        
        EDIT_BOX_WORLD,
        EDIT_BOX_STAGE,
        EDIT_BOX_BALL_COUNT,
    };
    
private:
    GameConfigEdit();
    
    bool init() override;
    
    void updateStartStageTitle();
    
    void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    
private:
    float posY;
    
    struct EditBoxComponent {
        cocos2d::Label *titleLabel;
        cocos2d::ui::EditBox *editBox;
    };
};

#endif /* GameConfigEdit_hpp */
