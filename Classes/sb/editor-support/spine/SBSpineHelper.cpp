//
//  SBSpineHelper.cpp
//
//  Created by seongmin hwang on 2018. 6. 15..
//

#include "SBSpineHelper.hpp"

#include "../../base/SBMacros.h"

USING_NS_CC;
using namespace spine;
using namespace std;

SkeletonAnimation* SBSpineHelper::runAnimation(SBCallback completeListener,
                                               const string &jsonFile, const string &animName,
                                               bool autoRemove) {
    
    auto anim = SkeletonAnimation::createWithJsonFile(jsonFile);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    
    if( animName != "" ) {
        runAnimation(completeListener, anim, animName, autoRemove);
    }
    
    return anim;
}

void SBSpineHelper::runAnimation(SBCallback completeListener,
                                 SpineAnimation *anim, const string &animName,
                                 bool autoRemove) {
    
    CCASSERT(anim != nullptr, "SBSpineHelper::runAnimation error: invalid anim.");
    CCASSERT(animName != "", "SBSpineHelper::runAnimation error: invalid anim name.");
    
    auto track = anim->setAnimation(0, animName, false);
    
    anim->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
        
        SB_SAFE_PERFORM_LISTENER(anim, completeListener);
        
        if( !autoRemove ) {
            return;
        }
        
        // remove
        anim->clearTracks();
        anim->setVisible(false);
        
        auto delay = DelayTime::create(0.1f);
        auto remove = RemoveSelf::create();
        anim->runAction(Sequence::create(delay, remove, nullptr));
    });
}

void SBSpineHelper::clearAnimation(SpineAnimation *anim, string clearAnimName) {
    
    CCASSERT(anim != nullptr, "SBSpineHelper::clearAnimation error: invalid anim.");
    
    anim->clearTracks();
    
    if( clearAnimName != "" ) {
        anim->setAnimation(0, clearAnimName, false);
    }
    
    anim->update(0);
}
