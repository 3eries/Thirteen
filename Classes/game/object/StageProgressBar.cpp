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
    
    auto gageBg = Sprite::create(DIR_IMG_GAME + "game_level_gage_bg.png");
    gageBg->setAnchorPoint(ANCHOR_M);
    gageBg->setPosition(Vec2TC(0, -58));
    addChild(gageBg);
    
    gage = ProgressTimer::create(Sprite::create(DIR_IMG_GAME + "game_level_gage.png"));
    gage->setType(ProgressTimer::Type::BAR);
    gage->setMidpoint(Vec2(0,0));
    gage->setBarChangeRate(Vec2(1, 0));
    gage->setAnchorPoint(ANCHOR_M);
    gage->setPosition(Vec2MC(gageBg->getContentSize(), 0, 0));
    gage->setPercentage(0);
    gageBg->addChild(gage);
    
    levelLabel = Label::createWithTTF("LEVEL 1", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    levelLabel->setTextColor(Color4B::BLACK);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(Vec2MC(gageBg->getContentSize(), 0, 0));
    gageBg->addChild(levelLabel);
}

void StageProgressBar::initGameListener() {
    
    StringList events({
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_RESTART,
        GAME_EVENT_STAGE_CLEAR,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::STAGE_CHANGED: {
                auto stage = (StageData*)userData;
                this->onStageChanged(*stage);
            } break;
                
            case GameEvent::STAGE_RESTART: {
                auto stage = (StageData*)userData;
                this->onStageRestart(*stage);
            } break;
                
            case GameEvent::STAGE_CLEAR: {
                auto stage = (StageData*)userData;
                this->onStageClear(*stage);
            } break;
                
            default: break;
        }
    }, this);
}

void StageProgressBar::setPercentage(float percentage, bool withAction) {
    
    gage->stopAllActions();
    
    if( withAction ) {
        auto updatePercentage = [=](float f) {
            gage->setPercentage(f);
        };
        auto numberAction = ActionFloat::create(0.2f, gage->getPercentage(), percentage,
                                                updatePercentage);
        gage->runAction(numberAction);
        
    } else {
        gage->setPercentage(percentage);
    }
}

/**
 * 스테이지 변경
 */
void StageProgressBar::onStageChanged(const StageData &stage) {
    
    setPercentage(0);
    levelLabel->setString(STR_FORMAT("LEVEL %d", stage.stage));
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



