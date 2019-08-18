//
//  GameView.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameView.hpp"

#include "Define.h"
#include "GameDefine.h"
#include "ContentManager.hpp"

#include "GameTile.hpp"

USING_NS_CC;
using namespace std;

GameView::GameView() {
}

GameView::~GameView() {
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    initTileMap();
    
    // Touch Listener
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameView::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameView::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameView::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameView::onTouchCancelled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    // FIXME:
    updateTileMap(Database::getStages()[1]);
    
    return true;
}

/**
 * 터치 시작
 */
bool GameView::onTouchBegan(Touch *touch, Event*) {
    
    Vec2 p = touch->getLocation();
    
    for( auto tile : tiles ) {
        auto tileBox = SB_BOUNDING_BOX_IN_WORLD(tile);
        
        if( tileBox.containsPoint(p) && !tile->isEmpty() ) {
            tile->setSelected(true);
            return true;
        }
    }
    
    return false;
}

/**
 * 터치 이동
 */
void GameView::onTouchMoved(Touch *touch, Event*) {

    Vec2 p = touch->getLocation();
    
    for( auto tile : tiles ) {
        if( tile->isEmpty() ) {
            continue;
        }
        
        auto tileBox = SB_BOUNDING_BOX_IN_WORLD(tile);
        
        if( tileBox.containsPoint(p) && !tile->isSelected() ) {
            tile->setSelected(true);
        }
    }
}

/**
 * 터치 종료
 */
void GameView::onTouchEnded(Touch *touch, Event*) {

    // 선택된 타일의 합을 구한다
    int number = 0;
    
    for( auto tile : tiles ) {
        if( tile->isSelected() ) {
            tile->setSelected(false);
            
            number += tile->getNumber();
        }
    }
    
    // 타일의 합이 13인지 체크
    if( number == 13 ) {
        CCLOG("13야쓰!!");
    }
}

void GameView::onTouchCancelled(Touch *touch, Event *e) {

    for( auto tile : tiles ) {
        tile->setSelected(false);
    }
}

/**
 * 타일맵 업데이트
 */
void GameView::updateTileMap(const StageData &stageData) {
 
    // 이전 타일 제거
    tileMap->removeAllChildren();
    tiles.clear();
    
    // 초기화
    random_device rd;
    numberEngine = mt19937(rd());
    
    tileMap->setContentSize(getTileContentSize(stageData.tileRows, stageData.tileColumns));
    
    auto numbers = stageData.numbers;
    
    for( auto tileData : stageData.tiles ) {
        auto tile = GameTile::create(tileData);
        tile->setAnchorPoint(ANCHOR_M);
        tile->setPosition(getTilePosition((int)tileData.p.x, (int)tileData.p.y));
        tileMap->addChild(tile);
        
        tiles.push_back(tile);
        
        if( !tile->isEmpty() ) {
            std::shuffle(numbers.begin(), numbers.end(), numberEngine);
            tile->setNumber(numbers[0]);
        }
    }
}

/**
 * 타일맵 초기화
 */
void GameView::initTileMap() {

    tileMap = Node::create();
    tileMap->setAnchorPoint(ANCHOR_M);
    tileMap->setPosition(Vec2MC(0,0));
    tileMap->setContentSize(TILE_MAP_CONTENT_SIZE);
    addChild(tileMap);
}
