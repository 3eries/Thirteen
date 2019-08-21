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

#include "object/HintButton.hpp"
#include "object/StageProgressBar.hpp"

USING_NS_CC;
USING_NS_SB;
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

void GameView::onEnter() {
    
    Log::i("GameView::onEnter");
    
    Node::onEnter();
    
    onStageChanged(GAME_MANAGER->getStage());
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
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
    stageProgressBar->setPercentage(((float)clearCount / clearCondition) * 100, true);
    
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
 * 힌트 보기
 */
void GameView::onHint() {
    
    auto patterns = getMadePatterns();
    Log::i("GameView onHint patterns: %d", (int)patterns.size());
    
    if( patterns.size() > 0 ) {
        auto pattern = patterns[0];
        
        for( auto tile : pattern ) {
            tile->yap();
        }
    }
}

/**
 * 타일 선택
 */
void GameView::selectTile(GameTile *tile) {
    
    CCASSERT(!tile->isSelected(), "GameView::selectTile error: already selected.");
    
    if( !isSelectableTile(tile) ) {
        return;
    }
    
    tile->setSelected(true);
    selectedTiles.push_back(tile);
    
    // 마스킹 영역 업데이트
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
    resetNumberEngine();
    
    // UI 업데이트
    tileMap->setContentSize(getTileContentSize(stage.tileRows, stage.tileColumns));
    
    for( auto tileData : stage.tiles ) {
        if( tileData.isEmpty ) {
            continue;
        }
        
        auto tile = GameTile::create(tileData);
        tile->setAnchorPoint(ANCHOR_M);
        tile->setPosition(getTilePosition((int)tileData.p.x, (int)tileData.p.y));
        tile->setNumber(getRandomNumber(), false);
        tileMap->addChild(tile);
        
        tiles.push_back(tile);
    }
    
    // 인접 타일 설정
    for( auto tile : tiles ) {
        auto p = tile->getTilePosition();
        tile->setNearTile(getTile(p + TilePosition(-1,0)),
                          getTile(p + TilePosition(1,0)),
                          getTile(p + TilePosition(0,1)),
                          getTile(p + TilePosition(0,-1)));
    }
    
    // 패턴 검증
    auto patterns = getMadePatterns();
    Log::i("GameView updateTileMap patterns: %d", (int)patterns.size());
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

#define PRINT_SIMULATION_NUMBER     1
#define NUMBER_SIMULATION_COUNT     10000

void GameView::resetNumberEngine() {
    
    auto level = GAME_MANAGER->getStage();
    
    random_device rd;
    numberEngine = mt19937(rd());
    uniform_int_distribution<int> dist(1, level.numberWeightSum);
    
#if PRINT_SIMULATION_NUMBER
    map<int,int> numberMap;
    double t = SBSystemUtils::getCurrentTimeSeconds();
#endif
    
    for( int i = 0; i < NUMBER_SIMULATION_COUNT; ++i ) {
        int n = dist(numberEngine);
        
#if PRINT_SIMULATION_NUMBER
        if( numberMap.find(n) == numberMap.end() ) {
            numberMap[n] = 1;
        } else {
            numberMap[n] = numberMap[n]+1;
        }
#endif
    }
    
    // print numbers
#if PRINT_SIMULATION_NUMBER
    for( auto it = numberMap.begin(); it != numberMap.end(); ++it ) {
        float per = ((float)it->second / NUMBER_SIMULATION_COUNT) * 100.0f;
        Log::i("simulation result number %d: %.2f%%", it->first, per);
    }
    Log::i("simulation time: %f", SBSystemUtils::getCurrentTimeSeconds() - t);
#endif
    
    int weightRangeBegin = 1;
    int weightRangeEnd = 0;
    
    for( int i = 0; i < level.numbers.size(); ++i ) {
        int weight = level.numberWeights[i];
        weightRangeEnd += weight;
        CCLOG("가중치 체크 num:%d weight range: %d~%d", level.numbers[i], weightRangeBegin, weightRangeEnd);
        
        weightRangeBegin = weightRangeEnd+1;
    }
}

int GameView::getRandomNumber() {
    
    auto level = GAME_MANAGER->getStage();
    
    uniform_int_distribution<int> dist(1, level.numberWeightSum);
    int r = dist(numberEngine);
    
    int weightRangeBegin = 1;
    int weightRangeEnd = 0;
    
    for( int i = 0; i < level.numbers.size(); ++i ) {
        int weight = level.numberWeights[i];
        weightRangeEnd += weight;
        
        if( r >= weightRangeBegin && r <= weightRangeEnd ) {
            return level.numbers[i];
        }
        
        weightRangeBegin = weightRangeEnd+1;
    }
    
    CCASSERT(false, "GameView::getRandomNumber error.");
    return 0;
    //    std::shuffle(numbers.begin(), numbers.end(), numberEngine);
    //    return numbers[0];
}

void GameView::recursiveMadePattern(GameTile *anchorTile, MadePattern &pattern, int &sum) {
    
    if( !anchorTile || sum >= 13 ) {
        return;
    }
    
    // 이미 등록된 타일
    for( auto patternTile : pattern ) {
        if( anchorTile == patternTile ) {
            return;
        }
    }
    
    sum += anchorTile->getNumber();
    pattern.push_back(anchorTile);
    
    recursiveMadePattern(anchorTile->getLeft(), pattern, sum);
    recursiveMadePattern(anchorTile->getRight(), pattern, sum);
    recursiveMadePattern(anchorTile->getTop(), pattern, sum);
    recursiveMadePattern(anchorTile->getBottom(), pattern, sum);
}

vector<GameView::MadePattern> GameView::getMadePatterns() {
    
    vector<MadePattern> patterns;
    
    for( auto tile : tiles ) {
        int number = 0;
        MadePattern pattern;
        
        recursiveMadePattern(tile, pattern, number);
        
        if( number == 13 ) {
            patterns.push_back(pattern);
        }
    }
    
    // TODO:
    // 중복 패턴 제거
    
    /*
    auto getAdjacentTiles = [=](GameTile *anchorTile) -> GameTileList {
        
        GameTileList adjacentTiles;
        auto anchorTilePos = anchorTile->getTilePosition();
        
        for( auto tile : tiles ) {
            if( anchorTile == tile ) {
                continue;
            }
            
            auto diff = anchorTilePos - tile->getTilePosition();
            diff.x = fabsf(diff.x);
            diff.y = fabsf(diff.y);
            
            if( (diff.x == 0 && diff.y == 1) || (diff.y == 0 && diff.x == 1) ) {
                adjacentTiles.push_back(tile);
            }
        }
        
        return adjacentTiles;
    };
    
    for( auto firstTile : tiles ) {
        int num = firstTile->getNumber();
        auto adjacentTiles = getAdjacentTiles(firstTile);
        
        for( auto adjacentTile : adjacentTiles ) {
            num += adjacentTile->getNumber();
            
            if( num == 13 ) {
                
            }
        }
    }
    */
     
    return patterns;
}

GameTile* GameView::getTile(const TilePosition &p) {
    
    for( auto tile : tiles ) {
        if( tile->getTilePosition() == p ) {
            return tile;
        }
    }
    
    return nullptr;
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
 * 배경 초기화
 */
void GameView::initBg() {

    // 힌트
    auto hintButton = HintButton::create();
    addChild(hintButton);
    
    hintButton->setOnHintListener(CC_CALLBACK_0(GameView::onHint, this));
    
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
