//
//  GiftManager.hpp
//
//  Created by seongmin hwang on 05/11/2018.
//

#ifndef GiftManager_hpp
#define GiftManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "GiftDefine.h"
#include "GiftListener.hpp"

class GiftBox;

/** @class GiftManager
 * @brief 선물 시스템을 관리합니다
 */
class GiftManager : public cocos2d::Ref {
public:
    static GiftManager* getInstance();
    static void destroyInstance();
    ~GiftManager();
    
private:
    GiftManager();
    
public:
    class Gift : public cocos2d::Ref {
    public:
        GiftType type;
        bool isEnabled;       // 선물 활성화 여부
        bool isCheckEnabled;  // 선물 체크 여부
        GiftBox *box;
        
        Gift() : type(GiftType::NONE), isEnabled(false), isCheckEnabled(true), box(nullptr) {}
    };
    
public:
    void init();
    void initAdListener();
    void initPopupListener();
    
public:
    static void     setEnabled(bool isEnabled);
    static void     setEnabled(GiftType type, bool isEnabled);
    
    static void     setCheckEnabled(bool isCheckEnabled);
    static void     setCheckEnabled(GiftType type, bool isCheckEnabled);
    
    static bool     addGiftBox(GiftType type);
    static bool     removeGiftBox(GiftType type);
    static void     removeAllGiftBox();
    
    static void     showPopup(GiftType type);
    
private:
    void checkGift(GiftType type);
    
    static Gift*    getGift(GiftType type);
    
private:
    CC_SYNTHESIZE(float, checkInterval, CheckInterval); // 선물 체크 간격
    
    std::map<GiftType, Gift*> gifts;
    
public:
    // 선물 박스 클릭
    void onClickBox(GiftType type);
    
    // 선물 보상
    void onGiftRewarded(const GiftRewardItem &item);
    
public:
    void addListener(GiftListener *listener);
    void removeListener(GiftListener *listener);
    void removeListener(cocos2d::Ref *target);
    
private:
    cocos2d::Vector<GiftListener*> listeners;
};

#endif /* GiftManager_hpp */
