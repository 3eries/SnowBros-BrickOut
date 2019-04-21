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

#include "ContentManager.hpp"

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
    void initOwnedFriends();
    
    static void             setCoin(int i);
    static void             addCoin(int i);
    static bool             spendCoin(int i);
    static int              getCoin();
    static bool             isEnoughCoin(int i = 0);
    
    static void             ownBallSkin(const std::string &ballId);
    static StringList       getOwnedBallSkins();
    static bool             isOwnedBallSkin(const std::string &ballId);
    
    static void             setSelectedBallSkin(const std::string &ballId);
    static std::string      getSelectedBallSkin();

    static void             ownFriend(const std::string &friendId);
    static StringList       getOwnedFriends();
    static bool             isOwnedFriend(const std::string &friendId);
    
    static void             setFriendsDeck(StringList friendsDeck);
    static void             setFriendsDeck(FriendDataList friendsDeck);
    static FriendDataList   getFriendsDeck();
    static bool             isExistFriendsDeck(const std::string &friendId);
    
    static bool             isOwnRemoveAdsItem();
    static void             setOwnRemoveAdsItem(bool owned);
    static void             removeAds();
    
    static bool             checkReview(float popupDelay = 0);
    
private:
    StringList ballSkins;       // 소유한 볼스킨
    StringList friends;         // 소유한 프렌즈
    StringList friendsDeck;     // 프렌즈 덱
};

#endif /* User_hpp */
