//
//  HintButton.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 20/08/2019.
//

#include "HintButton.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"

#include "GetHintPopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

HintButton::HintButton() :
onHintListener(nullptr) {
}

HintButton::~HintButton() {
}

bool HintButton::init() {
    
    if( !Widget::init() ) {
        return false;
    }
    
    // game_btn_hint.png Vec2TL(54, -54) , Size(74, 74)
    auto bg = Sprite::create(DIR_IMG_GAME + "game_btn_hint.png");
    auto size = bg->getContentSize();
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2TL(54, -54));
    setContentSize(size);
    
    auto contentView = Node::create();
    contentView->setAnchorPoint(ANCHOR_M);
    contentView->setPosition(Vec2MC(size, 0, 0));
    contentView->setContentSize(size);
    addChild(contentView);
    
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(size, 0, 0));
    contentView->addChild(bg);
    
    // 힌트 아이템 개수
    hintCountView = Sprite::create(DIR_IMG_GAME + "game_bg_hint_number.png");
    hintCountView->setAnchorPoint(ANCHOR_M);
    hintCountView->setPosition(contentView->convertToNodeSpace(Vec2TL(88, -23)));
    contentView->addChild(hintCountView);
    
    hintPlusIcon = Sprite::create(DIR_IMG_GAME + "game_bg_hint_number_plus.png");
    hintPlusIcon->setAnchorPoint(ANCHOR_M);
    hintPlusIcon->setPosition(hintCountView->getPosition());
    contentView->addChild(hintPlusIcon);
    
    hintCountLabel = Label::createWithTTF("", FONT_ROBOTO_BLACK, 27, Size::ZERO,
                                          TextHAlignment::CENTER, TextVAlignment::CENTER);
    hintCountLabel->setTextColor(Color4B::WHITE);
    hintCountLabel->setAnchorPoint(ANCHOR_M);
    hintCountLabel->setPosition(Vec2MC(hintCountView->getContentSize(), 0, 0));
    hintCountView->addChild(hintCountLabel);
 
    {
        auto listener = EventListenerCustom::create(DIRECTOR_EVENT_UPDATE_HINT_COUNT, [=](EventCustom *event) {
            this->updateHintCount();
        });
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    updateHintCount();
    
    // 터치 이벤트
    setTouchEnabled(true);
    
    addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
       
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN: {
                contentView->setScale(1 + ButtonZoomScale::NORMAL);
            } break;
            
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED: {
                contentView->setScale(1);
            } break;
                
            default: break;
        }
    });
    
    addClickEventListener([=](Ref*) {
        this->onClick();
    });
    
    return true;
}

/**
 * 버튼이 클릭되었습니다
 */
void HintButton::onClick() {
    
    // 힌트 없음, 광고 보기
    if( User::getHintCount() == 0 ) {
        auto popup = GetHintPopup::create();
        SceneManager::getInstance()->getScene()->addChild(popup, ZOrder::POPUP_BOTTOM);
        
        // 힌트 획득 리스너
        popup->setOnGetListener([=](int amount) {
            
            User::getHint(amount);
            
            hintCountView->runAction(Sequence::create(ScaleTo::create(0.1f, 1.2f),
                                                      ScaleTo::create(0.1f, 1.0f), nullptr));
        });
    }
    // 힌트 사용
    else {
        User::useHint();
        onHintListener();
    }
}

/**
 * 힌트 개수를 업데이트 합니다
 */
void HintButton::updateHintCount() {
    
    hintCountView->stopAllActions();
    int count = User::getHintCount();
    
    if( count == 0 ) {
        hintCountView->setVisible(false);
        hintPlusIcon->setVisible(true);
    } else {
        hintCountView->setVisible(true);
        hintPlusIcon->setVisible(false);
        
        hintCountLabel->stopAllActions();
        hintCountLabel->setString(TO_STRING(count));
    }
}
