//
//  ClearPopup.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 27/08/2019.
//

#include "ClearPopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "PopupManager.hpp"
#include "../GameManager.hpp"

#include "ExitAlertPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

ClearPopup::ClearPopup() : BasePopup(PopupType::GAME_CLEAR),
onNextListener(nullptr) {
    
}

ClearPopup::~ClearPopup() {
}

bool ClearPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ClearPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

bool ClearPopup::onBackKeyReleased() {
    
    if( !BasePopup::onBackKeyReleased() ) {
        return false;
    }
    
    // 앱 종료 알림 팝업 생성
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    auto popup = (ExitAlertPopup*)PopupManager::show(PopupType::EXIT_APP);
    popup->setOnExitAppListener([=]() {
        SBSystemUtils::exitApp();
    });
    
    return true;
}

void ClearPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void ClearPopup::initContentView() {
    
    BasePopup::initContentView();
    
    popupBg = Sprite::create(DIR_IMG_GAME + "game_btn_complete.png");
    popupBg->setAnchorPoint(ANCHOR_M);
    popupBg->setPosition(Vec2MC(0, 0));
    addContentChild(popupBg);
}

void ClearPopup::onClick() {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    retain();
    onNextListener();
    release();
}

/**
 * 등장 연출
 */
void ClearPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 효과음
    SBDirector::postDelayed(this, [=]() {
        SBAudioEngine::playEffect(SOUND_LEVEL_CLEAR);
    }, SLIDE_DURATION*0.5f);
    
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
void ClearPopup::runExitAction(SBCallback onFinished) {
    
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
void ClearPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 화면 터치 이벤트
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->onClick();
    });
    
    // 전면 광고 노출
    if( !User::isRemovedAds() ) {
        auto level = GAME_MANAGER->getStage();
        
        if( level.stage >= 4 && AdsHelper::isInterstitialLoaded() ) {
            SBDirector::getInstance()->setScreenTouchLocked(true);
            SBDirector::postDelayed(this, [=]() {
                
                auto listener = AdListener::create(AdType::INTERSTITIAL);
                listener->setTarget(this);
                listener->onAdOpened = [=]() {
                };
                listener->onAdClosed = [=]() {
                    // 전면 광고 닫힌 후 0.2초 후에 터치 재개
                    SBDirector::postDelayed(this, [=]() {
                        SBDirector::getInstance()->setScreenTouchLocked(false);
                    }, 0.2f);
                };
                AdsHelper::showInterstitial(listener);
                
            }, 0.2f);
        }
    }
}

