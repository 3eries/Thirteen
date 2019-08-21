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
selected(false),
left(nullptr), right(nullptr), top(nullptr), bottom(nullptr) {
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
    
    bg = Sprite::create(DIR_IMG_GAME + "game_tile.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(TILE_CONTENT_SIZE, 0, 0));
    addChild(bg);

    numberLabel = Label::createWithTTF("", FONT_ROBOTO_BLACK, 40, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    numberLabel->setTextColor(TILE_NUMBER_NORMAL_COLOR);
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

void GameTile::setNearTile(GameTile *left, GameTile *right, GameTile *top, GameTile *bottom) {
    
    this->left = left;
    this->right = right;
    this->top = top;
    this->bottom = bottom;
}

void GameTile::setNumber(int number, bool withAction) {
    
    this->number = number;
    numberLabel->setString(TO_STRING(number));
    
    // 배경색 업데이트
    int bgColorIdx = number-1;
    if( bgColorIdx < TILE_COLORS.size() ) {
        bg->setColor(TILE_COLORS[bgColorIdx]);
    }
    
    // 연출
    if( withAction ) {
        runNumberEnterAction();
    }
}

void GameTile::setSelected(bool isSelected) {
    
    this->selected = isSelected;
    
    bg->setVisible(!isSelected);
    numberLabel->setTextColor(isSelected ? TILE_NUMBER_SELECTED_COLOR : TILE_NUMBER_NORMAL_COLOR);
    
    // FIXME: 디버그용 코드
    numberLabel->stopAllActions();
}

void GameTile::yap() {

    auto scale1 = ScaleTo::create(1.0f, 2.0f);
    auto scale2 = ScaleTo::create(1.0f, 1.0f);
    numberLabel->runAction(RepeatForever::create(Sequence::create(scale1, scale2, nullptr)));
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
