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
#include "SceneManager.h"

#include "object/HintButton.hpp"
#include "object/StageProgressBar.hpp"

#include "HowToPlayPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

GameView::GameView() :
isTileMapUpdateLocked(false) {
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
    
    // Touch Listener
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameView::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameView::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameView::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameView::onTouchCancelled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    // FIXME: 테스트 버튼, 릴리즈 버전에서 제거 해야함
    /*
    {
        auto btn = SBNodeUtils::createTouchNode();
        btn->setAnchorPoint(ANCHOR_MT);
        btn->setPosition(Vec2TC(0, 0));
        btn->setContentSize(Size(SB_WIN_SIZE.width*0.6f, 100));
        addChild(btn, INT_MAX);
        
        btn->addClickEventListener([=](Ref*) {
            this->updateMadePatterns();
        });
    }
    */
     
    return true;
}

void GameView::onEnter() {
    
    Log::i("GameView::onEnter");
    
    Node::onEnter();
    
    onStageChanged(GAME_MANAGER->getStage());
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    // 튜토리얼
    if( GAME_MANAGER->getStage().stage == 1 ) {
        auto howToPlayPopup = HowToPlayPopup::create();
        howToPlayPopup->setTutorial(true);
        SceneManager::getScene()->addChild(howToPlayPopup, ZOrder::POPUP_MIDDLE);
    }
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

/**
 * 13 클리어
 */
void GameView::onNumberClear(GameTileList selectedTiles) {
    
    SBAudioEngine::playEffect(SOUND_NUMBER_MADE);
    
    auto level = GAME_MANAGER->getStage();
    
    ++clearCount;
    int clearCondition = level.clearCondition;
    
    stageProgressBar->setPercentage(((float)clearCount / clearCondition) * 100, true);
    stageProgressBar->setClearCount(clearCount, clearCondition);
    
    // 레벨 클리어 체크
    bool isLevelClear = (clearCount == clearCondition);
    
    if( isLevelClear ) {
        GameManager::onStageClear();
        return;
    }
    
    // 타일 클리어
    map<int, GameTileList> clearTilesMap;
    
    for( auto tile : selectedTiles ) {
        int x = tile->getTilePosition().x;
        
        if( clearTilesMap.find(x) == clearTilesMap.end() ) {
            clearTilesMap[x] = GameTileList();
        }
        
        ((GameTileList&)clearTilesMap[x]).push_back(tile);
        
        // 타일 제거
        // removeTile(tile);
        tile->clear();
    }
    
    // 타일 이동
    if( !isLevelClear ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
        
        SBDirector::postDelayed(this, [=]() {
            for( auto it = clearTilesMap.begin(); it != clearTilesMap.end(); ++it ) {
                auto x = it->first;
                auto clearTiles = it->second;
                
                CCLOG("x:%d, %d개 제거", x, (int)clearTiles.size());
                
                // 클리어 타일 재활용
                for( int i = 0; i < clearTiles.size(); ++i ) {
                    auto clearTile = clearTiles[i];
                    clearTile->setTilePosition(TilePosition(x, level.tileRows+i));
                    clearTile->setNumber(getRandomNumber());
                    clearTile->setVisible(true);
                }
                
                // 아래로 이동
                auto columnTiles = getColumnTiles(x);
                auto validPosList = getValidColumnTilePositions(x);
                
                CCASSERT(columnTiles.size() == validPosList.size(), "타일 이동 에러");
                
                for( int i = 0; i < columnTiles.size(); ++i ) {
                    auto tile = columnTiles[i];
                    auto tilePos = validPosList[i];
                    
                    auto move = MoveTo::create(TILE_MOVE_DURATION, convertTilePosition(tilePos));
                    auto callFunc = CallFunc::create([=]() {
                        
                        tile->setTilePosition(tilePos);
                        tile->setTileId(level.getTileId(tilePos));
                    });
                    tile->runAction(Sequence::create(move, callFunc, nullptr));
                }
            }
            
            // 이동 완료
            SBDirector::postDelayed(this, [=]() {
                
                SBDirector::getInstance()->setScreenTouchLocked(false);
                
                // 타일 업데이트
                this->updateNearTile();
                this->updateMadePatterns();
                
                // 메이드 불가능하면 새로고침
                if( madePatterns.size() == 0 ) {
                    this->refresh();
                }
                
            }, TILE_MOVE_DURATION+0.1f);
            
        }, TILE_EXIT_DURATION);
    }
}

/**
 * 힌트 보기
 */
void GameView::onHint() {
    
    // CCASSERT(madePatterns() > 0, "GameView::onHint error: 메이드 패턴 없음");
    
    Log::i("GameView onHint patterns: %d", (int)madePatterns.size());
    
    if( madePatterns.size() == 0 ) {
        SBAnalytics::logError("use hint error: no maid pattern");
        // MessageBox("메이드 패턴 없음!", "힌트 사용 에러");
        
        refresh();
        
        return;
    }

    hintButton->setTouchEnabled(false);
    
    // 셔플
    auto patterns = madePatterns;
    random_shuffle(patterns.begin(), patterns.end());
    
    // 패턴 크기 오름차순
    sort(patterns.begin(), patterns.end(), [](const MadePattern &p1, const MadePattern &p2) -> bool {
        return p1.size() < p2.size();
    });
    
    // 힌트 표시
    auto pattern = patterns[0];
    
    for( auto tile : pattern ) {
        tile->setSelected(true);
        tile->updateSelectedLine([=](GameTile *tile) -> bool {
            return SBCollection::contains(pattern, tile);
        }, TILE_HINT_LINE_COLOR, true);
    }
    
    // 통계 이벤트
    SBAnalytics::EventParams params;
    params[ANALYTICS_EVENT_PARAM_LEVEL] = SBAnalytics::EventParam(TO_STRING(GAME_MANAGER->getStage().stage));
    
    SBAnalytics::logEvent(ANALYTICS_EVENT_HINT_USE, params);
}

/**
 * 새로 고침
 */
void GameView::refresh() {
    
    const auto level = GAME_MANAGER->getStage();
    
    auto popup = SBNodeUtils::createTouchNode();
    addChild(popup, ZOrder::POPUP_BOTTOM);
    
    auto bg = LayerColor::create(Color::POPUP_BG);
    popup->addChild(bg);
    
    auto icon = Sprite::create(DIR_IMG_GAME + "game_ui_refresh.png");
    icon->setAnchorPoint(ANCHOR_M);
    icon->setPosition(Vec2MC(0, 0));
    popup->addChild(icon);
    
    // 팝업 연출
    bg->setOpacity(0);
    bg->runAction(FadeTo::create(0.15f, Color::POPUP_BG.a));
    
    icon->setPositionY(icon->getPositionY() + SB_WIN_SIZE.height);
    icon->runAction(MoveTo::create(EffectDuration::POPUP_SLIDE_FAST, Vec2MC(0,0)));
    
    // 맵 슬라이드 연출
    SBDirector::postDelayed(this, [=]() {

        SBAudioEngine::playEffect(SOUND_TILE_REFRESH);
        
        // 현재 맵 캡쳐
        double t = SBSystemUtils::getCurrentTimeSeconds();

        auto image = utils::captureNode(tileMap);
        auto texture = new Texture2D();
        texture->initWithImage(image);

        auto currentMap = Sprite::createWithTexture(texture);
        currentMap->setAnchorPoint(tileMap->getAnchorPoint());
        currentMap->setPosition(tileMap->getPosition());
        tileMap->getParent()->addChild(currentMap, tileMap->getLocalZOrder());

        CC_SAFE_RELEASE(texture);
        CC_SAFE_RELEASE(image);

        CCLOG("capture dt: %f", SBSystemUtils::getCurrentTimeSeconds() - t);

        // 맵 업데이트
        this->updateTileMap(level);

        // 슬라이드 연출
        const float DURATION = LEVEL_REFRESH_DURATION * 0.7f;

        auto move = MoveBy::create(DURATION, Vec2(0, -tileMap->getContentSize().height));
        currentMap->runAction(Sequence::create(move, RemoveSelf::create(), nullptr));

        tileMap->setPositionY(tileMap->getPositionY() + tileMap->getContentSize().height + TILE_PADDING);
        tileMap->runAction(MoveTo::create(DURATION, Vec2::ZERO));

    }, EffectDuration::POPUP_SLIDE_FAST);
    
    SBDirector::postDelayed(this, [=]() {
        popup->removeFromParent();
    }, LEVEL_REFRESH_DURATION);
    
    // 통계 이벤트
    SBAnalytics::EventParams params;
    params[ANALYTICS_EVENT_PARAM_LEVEL] = SBAnalytics::EventParam(TO_STRING(level.stage));
    
    SBAnalytics::logEvent(ANALYTICS_EVENT_LEVEL_REFRESH, params);
}

/**
 * 타일 선택
 */
void GameView::selectTile(GameTile *tile) {
    
    CCASSERT(!tile->isSelected(), "GameView::selectTile error: already selected.");
    
    if( !isSelectableTile(tile) ) {
        return;
    }
    
    SBAudioEngine::playEffect(SOUND_TILE_SELECT);
    
    tile->setSelected(true);
    selectedTiles.push_back(tile);
    
    for( auto tile : selectedTiles ) {
        tile->updateSelectedLine();
    }
}

/**
 * 타일맵 업데이트
 */
void GameView::updateTileMap(const StageData &level) {
    
    if( isTileMapUpdateLocked ) {
        return;
    }
    
    // 이전 타일 제거
    tileMap->removeAllChildren();
    tiles.clear();
    
    // 초기화
    numbers = level.numbers;
    resetNumberEngine();
    
    // UI 업데이트
    auto tileMapSize = getTileContentSize(level.tileRows, level.tileColumns);
    tileMapClippingNode->setContentSize(tileMapSize);
    tileMapClippingNode->getStencil()->setContentSize(tileMapSize);
    tileMap->setContentSize(tileMapSize);
    
    for( auto tileData : level.tiles ) {
        if( !tileData.isEmpty ) {
            addTile(tileData);
        }
    }
    
    //
    {
        map<int,int> currentNumberCounts;
        
        for( auto tile : tiles ) {
            int n = tile->getNumber();
            if( n == INVALID_TILE_NUMBER ) {
                continue;
            }
            
            if( currentNumberCounts.find(n) == currentNumberCounts.end() ) {
                currentNumberCounts[n] = 1;
            } else {
                currentNumberCounts[n] = currentNumberCounts[n]+1;
            }
        }
        
        for( auto it = currentNumberCounts.begin(); it != currentNumberCounts.end(); ++it ) {
            float per = ((float)it->second / tiles.size()) * 100.0f;
            Log::i("getRandomNumber current number %d: %.2f%%", it->first, per);
        }
    }
    //
    
    updateNearTile();
    updateMadePatterns();
    
    // 메이드 패턴 없으면 재귀
    Log::i("GameView updateTileMap patterns: %d", (int)madePatterns.size());
    
    if( madePatterns.size() == 0 ) {
        SBAnalytics::logDebug("recursive update tile map");
        updateTileMap(level);
    }
}

/**
 * 인접한 타일을 업데이트합니다
 */
void GameView::updateNearTile() {
    
    for( auto tile : tiles ) {
        auto p = tile->getTilePosition();
        tile->setNearTile(getTile(p + TilePosition(-1,0)),
                          getTile(p + TilePosition(1,0)),
                          getTile(p + TilePosition(0,1)),
                          getTile(p + TilePosition(0,-1)));
    }
}

/**
 * 타일을 추가합니다
 */
void GameView::addTile(const TileData &tileData) {

    auto tile = GameTile::create();
    tile->setAnchorPoint(ANCHOR_M);
    tile->setTilePosition(tileData.p);
    tile->setTileId(GAME_MANAGER->getStage().getTileId(tileData.p));
    tile->setNumber(getRandomNumber());
    tileMap->addChild(tile);
    
    tiles.push_back(tile);
}

/**
 * 타일을 제거합니다
 */
void GameView::removeTile(GameTile *tile) {
    
    tile->remove();
    SBCollection::remove(tiles, tile);
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

#define PRINT_SIMULATION_NUMBER     0
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

#pragma mark- Tile

#define MADE_PATTERN_SEARCH_COUNT                   1

void GameView::recursiveMadePatternSearch(GameTile *anchorTile, MadePattern &pattern, int &sum) {
    
    if( madePatterns.size() == MADE_PATTERN_SEARCH_COUNT ) {
        return;
    }
    
    if( !anchorTile || sum == 13 ) {
        return;
    }
    
    // 신규 타일만 계산
    if( !SBCollection::contains(pattern, anchorTile) ) {
        if( sum + anchorTile->getNumber() <= 13 ) {
            sum += anchorTile->getNumber();
            pattern.push_back(anchorTile);
        }
        // 13 초과 하는 경우 정지
        else {
            return;
        }
    }
    
    auto searchNearTile = [this](GameTile *nearTile, MadePattern &pattern, int &sum) {
      
        if( !SBCollection::contains(pattern, nearTile) ) {
            this->recursiveMadePatternSearch(nearTile, pattern, sum);
        }
    };

    searchNearTile(anchorTile->getLeft(), pattern, sum);
    searchNearTile(anchorTile->getRight(), pattern, sum);
    searchNearTile(anchorTile->getTop(), pattern, sum);
    searchNearTile(anchorTile->getBottom(), pattern, sum);
//    recursiveMadePattern(anchorTile->getLeft(), pattern, sum);
//    recursiveMadePattern(anchorTile->getRight(), pattern, sum);
//    recursiveMadePattern(anchorTile->getTop(), pattern, sum);
//    recursiveMadePattern(anchorTile->getBottom(), pattern, sum);
}

void GameView::updateMadePatterns() {
    
    CCLOG("========== UPDATE MADE PATTERNS START ==========");
    double t = SBSystemUtils::getCurrentTimeSeconds();

    madePatterns.clear();
    
    auto contains = [=](const MadePattern &pattern) -> bool {

        for( auto p : madePatterns ) {
            auto compare = [](GameTile *t1, GameTile *t2) -> bool {
                return t1->getTileId() == t2->getTileId();
            };

            if( SBCollection::isEqual(p, pattern, compare) ){
                return true;
            }
        }

        return false;
    };

    for( auto tile : tiles ) {
        auto checkPattern = [=](GameTile *nearTile) {
            
            if( madePatterns.size() == MADE_PATTERN_SEARCH_COUNT ) {
                return;
            }
            
            if( !nearTile ) {
                return;
            }
            
            MadePattern pattern;
            pattern.push_back(tile);
            pattern.push_back(nearTile);
            
            int number = getTileNumberSum(pattern);
            this->recursiveMadePatternSearch(nearTile, pattern, number);

            if( number == 13 ) {
                // 타일 아이디 오름차순 정렬
                sort(pattern.begin(), pattern.end(), [](GameTile *t1, GameTile *t2) -> bool {
                    return t1->getTileId() < t2->getTileId();
                });

                string str = "";
                for( auto tile : pattern ) {
                    str += STR_FORMAT("%d,", tile->getTileId());
                }

                if( !contains(pattern) ) {
                    CCLOG("%s", str.c_str());
                    madePatterns.push_back(pattern);
                }
            }
        };
        
        checkPattern(tile->getLeft());
        checkPattern(tile->getRight());
        checkPattern(tile->getTop());
        checkPattern(tile->getBottom());
        
        if( madePatterns.size() == MADE_PATTERN_SEARCH_COUNT ) {
            break;
        }
    }
    
    CCLOG("patterns: %d", (int)madePatterns.size());
    CCLOG("dt: %f", SBSystemUtils::getCurrentTimeSeconds() - t);
    CCLOG("========== UPDATE MADE PATTERNS END ==========");
}

GameTile* GameView::getTile(const TilePosition &p) {
    
    for( auto tile : tiles ) {
        if( tile->getTilePosition() == p ) {
            return tile;
        }
    }
    
    return nullptr;
}

GameTileList GameView::getColumnTiles(int x) {
    
    GameTileList columnTiles;
    
    for( auto tile : tiles ) {
        if( (int)tile->getTilePosition().x == x ) {
            columnTiles.push_back(tile);
        }
    }
    
    // y좌표 오름차순 정렬
    sort(columnTiles.begin(), columnTiles.end(), [=](GameTile *t1, GameTile *t2) -> bool {
        return t1->getTilePosition().y < t2->getTilePosition().y;
    });
    
    return columnTiles;
}

TilePositionList GameView::getValidColumnTilePositions(int x) {
    
    auto level = GAME_MANAGER->getStage();
    TilePositionList posList;
    
    for( int y = 0; y < level.tileRows; ++y ) {
        TilePosition p(x,y);
        
        if( !level.isTileEmpty(p) ) {
            posList.push_back(p);
        }
    }
    
    return posList;
}

int GameView::getTileNumberSum(GameTileList tiles) {
    
    int sum = 0;
    
    for( auto tile : tiles ) {
        sum += tile->getNumber();
    }
    
    return sum;
}

#pragma mark- Game Event

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
    isTileMapUpdateLocked = false;
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
    
//    GameTileList currentGameTiles;
//
//    // 다음 레벨로 변경
//    auto nextLevel = GAME_MANAGER->getStage();
//
//    updateTileMap(nextLevel);
//    isTileMapUpdateLocked = true;
    
    SBAudioEngine::playEffect(SOUND_TILE_REFRESH);
    
    // 현재 맵 캡쳐
    double t = SBSystemUtils::getCurrentTimeSeconds();

    auto image = utils::captureNode(tileMap);
    auto texture = new Texture2D();
    texture->initWithImage(image);

    auto currentMap = Sprite::createWithTexture(texture);
    currentMap->setAnchorPoint(ANCHOR_M);
    currentMap->setPosition(Vec2MC(0,0));
    SceneManager::getScene()->addChild(currentMap);

    CC_SAFE_RELEASE(texture);
    CC_SAFE_RELEASE(image);

    CCLOG("capture dt: %f", SBSystemUtils::getCurrentTimeSeconds() - t);

    // 다음 레벨로 변경
    auto nextLevel = GAME_MANAGER->getStage();

    updateTileMap(nextLevel);
    isTileMapUpdateLocked = true;

    // 맵 슬라이드 연출
    auto move = MoveBy::create(MOVE_NEXT_LEVEL_DURATION, Vec2(-SB_WIN_SIZE.width, 0));
    currentMap->runAction(Sequence::create(move, RemoveSelf::create(), nullptr));

    tileMap->setPositionX(tileMap->getPositionX() + SB_WIN_SIZE.width);
    tileMap->runAction(move->clone());
    
    // 연출 완료
    SBDirector::postDelayed(this, [=]() {
        GameManager::onMoveNextStageFinished();
    }, MOVE_NEXT_LEVEL_DURATION, true);
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameView::onMoveNextStageFinished() {
}

#pragma mark- Touch Event

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
    
    // 힌트 UI 되돌리기
    hintButton->setTouchEnabled(true);
    
    for( auto tile : tiles ) {
        tile->setSelected(false);
    }
    
    // 타일 선택
    for( auto tile : tiles ) {
        if( isContainsPoint(tile, p) ) {
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
        if( tile->isSelected() ) {
            continue;
        }
        
        if( isContainsPoint(tile, p) ) {
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

bool GameView::isContainsPoint(GameTile *tile, const Vec2 &p) {
    
    auto tileBox = SB_BOUNDING_BOX_IN_WORLD(tile);
    
    if( !tileBox.containsPoint(p) ) {
        return false;
    }
    
    auto tileCenter = Vec2(tileBox.getMidX(), tileBox.getMidY());
    auto dist = tileCenter.getDistance(p);
    
    return dist <= TILE_CONTENT_SIZE.height*0.5f;
}

/**
 * 배경 초기화
 */
void GameView::initBg() {

    // 힌트
    hintButton = HintButton::create();
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

    auto stencil = LayerColor::create(Color4B::WHITE);
    stencil->setIgnoreAnchorPointForPosition(false);
    stencil->setAnchorPoint(Vec2::ZERO);
    stencil->setPosition(Vec2::ZERO);
    stencil->setContentSize(TILE_MAP_CONTENT_SIZE);
    
    tileMapClippingNode = ClippingNode::create(stencil);
    tileMapClippingNode->setAnchorPoint(ANCHOR_M);
    tileMapClippingNode->setPosition(Vec2MC(0,0));
    tileMapClippingNode->setContentSize(TILE_MAP_CONTENT_SIZE);
    addChild(tileMapClippingNode);
    
    tileMap = Node::create();
    tileMap->setAnchorPoint(Vec2::ZERO);
    tileMap->setPosition(Vec2::ZERO);
    tileMap->setContentSize(TILE_MAP_CONTENT_SIZE);
    tileMapClippingNode->addChild(tileMap);
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
        GAME_EVENT_MOVE_NEXT_STAGE,
        GAME_EVENT_MOVE_NEXT_STAGE_FINISHED,
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
                
            case GameEvent::MOVE_NEXT_STAGE:            this->onMoveNextStage();          break;
            case GameEvent::MOVE_NEXT_STAGE_FINISHED:   this->onMoveNextStageFinished();  break;
                
            default: break;
        }
    }, this);
}
