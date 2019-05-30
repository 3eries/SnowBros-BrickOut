//
//  BoostPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 12/05/2019.
//

#ifndef BoostPopup_hpp
#define BoostPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "../../content/data/model/WorldData.h"

#define NS_BOOST_BEGIN         namespace Boost {
#define NS_BOOST_END           }
#define USING_NS_BOOST         using namespace Boost

NS_BOOST_BEGIN

class ListCell;

NS_BOOST_END

class BoostPopup : public BasePopup {
public:
    static BoostPopup* create();
    ~BoostPopup();
    
private:
    enum Tag {
        BANNER             = 10,
        WORLD_LABEL,
        
        BTN_BOOST          = 100,
        BTN_WORLD_PREV,
        BTN_WORLD_NEXT,
        BTN_CLOSE,
    };
    
private:
    BoostPopup();
    
    bool init() override;
    void onEnter() override;
    void dismiss() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initBg();
    void initWorldView();
    
    void updateWorld(int idx);
    
    void onSelectedCell(Boost::ListCell *cell);
    void onClick(cocos2d::Node *sender) override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(std::function<void(StageData)>, onBoostListener, OnBoostListener);
    CC_SYNTHESIZE(SBCallback, onClosedListener, OnClosedListener);
    
    StageData boostMaxStage;
    
    int currentWorldIndex;
    std::vector<cocos2d::Node*> worldViews;
    
    Boost::ListCell *selectedCell;
};

NS_BOOST_BEGIN

/**
 * ListCell
 */
class ListCell : public cocos2d::ui::Widget {
public:
    static ListCell* create(const StageData &data, int cellIndex);
    ~ListCell();
    
    void setSelected(bool selected);
    void setLocked(bool locked);
    
private:
    ListCell();
    
    bool init(const StageData &data, int cellIndex);
    void updateCostTextColor();
    
    CC_SYNTHESIZE_READONLY(StageData, data, Data);
    CC_SYNTHESIZE_READONLY(int, cellIndex, CellIndex);
    CC_SYNTHESIZE(std::function<void(ListCell*)>, onClickListener, OnClickListener);
    
    SB_SYNTHESIZE_READONLY_BOOL(selected, Selected);
    SB_SYNTHESIZE_READONLY_BOOL(locked, Locked);
    
    cocos2d::Label *costLabel;
    cocos2d::Node *selectedImage;
    cocos2d::Node *lockedLayer;
};

NS_BOOST_END

#endif /* BoostPopup_hpp */
