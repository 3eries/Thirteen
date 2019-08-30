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

#define DEBUG_TILE_ID               0

GameTile* GameTile::create() {
    
    auto tile = new GameTile();
    
    if( tile && tile->init() ) {
        tile->autorelease();
        return tile;
    }
    
    CC_SAFE_DELETE(tile);
    return nullptr;
}

GameTile::GameTile() :
number(INVALID_TILE_NUMBER),
selected(false),
left(nullptr), right(nullptr), top(nullptr), bottom(nullptr) {
}

GameTile::~GameTile() {
}

bool GameTile::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setContentSize(TILE_CONTENT_SIZE);
    setCascadeOpacityEnabled(true);
    
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
    
    selectedLine = Node::create();
    selectedLine->setCascadeOpacityEnabled(true);
    selectedLine->setAnchorPoint(ANCHOR_M);
    selectedLine->setPosition(Vec2MC(TILE_CONTENT_SIZE, 0, 0));
    selectedLine->setContentSize(TILE_CONTENT_SIZE);
    addChild(selectedLine);
    
    return true;
}

void GameTile::remove() {
    
    runAction(ScaleTo::create(TILE_EXIT_DURATION, 0)); // scale
    runAction(FadeOut::create(TILE_EXIT_DURATION));    // fade out
    
    SBDirector::postDelayed(this, [=]() {
        this->removeFromParent();
    }, TILE_EXIT_DURATION);
}

void GameTile::clear() {
    
    runAction(ScaleTo::create(TILE_EXIT_DURATION, 0)); // scale
    // runAction(FadeOut::create(TILE_EXIT_DURATION));    // fade out
    
    SBDirector::postDelayed(this, [=]() {
        this->setScale(1);
        this->setOpacity(255);
        this->setVisible(false);
    }, TILE_EXIT_DURATION);
}

void GameTile::setTilePosition(const TilePosition &p) {
    
    this->tilePos = p;
    setPosition(convertTilePosition(p));
}

void GameTile::setTileId(int tileId) {
    
    this->tileId = tileId;
    
#if DEBUG_TILE_ID
    auto label = getChildByTag<Label*>(419);
    
    if( label ) {
        label->setString(TO_STRING(tileId));
    } else {
        label = Label::createWithTTF(TO_STRING(tileId), FONT_ROBOTO_BLACK, 25, Size::ZERO,
                                     TextHAlignment::CENTER, TextVAlignment::BOTTOM);
        label->setTag(419);
        label->setTextColor(TILE_NUMBER_NORMAL_COLOR);
        label->setAnchorPoint(ANCHOR_MB);
        label->setPosition(Vec2BC(TILE_CONTENT_SIZE, 0, 0));
        addChild(label);
    }
#endif
}

void GameTile::setNumber(int number) {
    
    this->number = number;
    numberLabel->setString(TO_STRING(number));
    
    // 배경색 업데이트
    int bgColorIdx = number-1;
    
    if( bgColorIdx < TILE_COLORS.size() ) {
        bg->setColor(TILE_COLORS[bgColorIdx]);
    }
}

void GameTile::setSelected(bool isSelected) {
    
    this->selected = isSelected;
    
    bg->setVisible(!isSelected);
    numberLabel->setTextColor(isSelected ? TILE_NUMBER_SELECTED_COLOR : TILE_NUMBER_NORMAL_COLOR);
    
    selectedLine->stopAllActions();
    selectedLine->removeAllChildren();
    
    // FIXME: 디버그용 코드
    numberLabel->stopAllActions();
}

void GameTile::setNearTile(GameTile *left, GameTile *right, GameTile *top, GameTile *bottom) {
    
    this->left = left;
    this->right = right;
    this->top = top;
    this->bottom = bottom;
}

void GameTile::updateSelectedLine(function<bool(GameTile*)> isTileSelected,
                                  const Color3B &lineColor, bool withAction) {
    
    // 선택된 경우 테두리 생성
    selectedLine->removeAllChildren();
    
    if( withAction ) {
    }
    
    const float LINE_SIZE = TILE_SELECTED_LINE_SIZE;
    
    Vec2 origin;
    Vec2 destination = getContentSize();
    
    auto addLine = [=](Vec2 origin, Vec2 destination) {
        auto line = DrawNode::create();
        line->setAnchorPoint(Vec2::ZERO);
        line->setPosition(Vec2::ZERO);
        line->setContentSize(getContentSize());
        line->drawSolidRect(origin, destination, Color4F(lineColor));
        selectedLine->addChild(line);
    };
    
    auto _isTileSelected = [isTileSelected](GameTile *tile) -> bool {
        return tile && isTileSelected(tile);
    };
    
    // left
    if( _isTileSelected(left) ) {
        // bottom
        if( !_isTileSelected(bottom) || !_isTileSelected(left->getBottom()) ) {
            addLine(Vec2(origin.x + LINE_SIZE, origin.y), Vec2(-TILE_PADDING, LINE_SIZE));
        }
        // top
        if( !_isTileSelected(top) || !_isTileSelected(left->getTop()) ) {
            addLine(Vec2(origin.x + LINE_SIZE, destination.y),
                    Vec2(-TILE_PADDING, destination.y - LINE_SIZE));
        }
    } else {
        addLine(origin, Vec2(LINE_SIZE, destination.y));
    }
    
    // right
    if( _isTileSelected(right) ) {
        // bottom
        if( !_isTileSelected(bottom) || !_isTileSelected(right->getBottom()) ) {
            addLine(Vec2(destination.x, origin.y), Vec2(destination.x + TILE_PADDING, LINE_SIZE));
        }
        // top
        if( !_isTileSelected(top) || !_isTileSelected(right->getTop()) ) {
            addLine(destination, Vec2(destination.x + TILE_PADDING, destination.y - LINE_SIZE));
        }
    } else {
        addLine(Vec2(origin.x + destination.x, origin.y),
                Vec2(destination.x - LINE_SIZE, destination.y));
    }
    
    // top
    if( _isTileSelected(top) ) {
        // left
        if( !_isTileSelected(left) || !_isTileSelected(top->getLeft()) ) {
            addLine(Vec2(origin.x, destination.y - LINE_SIZE),
                    Vec2(LINE_SIZE, destination.y + TILE_PADDING));
        }
        // right
        if( !_isTileSelected(right) || !_isTileSelected(top->getRight()) ) {
            addLine(Vec2(destination.x, destination.y - LINE_SIZE),
                    Vec2(destination.x - LINE_SIZE, destination.y + TILE_PADDING));
        }
    } else {
        addLine(Vec2(origin.x, destination.y), Vec2(destination.x, destination.y - LINE_SIZE));
    }
    
    // bottom
    if( _isTileSelected(bottom) ) {
        // left
        if( !_isTileSelected(left) || !_isTileSelected(bottom->getLeft()) ) {
            addLine(Vec2(origin.x, LINE_SIZE), Vec2(LINE_SIZE, -TILE_PADDING));
        }
        // right
        if( !_isTileSelected(right) || !_isTileSelected(bottom->getRight()) ) {
            addLine(Vec2(destination.x, LINE_SIZE),
                    Vec2(destination.x - LINE_SIZE, -TILE_PADDING));
        }
    } else {
        addLine(origin, Vec2(destination.x, LINE_SIZE));
    }
}

void GameTile::updateSelectedLine() {
    
    // 선택된 경우 테두리 생성
    selectedLine->removeAllChildren();
    
    const float LINE_SIZE = TILE_SELECTED_LINE_SIZE;
    
    Vec2 origin;
    Vec2 destination = getContentSize();
    
    auto addLine = [=](Vec2 origin, Vec2 destination) {
        auto line = DrawNode::create();
        line->setAnchorPoint(Vec2::ZERO);
        line->setPosition(Vec2::ZERO);
        line->setContentSize(getContentSize());
        line->drawSolidRect(origin, destination, Color4F(TILE_SELECTED_LINE_COLOR));
        // line->drawLine(origin, destination, Color4F(TILE_SELECTED_LINE_COLOR));
        selectedLine->addChild(line);
    };
    
    auto isNearTileSelected = [=](GameTile *tile) -> bool {
        return tile && tile->isSelected();
    };
    
    // left
    if( isNearTileSelected(left) ) {
        // bottom
        if( !isNearTileSelected(bottom) || !isNearTileSelected(left->getBottom()) ) {
            addLine(Vec2(origin.x + LINE_SIZE, origin.y), Vec2(-TILE_PADDING, LINE_SIZE));
        }
        // top
        if( !isNearTileSelected(top) || !isNearTileSelected(left->getTop()) ) {
            addLine(Vec2(origin.x + LINE_SIZE, destination.y),
                    Vec2(-TILE_PADDING, destination.y - LINE_SIZE));
        }
    } else {
        addLine(origin, Vec2(LINE_SIZE, destination.y));
    }
    
    // right
    if( isNearTileSelected(right) ) {
        // bottom
        if( !isNearTileSelected(bottom) || !isNearTileSelected(right->getBottom()) ) {
            addLine(Vec2(destination.x, origin.y), Vec2(destination.x + TILE_PADDING, LINE_SIZE));
        }
        // top
        if( !isNearTileSelected(top) || !isNearTileSelected(right->getTop()) ) {
            addLine(destination, Vec2(destination.x + TILE_PADDING, destination.y - LINE_SIZE));
        }
    } else {
        addLine(Vec2(origin.x + destination.x, origin.y),
                Vec2(destination.x - LINE_SIZE, destination.y));
    }
    
    // top
    if( isNearTileSelected(top) ) {
        // left
        if( !isNearTileSelected(left) || !isNearTileSelected(top->getLeft()) ) {
            addLine(Vec2(origin.x, destination.y - LINE_SIZE),
                    Vec2(LINE_SIZE, destination.y + TILE_PADDING));
        }
        // right
        if( !isNearTileSelected(right) || !isNearTileSelected(top->getRight()) ) {
            addLine(Vec2(destination.x, destination.y - LINE_SIZE),
                    Vec2(destination.x - LINE_SIZE, destination.y + TILE_PADDING));
        }
    } else {
        addLine(Vec2(origin.x, destination.y), Vec2(destination.x, destination.y - LINE_SIZE));
    }
    
    // bottom
    if( isNearTileSelected(bottom) ) {
        // left
        if( !isNearTileSelected(left) || !isNearTileSelected(bottom->getLeft()) ) {
            addLine(Vec2(origin.x, LINE_SIZE), Vec2(LINE_SIZE, -TILE_PADDING));
        }
        // right
        if( !isNearTileSelected(right) || !isNearTileSelected(bottom->getRight()) ) {
            addLine(Vec2(destination.x, LINE_SIZE),
                    Vec2(destination.x - LINE_SIZE, -TILE_PADDING));
        }
    } else {
        addLine(origin, Vec2(destination.x, LINE_SIZE));
    }
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
