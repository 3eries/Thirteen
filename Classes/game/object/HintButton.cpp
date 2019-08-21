//
//  HintButton.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 20/08/2019.
//

#include "HintButton.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"

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
    // game_bg_hint_number.png Vec2TL(88, -23) , Size(32, 32)
    // 9 size:27 Vec2TL(88, -23) , Size(14, 20)
    
    // 터치 이벤트
    setTouchEnabled(true);
    
    addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
       
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN: {
                contentView->setScale(1 - ButtonZoomScale::NORMAL);
            } break;
            
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED: {
                contentView->setScale(1);
            } break;
                
            default: break;
        }
    });
    
    addClickEventListener([=](Ref*) {
        onHintListener();
    });
    
    return true;
}

void HintButton::onClick() {
    
}
