//
//  GameTile.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameTile.hpp"

#include "Define.h"
#include "../GameDefine.h"

USING_NS_CC;
using namespace std;

GameTile* GameTile::create(const TileData &data) {
    
    auto tile = new GameTile();
    
    if( tile && tile->init(data) ) {
        tile->autorelease();
        return tile;
    }
    
    CC_SAFE_DELETE(tile);
    return nullptr;
}

GameTile::GameTile() :
number(0),
selected(false) {
}

GameTile::~GameTile() {
}

bool GameTile::init(const TileData &data) {
    
    if( !Node::init() ) {
        return false;
    }
    
    this->data = data;
    
    setAnchorPoint(ANCHOR_M);
    setContentSize(TILE_CONTENT_SIZE);
    setVisible(!data.isEmpty);
    
    bg = Sprite::create(DIR_IMG_GAME + "game_tile.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(TILE_CONTENT_SIZE, 0, 0));
    bg->setColor(TILE_NORMAL_COLOR);
    addChild(bg);

    numberLabel = Label::createWithTTF("", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    numberLabel->setTextColor(Color4B::BLACK);
    numberLabel->setAnchorPoint(ANCHOR_M);
    numberLabel->setPosition(Vec2MC(TILE_CONTENT_SIZE, 0, 0));
    addChild(numberLabel);
    
    return true;
}

void GameTile::clear(bool withAction) {
    
    if( withAction ) {
        runNumberExitAction();
    }
}

void GameTile::setNumber(int number, bool withAction) {
    
    this->number = number;
    numberLabel->setString(TO_STRING(number));
    
    if( withAction ) {
        runNumberEnterAction();
    }
}

void GameTile::setSelected(bool isSelected) {
    
    this->selected = isSelected;
    bg->setColor(isSelected ? TILE_SELECTED_COLOR : TILE_NORMAL_COLOR);
}

/**
 * 숫자 등장 연출
 */
void GameTile::runNumberEnterAction() {
    
    numberLabel->stopAllActions();
    numberLabel->setScale(0);
    numberLabel->setOpacity(0);
    
    // scale
    auto scale1 = ScaleTo::create(TILE_NUMBER_ENTER_DURATION*0.6f, 1.15f);
    auto scale2 = ScaleTo::create(TILE_NUMBER_ENTER_DURATION*0.4f, 1.0f);
    numberLabel->runAction(Sequence::create(scale1, scale2, nullptr));
    
    // fade in
    numberLabel->runAction(FadeIn::create(TILE_NUMBER_ENTER_DURATION));
}

/**
 * 숫자 퇴장 연출
 */
void GameTile::runNumberExitAction() {

    numberLabel->stopAllActions();
    
    // scale
    numberLabel->runAction(ScaleTo::create(TILE_NUMBER_EXIT_DURATION, 0));
    
    // fade out
    numberLabel->runAction(FadeOut::create(TILE_NUMBER_EXIT_DURATION));
}
