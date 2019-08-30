//
//  Database.hpp
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "model/StageData.h"

/** @class Database
 * @brief 이 클래스는 컨텐츠 데이터를 관리합니다
 */
class Database {
public:
    static Database* getInstance();
    static void destroyInstance();
    ~Database();
    
    void init();
    
private:
    Database();
    
    void parseLevelJson();
    
public:
    static StageDataList       getOriginalStages();
    static StageDataList       getStages();
    static StageData           getStage(int stage);
    static StageData           getFirstStage();
    static StageData           getLastStage();
    
    static StageData           addVirtualLevel();
    static StageData           createVirtualLevel();

private:
    StageDataList originStages;
    StageDataList stages;
};

#endif /* Database_hpp */
