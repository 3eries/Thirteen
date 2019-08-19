//
//  StageProgressBar.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "StageProgressBar.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

StageProgressBar::StageProgressBar() {
}

StageProgressBar::~StageProgressBar() {
}

bool StageProgressBar::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initUI();
    initGameListener();
    
    return true;
}

void StageProgressBar::initUI() {
    
    // scratch_stage_gage_bg.png Vec2TC(-1, -58) , Size(460, 80)
    auto gageBg = Sprite::create(DIR_IMG_GAME + "scratch_stage_gage_bg.png");
    gageBg->setAnchorPoint(ANCHOR_M);
    gageBg->setPosition(Vec2TC(0, -58));
    addChild(gageBg);
    
    gage = ProgressTimer::create(Sprite::create(DIR_IMG_GAME + "scratch_stage_gage.png"));
    gage->setType(ProgressTimer::Type::BAR);
    gage->setMidpoint(Vec2(0,0));
    gage->setBarChangeRate(Vec2(1, 0));
    gage->setAnchorPoint(ANCHOR_M);
    gage->setPosition(Vec2MC(gageBg->getContentSize(), 0, 0));
    gage->setPercentage(0);
    gageBg->addChild(gage);
    
    // 50 roboto_black_40 Vec2TC(-190, -58) , Size(43, 29)
    currStageLabel = Label::createWithTTF("1", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    currStageLabel->setAnchorPoint(ANCHOR_M);
    currStageLabel->setPosition(Vec2TC(-190, -58));
    currStageLabel->setTextColor(Color4B::BLACK);
    addChild(currStageLabel);
    
    // 51 roboto_black_40 Vec2TC(187, -58) , Size(38, 30)
    nextStageLabel = Label::createWithTTF("2", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    nextStageLabel->setAnchorPoint(ANCHOR_M);
    nextStageLabel->setPosition(Vec2TC(190, -58));
    nextStageLabel->setTextColor(Color4B::BLACK);
    addChild(nextStageLabel);
}

void StageProgressBar::initGameListener() {
    
//    auto listener = GameEventListener::create(this);
//    listener->onStageChanged         = CC_CALLBACK_1(StageProgressBar::onStageChanged, this);
//    listener->onStageRestart         = CC_CALLBACK_1(StageProgressBar::onStageRestart, this);
//    listener->onStageClear           = CC_CALLBACK_1(StageProgressBar::onStageClear, this);
//    GameManager::getEventDispatcher()->addListener(listener);
    
    string eventNames[] = {
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_RESTART,
        GAME_EVENT_STAGE_CLEAR,
    };
    
    for( string eventName : eventNames ) {
        /*
         auto listener = eventDispatcher->addCustomEventListener(eventName, [=](EventCustom *event) {
         this->onCustomEvent(HASH_STR(event->getEventName().c_str()));
         });
         */
        auto listener = EventListenerCustom::create(eventName, [=](EventCustom *event) {
            
            switch( GAME_EVENT_ENUMS[eventName] ) {
                case GameEvent::STAGE_CHANGED: {
                    auto stage = (StageData*)event->getUserData();
                    this->onStageChanged(*stage);
                } break;
                    
                case GameEvent::STAGE_RESTART: {
                    auto stage = (StageData*)event->getUserData();
                    this->onStageRestart(*stage);
                } break;
                    
                case GameEvent::STAGE_CLEAR: {
                    auto stage = (StageData*)event->getUserData();
                    this->onStageClear(*stage);
                } break;
                    
                default: break;
            }
        });
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
}

void StageProgressBar::setPercentage(float percentage) {
    
    gage->setPercentage(percentage);
}

/**
 * 스테이지 변경
 */
void StageProgressBar::onStageChanged(const StageData &stage) {
    
    setPercentage(0);
    currStageLabel->setString(TO_STRING(stage.stage));
    nextStageLabel->setString(TO_STRING(stage.stage+1));
}

/**
 * 스테이지 재시작
 */
void StageProgressBar::onStageRestart(const StageData &stage) {
    
    setPercentage(0);
}

/**
 * 스테이지 클리어
 */
void StageProgressBar::onStageClear(const StageData &stage) {
}



