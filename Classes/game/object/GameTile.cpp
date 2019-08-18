//
//  GameTile.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameTile.hpp"

#include "Define.h"
#include "GameDefine.h"

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

void GameTile::setNumber(int number) {
    
    this->number = number;
    numberLabel->setString(TO_STRING(number));
}

void GameTile::setSelected(bool isSelected) {
    
    this->selected = isSelected;
    bg->setColor(isSelected ? TILE_SELECTED_COLOR : TILE_NORMAL_COLOR);
}
