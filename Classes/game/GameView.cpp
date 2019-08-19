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

#include "object/StageProgressBar.hpp"

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
    
    initBg();
    initTileMap();
    initGameListener();
    
    // 멀티 터치 방지
    addChild(SBNodeUtils::createSwallowMultiTouchNode(), SBZOrder::BOTTOM+1);
    
    // Touch Listener
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameView::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameView::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameView::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameView::onTouchCancelled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    // FIXME: 다시 하기 버튼
    {
        auto btn = SBNodeUtils::createTouchNode();
        btn->setAnchorPoint(ANCHOR_MT);
        btn->setPosition(Vec2TC(0, 0));
        btn->setContentSize(Size(SB_WIN_SIZE.width*0.6f, 100));
        addChild(btn, INT_MAX);
        
        btn->addClickEventListener([=](Ref*) {
            this->updateTileMap(GAME_MANAGER->getStage());
        });
    }
    
    return true;
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 타일 선택
 */
void GameView::selectTile(GameTile *tile) {
    
    CCASSERT(!tile->isSelected(), "GameView::selectTile error: already selected.");
    
    if( isSelectableTile(tile) ) {
        tile->setSelected(true);
        selectedTiles.push_back(tile);
    }
}

/**
 * 선택 가능한 타일인지 반환합니다
 */
bool GameView::isSelectableTile(GameTile *tile) {
    
    if( selectedTiles.size() == 0 ) {
        return true;
    }
    
    // 이미 선택된 타일과 인접한지 체크
    bool isAdjacent = false;
    
    for( auto selectedTile : selectedTiles ) {
        auto diff = selectedTile->getTilePosition() - tile->getTilePosition();
        diff.x = fabsf(diff.x);
        diff.y = fabsf(diff.y);
        // CCLOG("diff: %d,%d", (int)diff.x, (int)diff.y);
        
        if( diff.x == 0 && diff.y == 1 ) {
            isAdjacent = true;
            break;
        }

        if( diff.y == 0 && diff.x == 1 ) {
            isAdjacent = true;
            break;
        }
        
        // float dist = selectedTile->getTilePosition().getDistance(tile->getTilePosition());
        // CCLOG("dist: %f", dist);
    }
    
    return isAdjacent;
}

/**
 * 13 클리어
 */
void GameView::onNumberClear(GameTileList selectedTiles) {
    
    ++clearCount;
    int clearCondition = GAME_MANAGER->getStage().clearCondition;
    
    // 스테이지 클리어 체크
    bool isStageClear = (clearCount == clearCondition);
    
    if( isStageClear ) {
        GameManager::onStageClear();
    }
    
    // UI
    stageProgressBar->setPercentage(((float)clearCount / clearCondition) * 100);
    
    for( auto tile : selectedTiles ) {
        tile->clear(true);
    }
    
    // 스테이지 클리어 못한 경우 다음 번호 생성
    if( !isStageClear ) {
        SBDirector::postDelayed(this, [=]() {
            for( auto tile : selectedTiles ) {
                tile->setNumber(getRandomNumber(), true);
            }
        }, TILE_NUMBER_EXIT_DURATION);
    }
}

/**
 * 게임 리셋
 */
void GameView::onGameReset() {
    
    isTouchLocked = false;
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    // SBNodeUtils::recursivePause(this);
}

/**
 * 게임 재개
 */
void GameView::onGameResume() {
    
    // SBNodeUtils::recursiveResume(this);
}

/**
 * 스테이지 변경
 */
void GameView::onStageChanged(const StageData &stage) {
    
    isTouchLocked = false;
    clearCount = 0;
    
    updateTileMap(stage);
}

/**
 * 스테이지 재시작
 */
void GameView::onStageRestart(const StageData &stage) {
    
    isTouchLocked = false;
}

/**
 * 스테이지 클리어
 */
void GameView::onStageClear(const StageData &stage) {
    
    isTouchLocked = true;
}

/**
 * 다음 스테이지로 이동
 */
void GameView::onMoveNextStage() {
    
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameView::onMoveNextStageFinished() {
    
    stageProgressBar->setVisible(true);
}

/**
 * 터치 시작
 */
bool GameView::onTouchBegan(Touch *touch, Event*) {
    
    if( isTouchLocked ) {
        return false;
    }
    
    Vec2 p = touch->getLocation();
    
    if( !SB_BOUNDING_BOX_IN_WORLD(tileMap).containsPoint(p) ) {
        return false;
    }
    
    for( auto tile : tiles ) {
        auto tileBox = SB_BOUNDING_BOX_IN_WORLD(tile);
        
        if( tileBox.containsPoint(p) && !tile->isEmpty() ) {
            selectTile(tile);
        }
    }
    
    return true;
}

/**
 * 터치 이동
 */
void GameView::onTouchMoved(Touch *touch, Event*) {

    if( isTouchLocked ) {
        return;
    }
    
    Vec2 p = touch->getLocation();
    
    for( auto tile : tiles ) {
        if( tile->isEmpty() || tile->isSelected() ) {
            continue;
        }
        
        auto tileBox = SB_BOUNDING_BOX_IN_WORLD(tile);
        
        if( tileBox.containsPoint(p) ) {
            selectTile(tile);
        }
    }
}

/**
 * 터치 종료
 */
void GameView::onTouchEnded(Touch *touch, Event*) {

    // 선택된 타일의 합을 구한다
    int number = 0;
    
    for( auto tile : selectedTiles ) {
        tile->setSelected(false);
        number += tile->getNumber();
    }
    
    // 타일의 합이 13인지 체크
    if( number == 13 ) {
        onNumberClear(selectedTiles);
    }
    
    selectedTiles.clear();
}

void GameView::onTouchCancelled(Touch *touch, Event *e) {

    for( auto tile : selectedTiles ) {
        tile->setSelected(false);
    }
    
    selectedTiles.clear();
}

/**
 * 타일맵 업데이트
 */
void GameView::updateTileMap(const StageData &stage) {
 
    // 이전 타일 제거
    tileMap->removeAllChildren();
    tiles.clear();
    
    // 초기화
    numbers = stage.numbers;
    
    random_device rd;
    numberEngine = mt19937(rd());
    
    tileMap->setContentSize(getTileContentSize(stage.tileRows, stage.tileColumns));
    
    for( auto tileData : stage.tiles ) {
        auto tile = GameTile::create(tileData);
        tile->setAnchorPoint(ANCHOR_M);
        tile->setPosition(getTilePosition((int)tileData.p.x, (int)tileData.p.y));
        tileMap->addChild(tile);
        
        tiles.push_back(tile);
        
        if( !tile->isEmpty() ) {
            tile->setNumber(getRandomNumber(), true);
        }
    }
}

int GameView::getRandomNumber() {
    
    std::shuffle(numbers.begin(), numbers.end(), numberEngine);
    return numbers[0];
}

/**
 * 배경 초기화
 */
void GameView::initBg() {

    // 스테이지 진행도
    stageProgressBar = StageProgressBar::create();
    addChild(stageProgressBar);
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

/**
 * 게임 이벤트 리스너 초기화
 */
void GameView::initGameListener() {
    
    StringList events({
        GAME_EVENT_PAUSE,
        GAME_EVENT_RESUME,
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_RESTART,
        GAME_EVENT_STAGE_CLEAR,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::RESET:     this->onGameReset();         break;
            case GameEvent::PAUSE:     this->onGamePause();         break;
            case GameEvent::RESUME:    this->onGameResume();        break;
                
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
