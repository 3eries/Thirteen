//
//  GameDefine.h
//  Thirteen
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameDefine_h
#define GameDefine_h

#include "cocos2d.h"
#include "superbomb.h"

#define                 TILE_MAP_CONTENT_SIZE               cocos2d::Size(648,1024)
#define                 TILE_CONTENT_SIZE                   cocos2d::Size(84,84)
#define                 TILE_PADDING                        10

#define                 TILE_NORMAL_COLOR                   cocos2d::Color3B(255,255,255)
#define                 TILE_SELECTED_COLOR                 cocos2d::Color3B(0,255,207)

static inline cocos2d::Size getTileContentSize(int rows, int columns) {
    
    cocos2d::Size size;
    size.width += TILE_CONTENT_SIZE.width * columns;
    size.width += TILE_PADDING * (columns-1);
    size.height += TILE_CONTENT_SIZE.height * rows;
    size.height += TILE_PADDING * (rows-1);
    
    return size;
}

static inline cocos2d::Vec2 getTilePosition(int x, int y) {
    
    cocos2d::Vec2 pos;
    pos.x += TILE_CONTENT_SIZE.width * x;
    pos.y += TILE_CONTENT_SIZE.height * y;
    if( x > 0 ) pos.x += TILE_PADDING * x;
    if( y > 0 ) pos.y += TILE_PADDING * y;
    
    // 타일 중점
    pos.x += TILE_CONTENT_SIZE.width * 0.5f;
    pos.y += TILE_CONTENT_SIZE.height * 0.5f;
    
    return pos;
}

#endif /* GameDefine_h */
