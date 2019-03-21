//
//  User.hpp
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef User_hpp
#define User_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class User : public cocos2d::Ref {
public:
    static User* getInstance();
    static void destroyInstance();
    
    ~User();
    
private:
    User();
    
public:
    void init();
    void initOwnedBallSkins();
    
    static void          ownBallSkin(const std::string &ballId);
    static StringList    getOwnedBallSkins();
    static bool          isOwnedBallSkin(const std::string &ballId);
    
    static void          setSelectedBallSkin(const std::string &ballId);
    static std::string   getSelectedBallSkin();
    
    static bool          isOwnRemoveAdsItem();
    static void          setOwnRemoveAdsItem(bool owned);
    static void          removeAds();
    
    static bool          checkReview(float popupDelay = 0);
    
private:
    StringList ballSkins;
};

#endif /* User_hpp */
