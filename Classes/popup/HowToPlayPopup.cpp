//
//  HowToPlayPopup.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 30/08/2019.
//

#include "HowToPlayPopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "UserDefaultKey.h"
#include "GameConfiguration.hpp"

#include "SceneManager.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

HowToPlayPopup::HowToPlayPopup() : BasePopup(PopupType::HOW_TO_PLAY),
animLoopCount(0),
tutorial(false) {
    
}

HowToPlayPopup::~HowToPlayPopup() {
}

bool HowToPlayPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void HowToPlayPopup::onEnter() {
    
    BasePopup::onEnter();
    
    // runEnterAction();
    onEnterActionFinished();
}

bool HowToPlayPopup::onBackKeyReleased() {
    
    if( !BasePopup::onBackKeyReleased() ) {
        return false;
    }
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    dismissWithAction();
    
    return true;
}

void HowToPlayPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void HowToPlayPopup::initContentView() {
    
    BasePopup::initContentView();
    
    anim = SBSkeletonAnimation::create(DIR_IMG_GAME + "howtoplay.json");
    anim->setAnimation(0, "idle", true);
    addContentChild(anim);
}

/**
 * 등장 연출
 */
void HowToPlayPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void HowToPlayPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void HowToPlayPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // run 애니메이션 재생
    auto track = anim->setAnimation(0, "run", true);
    
    // 화면 터치 시 팝업 종료
    auto addTouchEvent = [=]() {
        
        auto touchNode = SBNodeUtils::createTouchNode();
        addChild(touchNode);
        
        touchNode->addClickEventListener([=](Ref*) {
            SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
            
            anim->clearTracks();
            anim->setAnimation(0, "idle", true);
            
            this->dismissWithAction();
            touchNode->removeFromParent();
        });
    };
    
    // 튜토리얼, 애니메이션 1회 필수 시청
    if( tutorial ) {
        anim->setTrackCompleteListener(track, [=](spine::TrackEntry *entry) {
           
            ++animLoopCount;
            
            if( animLoopCount == 1 ) {
                addTouchEvent();
            }
        });
    } else {
        addTouchEvent();
    }
}

