//
//  Database.cpp
//
//  Created by seongmin hwang on 13/12/2018.
//

#include "Database.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "../ContentResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

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
    
    parseStageJson();
}

/**
 * stage_xxxx.json
 */
void Database::parseStageJson() {
    
    int stageId = 0;
    
    while( true ) {
        stageId++;
        
        string fileName = STR_FORMAT("stage_%04d.json", stageId);
        string filePath = DIR_CONTENT_DATA + fileName;
        
        if( !FileUtils::getInstance()->isFileExist(filePath) ) {
            break;
        }
        
        CCLOG("========== PARSE START (%s)  ==========", fileName.c_str());
        
        string json = SBStringUtils::readTextFile(filePath);
        
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        
        StageData stage;
        stage.parse(doc, allocator);

        stages.push_back(stage);
        
        CCLOG("========== PARSE END (%s)  ==========", fileName.c_str());
    } // stage loop
    
    // order by floor asc
    sort(stages.begin(), stages.end(), [](const StageData &s1, const StageData &s2) {
        return s1.stage < s2.stage;
    });
    
    // log
    for( auto stage : stages ) {
        CCLOG("%s", stage.toString().c_str());
    }
}

/**
 * 스테이지 데이터를 반환합니다
 */
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

