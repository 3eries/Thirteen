//
//  Database.cpp
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Database.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "User.hpp"
#include "../ContentResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

#define LEVEL_FILE               (DIR_CONTENT_DATA + "level.json")

static Database *instance = nullptr;
Database* Database::getInstance() {
    
    if( !instance ) {
        instance = new Database();
    }
    
    return instance;
}

void Database::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

Database::Database() {
    
}

Database::~Database() {
    
}

void Database::init() {
    
    parseLevelJson();
}

/**
 * level.json
 */
void Database::parseLevelJson() {
    
    CCLOG("========== PARSE START (level.json)  ==========");
    string json = SBStringUtils::readTextFile(LEVEL_FILE);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

    auto list = doc.GetArray();
    
    for( int i = 0; i < list.Size(); ++i ) {
        const rapidjson::Value &v = list[i];
        
        StageData stage;
        stage.parse(v, allocator);
        
        stages.push_back(stage);
    }
    
    CCLOG("========== PARSE END (level.json)  ==========");
    
    // order by floor asc
    sort(stages.begin(), stages.end(), [](const StageData &s1, const StageData &s2) {
        return s1.stage < s2.stage;
    });
    
    originStages = stages;
    
    // log
    for( auto stage : stages ) {
        CCLOG("%s", stage.toString().c_str());
    }
    
    // valid
//    for( int i = 0; i < 3; ++i ) {
//        addVirtualLevel();
//    }
    //
    
    // 가상 스테이지 추가
    int addCount = (User::getClearStage()+1) - getLastStage().stage;
    CCLOG("addCount: %d", addCount);
    
    for( int i = 0; i < addCount; ++i ) {
        addVirtualLevel();
    }
}

/**
 * 스테이지 데이터를 반환합니다
 */
StageDataList Database::getOriginalStages() {
    
    return instance->originStages;
}

StageDataList Database::getStages() {
    
    return instance->stages;
}

StageData Database::getStage(int stage) {
    
    auto stages = getStages();
    
    for( auto stageData : stages ) {
        if( stageData.stage == stage ) {
            return stageData;
        }
    }
    
    return StageData();
}

StageData Database::getFirstStage() {
    
    auto stages = getStages();
    return stages[0];
}

StageData Database::getLastStage() {
    
    auto stages = getStages();
    return stages.size() > 0 ? stages[stages.size()-1] : StageData();
}

/**
 * 가상 레벨을 추가합니다
 */
StageData Database::addVirtualLevel() {
    
    auto virtualLevel = createVirtualLevel();
    
    auto &levels = instance->stages;
    levels.push_back(virtualLevel);
    
    return virtualLevel;
}

/**
 * 가상 레벨을 생성합니다
 */
StageData Database::createVirtualLevel() {
    
    auto levels = getOriginalStages();
    auto randomLevel = levels[arc4random() % levels.size()];
    
    StageData level;
    level.isVirtual = true;
    level.stage = getLastStage().stage+1;
    
    // 타일 개수 설정
    level.tileRows = SBMath::random(2, 10);
    level.tileColumns = SBMath::random(2, 7);
    level.numberWeightSum = 100;
    
    int totalTileCount = level.tileRows * level.tileColumns;
    int removeTileCount = (totalTileCount <= 16) ? 0 : totalTileCount * (SBMath::random(3,5) * 0.1f);
    int tileCount = totalTileCount - removeTileCount;
    
    if( tileCount <= 10 ) {
        level.numbers       = IntList({ 5,8 });
        level.numberWeights = IntList({ 50,50 });
    }
    else if( tileCount <= 20 ) {
        level.numbers       = IntList({ 2,3,4 });
        level.numberWeights = IntList({ 33,34,33 });
    }
    else if( tileCount <= 30 ) {
        level.numbers       = IntList({ 2,3,4,5 });
        level.numberWeights = IntList({ 20,35,25,20 });
    }
    else if( tileCount <= 50 ) {
        level.numbers       = IntList({ 1,2,3,6,7, });
        level.numberWeights = IntList({ 10,25,35,15,15 });
    }
    else {
        level.numbers       = IntList({ 1,2,3,5,8,10,12 });
        level.numberWeights = IntList({ 7,20,30,15,15,8,5 });
    }
    
    level.clearCondition = round(totalTileCount / 3.3f);
    level.clearCondition *= (level.clearCondition <= 2) ? 2 : 1;
    
    CCLOG("createVirtualLevel: %d, totalTileCount: %d, removeTileCount: %d",
          level.stage, totalTileCount, removeTileCount);
    CCLOG("%s", level.toString().c_str());
    
    for( int y = 0; y < level.tileRows; ++y ) {
        for( int x = 0; x < level.tileColumns; ++x ) {
            TileData tile;
            tile.p.x = x;
            tile.p.y = y;
            tile.isEmpty = false;
            
            level.tiles.push_back(tile);
        }
    }
    
    if( removeTileCount > 0 ) {
        auto removeAnchorTile = level.tiles[arc4random() % level.tiles.size()].p;
        
        TilePositionList removePosList;
        removePosList.push_back(removeAnchorTile);

        for( int i = 0; i < removeTileCount-1; ++i ) {
            auto safeAdd = [=](TilePositionList &posList, const TilePosition &p) -> bool {
                // 맵 범위 체크
                if( p.x >= 0 && p.x < level.tileColumns &&
                    p.y >= 0 && p.y < level.tileRows ) {
                    
                    // 좌표 중복 방지
                    for( auto removePos : removePosList ) {
                        if( removePos.equals(p) ) {
                            return false;
                        }
                    }
                    
                    posList.push_back(p);
                    return true;
                }
                
                return false;
            };
            
            TilePositionList posList;
            safeAdd(posList, removeAnchorTile + TilePosition(-1,0));
            safeAdd(posList, removeAnchorTile + TilePosition(1,0));
            safeAdd(posList, removeAnchorTile + TilePosition(0,1));
            safeAdd(posList, removeAnchorTile + TilePosition(0,-1));
            
            if( posList.size() > 0 ) {
                removeAnchorTile = posList[arc4random() % posList.size()];
                removePosList.push_back(removeAnchorTile);
            }
        }
        
        for( auto removePos : removePosList ) {
            SBCollection::remove(level.tiles, [removePos](const TileData &tile) -> bool {
                return tile.p.equals(removePos);
            });
        }
    }
    
    // log
    for( int y = 0; y < level.tileRows; ++y ) {
        string str = "[";
        
        for( int x = 0; x < level.tileColumns; ++x ) {
            auto tile = level.getTile(TilePosition(x,y));
            str += STR_FORMAT("%d", !tile.isEmpty ? 1 : 0);
        }
        
        str += "]";
        CCLOG("%s", str.c_str());
    }
    
    return level;
}
