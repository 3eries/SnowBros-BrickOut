//
//  StageClearPopup.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 19/12/2018.
//

#ifndef StageClearPopup_hpp
#define StageClearPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "ContentManager.hpp"

class StageClearPopup : public BasePopup {
public:
    static StageClearPopup* create(const StageData &stage, BrickDataList bricks);
    ~StageClearPopup();
    
private:
    StageClearPopup();
    
    bool init(const StageData &stage, BrickDataList bricks);
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initTitle();
    void initBrick();
    void initMoney();
    void initMenu();
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    StageData stage;
    BrickDataList bossBricks;
    BrickDataList normalBricks;
    
    CC_SYNTHESIZE(SBCallback, onNextListener, OnNextListener);
    
    /**
     * 브릭이 표시되는 1개 층
     */
    class BrickLayer : public cocos2d::Node {
    public:
        CREATE_FUNC(BrickLayer);
        ~BrickLayer();
        
        bool add(SBAnimationSprite *brick);
        void addFinish();
        
        cocos2d::Size        getChildrenBoundingSize();
        float                getBrickTotalWidth();
        static cocos2d::Size getBrickSize(cocos2d::Node *brick);
        
    private:
        BrickLayer();
        
        bool init() override;
        
        CC_SYNTHESIZE_READONLY(std::vector<cocos2d::Node*>, bricks, Bricks);
        float brickPosX;
    };
    
    std::vector<BrickLayer*>    brickLayers;
    std::vector<cocos2d::Node*> brickNodes;
    int brickEnterIndex;
};

#endif /* StageClearPopup_hpp */
