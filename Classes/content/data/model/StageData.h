//
//  StageData.h
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef StageData_h
#define StageData_h

#include "cocos2d.h"
#include "superbomb.h"

typedef cocos2d::Vec2 TilePosition;
typedef std::vector<TilePosition> TilePositionList;

struct TileData {
    TilePosition p;
    bool isEmpty;
    
    TileData() : isEmpty(true) {}
};

typedef std::vector<TileData> TileDataList;

struct StageData {
    int             stage;                   // 스테이지
    int             clearCondition;          // 클리어 조건
    
    IntList         numbers;                 // 숫자 리스트
    IntList         numberWeights;           // 숫자 가중치 리스트
    int             numberWeightSum;         // 숫자 가중치 합계
    
    TileDataList    tiles;                   // 타일 리스트
    int             tileRows;                // 타일 가로줄 수
    int             tileColumns;             // 타일 세로줄 수
    
    StageData() : stage(0), numberWeightSum(0) {}
    
    void parse(const rapidjson::Value &v, rapidjson::Document::AllocatorType &allocator) {
        
        // int values
        {
            StringList keys({
                "level",
                "clear_condition",
            });
            
            std::vector<int*> ptrs({
                &stage,
                &clearCondition,
            });
            
            SBJSON::parse(v, allocator, keys, ptrs);
        }
        
        // number_list
        auto numberList = v["number_list"].GetArray();
        
        for( int i = 0; i < numberList.Size(); ++i ) {
            // const rapidjson::Value &v = list[i];
            numbers.push_back(numberList[i].GetInt());
        }
        
        // number_weight_list
        if( v.HasMember("number_weight_list") ) {
            auto numberWeightList = v["number_weight_list"].GetArray();
            
            for( int i = 0; i < numberWeightList.Size(); ++i ) {
                numberWeights.push_back(numberWeightList[i].GetInt());
            }
        }
        
        if( numberWeights.size() == 0 ) {
            for( int i = 0; i < numberList.Size(); ++i ) {
                numberWeights.push_back(100 / numberList.Size());
            }
        }
        
        for( auto n : numberWeights ) {
            numberWeightSum += n;
        }
        
        CCASSERT(numbers.size() == numberWeights.size(), "StageData number error: 숫자 리스트와 가중치 리스트의 크기가 다릅니다.");
        
        // tile
        CCLOG("LEVEL %d", stage);
        
        auto tileList = v["tile"].GetArray();
        tileRows = tileList.Size();
        tileColumns = tileList[0].GetArray().Size();
        
        for( int i = 0; i < tileRows; ++i ) {
            auto row = tileList[i].GetArray();
            int y = tileRows - i - 1;
            
            std::string str = "[";
            
            for( int x = 0; x < row.Size(); ++x ) {
                TileData tile;
                tile.p.x = x;
                tile.p.y = y;
                tile.isEmpty = (row[x].GetInt() == 0 );
                
                // str += STR_FORMAT("%d(%d,%d)", !tile.isEmpty ? 1 : 0, (int)x, (int)y);
                str += STR_FORMAT("%d", !tile.isEmpty ? 1 : 0);
                
                tiles.push_back(tile);
            }
            
            str += "]";
            CCLOG("%s", str.c_str());
        }
    }
    
    bool isNull() const {
        return stage == 0;
    }
    
    bool isTileEmpty(const TilePosition &p) const {
        for( auto tile : tiles ) {
            if( tile.p.equals(p) ) {
                return tile.isEmpty;
            }
        }
        
        return true;
    }
    
    std::string toString() {
        std::string str = "StageData {\n";
        str += STR_FORMAT("\tstage: %d, clearCondition: %d\n", stage, clearCondition);
        str += STR_FORMAT("\ttileSize: %dx%d\n", tileColumns, tileRows);
        
        str += "\tnumbers: ";
        for( int n : numbers ) {
            str += STR_FORMAT("%d,", n);
        }
        str += "\n";

        str += STR_FORMAT("\tnumberWeightSum: %d, list: ", numberWeightSum);
        for( int n : numberWeights ) {
            str += STR_FORMAT("%d,", n);
        }
        str += "\n";
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageDataList;

#endif /* StageData_h */
