//
//  ClearPopup.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 27/08/2019.
//

#include "ClearPopup.hpp"

#include "Define.h"

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
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    dismissWithAction();
    
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
    
    onNextListener();
    dismiss();
}

/**
 * 등장 연출
 */
void ClearPopup::runEnterAction(SBCallback onFinished) {
    
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
}

