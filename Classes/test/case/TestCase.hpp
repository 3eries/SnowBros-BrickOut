//
//  TestCase.hpp
//
//  Created by seongmin hwang on 09/02/2019.
//

#ifndef TestCase_hpp
#define TestCase_hpp

#include <stdio.h>

#include "BaseScene.hpp"
#include "TestHelper.hpp"

/** @class TestCase
 * @brief 모든 테스트 케이스의 부모 클래스입니다
 */
class TestCase : public BaseScene {
public:
    TestCase();
    ~TestCase();
    
protected:
    virtual bool init() override;
    virtual bool onBackKeyReleased() override;
    
    virtual void setTitle(const std::string &title);
    
private:
    cocos2d::Label *titleLabel;
};

#endif /* TestCase_hpp */
