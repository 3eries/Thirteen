//
//  GetHintPopup.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 27/08/2019.
//

#include "GetHintPopup.hpp"

#include "Define.h"
#include "User.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

GetHintPopup::GetHintPopup() : BasePopup(PopupType::GET_HINT),
onGetListener(nullptr) {
    
}

GetHintPopup::~GetHintPopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool GetHintPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void GetHintPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

bool GetHintPopup::onBackKeyReleased() {

    if( !BasePopup::onBackKeyReleased() ) {
        return false;
    }

    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    dismissWithAction();

    return true;
}

void GetHintPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void GetHintPopup::initContentView() {
    
    BasePopup::initContentView();
    
    popupBg = Sprite::create(DIR_IMG_GAME + "game_btn_view_ads.png");
    popupBg->setAnchorPoint(ANCHOR_M);
    popupBg->setPosition(Vec2MC(0, 0));
    addContentChild(popupBg);
}

void GetHintPopup::onClick() {
    
    // TODO: 리워드 광고
    onGetListener(2);
    dismissWithAction();
}

/**
 * 등장 연출
 */
void GetHintPopup::runEnterAction(SBCallback onFinished) {
    
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
void GetHintPopup::runExitAction(SBCallback onFinished) {
    
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
void GetHintPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 팝업 배경 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
    
    // 팝업 배경 터치
    auto bgTouchNode = SBNodeUtils::createTouchNode();
    bgTouchNode->setAnchorPoint(popupBg->getAnchorPoint());
    bgTouchNode->setPosition(popupBg->getPosition());
    bgTouchNode->setContentSize(popupBg->getContentSize());
    addChild(bgTouchNode);
    
    bgTouchNode->addClickEventListener([=](Ref*) {
        this->onClick();
    });
}
