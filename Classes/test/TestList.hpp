//
//  TestList.hpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#ifndef TestList_hpp
#define TestList_hpp

#include <stdio.h>

#include "BaseScene.hpp"
#include "TestHelper.hpp"

/** @class TestList
 * @brief 테스트 리스트가 노출되는 Scene
 */
class TestList : public BaseScene {
public:
    CREATE_FUNC(TestList);
    ~TestList();
    
private:
    TestList();
    
    bool init() override;
    bool onBackKeyReleased() override;
    
    void initScrollView();
    void initTestList();
    
    void addTest(const std::string &title, std::function<Scene*()> getScene);
    
private:
    cocos2d::ui::ScrollView *scrollView;
    std::vector<cocos2d::ui::Widget*> cells;
};

#endif /* TestList_hpp */
