//
//  ExitAlertPopup.cpp
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#include "ExitAlertPopup.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "GameUIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

ExitAlertPopup::ExitAlertPopup() : BasePopup(PopupType::EXIT_APP),
onExitAppListener(nullptr) {
    
}

ExitAlertPopup::~ExitAlertPopup() {
}

bool ExitAlertPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ExitAlertPopup::onEnter() {
    
    BasePopup::onEnter();
}

void ExitAlertPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void ExitAlertPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_give_up.png Vec2MC(0, 0) , Size(552, 568)
    stoneBg = Sprite::create(DIR_IMG_COMMON + "RSP_popup_bg_give_up.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 0));
    addContentChild(stoneBg);
    
    const Size SIZE(stoneBg->getContentSize());
    
    // title
    // RSP_popup_title_give_up.png Vec2MC(0, 210) , Size(392, 60)
    auto title = Sprite::create(DIR_IMG_COMMON + "RSP_popup_title_give_up.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(SIZE, 0, 210));
    stoneBg->addChild(title);
    
    // YES
    auto yesBtn = SBButton::create("YES", FONT_COMMODORE, 50);
    yesBtn->setAnchorPoint(ANCHOR_MR);
    yesBtn->setPosition(Vec2MC(SIZE, -60, -150));
    yesBtn->setContentSize(Size(100, 80));
    stoneBg->addChild(yesBtn);
    
    yesBtn->setOnClickListener([=](Node*) {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        SB_SAFE_PERFORM_LISTENER(this, onExitAppListener);
    });
    
    // NO
    auto noBtn = SBButton::create("NO", FONT_COMMODORE, 50);
    noBtn->setAnchorPoint(ANCHOR_ML);
    noBtn->setPosition(Vec2MC(SIZE, 60, -150));
    noBtn->setContentSize(Size(100, 80));
    stoneBg->addChild(noBtn);
    
    noBtn->setOnClickListener([=](Node*) {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
}

/**
 * 등장 연출
 */
void ExitAlertPopup::runEnterAction(float duration, SBCallback onFinished) {
    
    BasePopup::runEnterAction(duration, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, duration);
}

void ExitAlertPopup::runEnterAction(SBCallback onFinished) {
    
    runEnterAction(SLIDE_DURATION, onFinished);
}

/**
 * 퇴장 연출
 */
void ExitAlertPopup::runExitAction(float duration, SBCallback onFinished) {
    
    BasePopup::runExitAction(duration, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, duration);
}

void ExitAlertPopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(SLIDE_DURATION, onFinished);
}

/**
 * 등장 연출 완료
 */
void ExitAlertPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->dismissWithAction();
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
}

