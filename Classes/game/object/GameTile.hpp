//
//  GameTile.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameTile_hpp
#define GameTile_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../../content/data/model/StageData.h"

class GameTile : public cocos2d::Node {
public:
    static GameTile* create(const TileData &data);
    ~GameTile();
    
private:
    GameTile();
    
    bool init(const TileData &data);
    
public:
    void clear(bool withAction);
    
    void setNearTile(GameTile *left, GameTile *right, GameTile *top, GameTile *bottom);
    void setNumber(int number, bool withAction);
    void setSelected(bool isSelected);
    
    void yap();
    
    void runNumberEnterAction();
    void runNumberExitAction();
    
    inline TilePosition getTilePosition() { return data.p; }
    inline bool isEmpty() { return data.isEmpty; }
    
private:
    CC_SYNTHESIZE_READONLY(TileData, data, Data);
    CC_SYNTHESIZE_READONLY(int, number, Number);
    SB_SYNTHESIZE_READONLY_BOOL(selected, Selected);
    
    CC_SYNTHESIZE_READONLY(GameTile*, left, Left);
    CC_SYNTHESIZE_READONLY(GameTile*, right, Right);
    CC_SYNTHESIZE_READONLY(GameTile*, top, Top);
    CC_SYNTHESIZE_READONLY(GameTile*, bottom, Bottom);
    
    cocos2d::Sprite *bg;
    cocos2d::Label *numberLabel;
};

typedef std::vector<GameTile*> GameTileList;

#endif /* GameTile_hpp */
