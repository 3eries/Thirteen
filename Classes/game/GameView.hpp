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

class StageProgressBar;

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    GameView();
    
    bool init() override;
    void cleanup() override;
    
    void initBg();
    void initTileMap();
    void initGameListener();
    
    void selectTile(GameTile *tile);
    void onNumberClear(GameTileList selectedTiles);
    
    void updateTileMap(const StageData &stage);
    
    bool isSelectableTile(GameTile *tile);
    int getRandomNumber();
    
// Game Event
public:
    void onGameReset();
    void onGamePause();
    void onGameResume();
    
    void onStageChanged(const StageData &stage);
    void onStageRestart(const StageData &stage);
    void onStageClear(const StageData &stage);
    
    void onMoveNextStage();
    void onMoveNextStageFinished();
    
// Touch Event
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event*);
    
    // int getMinDist();
    
private:
    bool isTouchLocked;
    
    cocos2d::Node *tileMap;
    GameTileList tiles;
    GameTileList selectedTiles;
    
    StageProgressBar *stageProgressBar;
    
    IntList numbers;
    std::mt19937 numberEngine;
    
    int clearCount; // 클리어 횟수
};

#endif /* GameView_hpp */
