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
    
    setContentSize(Size(120, 60));
    
    auto contentView = Node::create();
    contentView->setAnchorPoint(ANCHOR_M);
    contentView->setPosition(Vec2MC(getContentSize(), 0, 0));
    contentView->setContentSize(getContentSize());
    addChild(contentView);
    
    // 배경
    auto bg = LayerColor::create(Color4B::WHITE);
    bg->setIgnoreAnchorPointForPosition(false);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(getContentSize(), 0, 0));
    bg->setContentSize(getContentSize());
    contentView->addChild(bg);
    
    // 타이틀
    auto title = Label::createWithTTF("HINT", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    title->setTextColor(Color4B::BLACK);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(getContentSize(), 0, 0));
    contentView->addChild(title);
    
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
