//
//  GameView.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../content/data/model/StageData.h"
#include "object/GameTile.hpp"

class HintButton;
class StageProgressBar;

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void cleanup() override;
    
    void initBg();
    void initTileMap();
    void initGameListener();
    
    void onNumberClear(GameTileList selectedTiles);
    void onHint();
    void refresh();
    
    void selectTile(GameTile *tile);
    
    void updateTileMap(const StageData &level);
    void updateNearTile();
    
    void addTile(const TileData &tileData);
    void removeTile(GameTile *tile);
    
    bool isSelectableTile(GameTile *tile);
    
    void resetNumberEngine();
    int getRandomNumber();
    
private:
    HintButton *hintButton;
    StageProgressBar *stageProgressBar;
    
    IntList numbers;
    std::mt19937 numberEngine;
    
    int clearCount; // 클리어 횟수

#pragma mark- Tile
private:
    typedef GameTileList             MadePattern;
    typedef std::vector<MadePattern> MadePatternList;
    
    void recursiveMadePattern(GameTile *anchorTile, MadePattern &pattern, int &sum);
    void updateMadePatterns();
    
    GameTile*        getTile(const TilePosition &p);
    GameTileList     getColumnTiles(int x);
    TilePositionList getValidColumnTilePositions(int x);
    
    static int getTileNumberSum(GameTileList tiles);
    
private:
    bool isTileMapUpdateLocked;
    
    cocos2d::ClippingNode* tileMapClippingNode;
    cocos2d::Node*         tileMap;
    
    GameTileList tiles;
    GameTileList selectedTiles;
    
    MadePatternList madePatterns;
    
#pragma mark- Game Event
public:
    void onGameReset();
    void onGamePause();
    void onGameResume();
    
    void onStageChanged(const StageData &stage);
    void onStageRestart(const StageData &stage);
    void onStageClear(const StageData &stage);
    
    void onMoveNextStage();
    void onMoveNextStageFinished();
    
#pragma mark- Touch Event
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
    bool isContainsPoint(GameTile *tile, const cocos2d::Vec2 &p);
    
private:
    bool isTouchLocked;
};

#endif /* GameView_hpp */
