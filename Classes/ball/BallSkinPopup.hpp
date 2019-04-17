//
//  BallSkinPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 14/03/2019.
//

#ifndef BallSkinPopup_hpp
#define BallSkinPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "../content/data/model/BallSkinData.h"

#define NS_BALL_SKIN_BEGIN         namespace BallSkin {
#define NS_BALL_SKIN_END           }
#define USING_NS_BALL_SKIN         using namespace BallSkin

NS_BALL_SKIN_BEGIN

class UnlockButton;
class ListCell;

typedef std::vector<ListCell*> Cells;
typedef std::function<void(ListCell*)> OnCellListener;

NS_BALL_SKIN_END

class BallSkinPopup : public BasePopup {
public:
    static BallSkinPopup* create();
    ~BallSkinPopup();
    
private:
    BallSkinPopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initBg();
    void initButton();
    void initListView();
    
    BallSkin::ListCell* getCell(const std::string &ballId);
    
    void onSelectedCell(BallSkin::ListCell *cell);
    void onClickUnlockButton();
    void onClickSelectButton();
    
    void showButton(cocos2d::ui::Widget *btn, bool withAction);
    void hideButton(cocos2d::ui::Widget *btn, bool withAction);
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    cocos2d::ui::ListView*  ballListView;
    
    BallSkin::Cells         cells;
    BallSkin::ListCell*     selectedCell;
    
    BallSkin::UnlockButton* unlockButton;
    cocos2d::ui::Button*    selectButton;
};

NS_BALL_SKIN_BEGIN

/**
 * UnlockButton
 */
class UnlockButton : public cocos2d::ui::Widget {
public:
    CREATE_FUNC(UnlockButton);
    ~UnlockButton();
    
    void show();
    void hide();
    
    void setData(const BallSkinData &data);
    
private:
    UnlockButton();
    
    bool init() override;
    
    CC_SYNTHESIZE_READONLY(BallSkinData, data, Data);
    cocos2d::Label *coinLabel;
};

/**
 * ListRow
 */
class ListRow : public cocos2d::ui::Widget {
public:
    static ListRow* create(const Cells &cells);
    ~ListRow();
    
private:
    ListRow();
    
    bool init(const Cells &cells);
    
    CC_SYNTHESIZE(int, rowIndex, RowIndex);
    CC_SYNTHESIZE_READONLY(Cells, cells, Cells);
};

/**
 * ListCell
 */
struct ListCellData {
    bool isWatchAd;
    BallSkinData ballData;
    int amount;
    
    ListCellData() : isWatchAd(false), amount(0) {}
};

class ListCell : public cocos2d::ui::Widget {
public:
    static ListCell* create(const ListCellData &data);
    ~ListCell();
    
    void setUnlocked(bool isUnlocked);
    void setSelected(bool isSelected);
    
private:
    ListCell();
    
    bool init(const ListCellData &data);
    
    CC_SYNTHESIZE(int, rowIndex, RowIndex);
    CC_SYNTHESIZE(OnCellListener, onClickListener, OnClickListener);
    CC_SYNTHESIZE_READONLY(ListCellData, data, Data);
    SB_SYNTHESIZE_READONLY_BOOL(unlocked, Unlocked);
    SB_SYNTHESIZE_READONLY_BOOL(selected, Selected);
    
    enum ZOrder {
        BG = 0,
        ICON,
        LOCKED,
        SELECTED,
        COIN,
    };
    
    cocos2d::Sprite *bg;
    cocos2d::Sprite *selectedImage;
    cocos2d::Sprite *ball;
    cocos2d::Node *coinLayer;
    cocos2d::Node *lockedLayer;
};

NS_BALL_SKIN_END

#endif /* BallSkinPopup_hpp */
