//
//  DemoViewEdit.hpp
//  SnowBrosBrickOut-mobile
//
//  Created by seongmin hwang on 22/04/2019.
//

#ifndef DemoViewEdit_hpp
#define DemoViewEdit_hpp

#include <stdio.h>

#include "TestCase.hpp"

class DemoViewEdit : public TestCase {
public:
    CREATE_FUNC(DemoViewEdit);
    ~DemoViewEdit();
    
private:
    DemoViewEdit();
    
    bool init() override;
    
private:
    float posY;
};

#endif /* DemoViewEdit_hpp */
