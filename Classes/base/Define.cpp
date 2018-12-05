//
//  Define.cpp
//
//  Created by seongmin hwang
//

#include "Define.h"

#include "SceneManager.h"
#include "GiftManager.hpp"
#include "PopupManager.hpp"
#include "../game/GameManager.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

void removeListeners(Ref *target) {
    
    iap::IAPHelper::getInstance()->removeListener(target);
    AdsHelper::getInstance()->getEventDispatcher()->removeListener(target);
    GiftManager::getInstance()->removeListener(target);
    PopupManager::getInstance()->removeListener(target);
    GameManager::getEventDispatcher()->removeListener(target);
}
