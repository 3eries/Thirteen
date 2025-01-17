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
    static GameTile* create();
    ~GameTile();
    
private:
    GameTile();
    
    bool init() override;
    
public:
    void remove();
    void clear();
    
    void setTilePosition(const TilePosition &p);
    void setTileId(int tileId);
    void setNumber(int number);
    void setSelected(bool isSelected);
    void setNearTile(GameTile *left, GameTile *right, GameTile *top, GameTile *bottom);
    
    void updateSelectedLine(std::function<bool(GameTile*)> isTileSelected,
                            const cocos2d::Color3B &lineColor,
                            bool withAction = false);
    void updateSelectedLine();
    
    void runNumberEnterAction();
    void runNumberExitAction();
    
private:
    CC_SYNTHESIZE_READONLY(TilePosition, tilePos, TilePosition);
    CC_SYNTHESIZE_READONLY(int, tileId, TileId);
    CC_SYNTHESIZE_READONLY(int, number, Number);
    SB_SYNTHESIZE_READONLY_BOOL(selected, Selected);
    
    CC_SYNTHESIZE_READONLY(GameTile*, left, Left);
    CC_SYNTHESIZE_READONLY(GameTile*, right, Right);
    CC_SYNTHESIZE_READONLY(GameTile*, top, Top);
    CC_SYNTHESIZE_READONLY(GameTile*, bottom, Bottom);
    
    cocos2d::Sprite *bg;
    cocos2d::Label *numberLabel;
    cocos2d::Node* selectedLine;
};

typedef std::vector<GameTile*> GameTileList;

#endif /* GameTile_hpp */
