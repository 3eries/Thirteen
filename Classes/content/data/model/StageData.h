//
//  StageData.h
//
//  Created by seongmin hwang on 13/12/2018.
//

#ifndef StageData_h
#define StageData_h

#include "cocos2d.h"
#include "superbomb.h"

struct StageData {
    int            stage;                   // 스테이지
    
    StageData() : stage(0) {}
    
    void parse(const rapidjson::Value &v, rapidjson::Document::AllocatorType &allocator) {
        
        // int values
        {
            StringList keys({
                "stage",
            });
            
            std::vector<int*> ptrs({
                &stage,
            });
            
            SBJSON::parse(v, allocator, keys, ptrs);
        }
    }
    
    bool isNull() const {
        return stage == 0;
    }

    std::string toString() {
        std::string str = "StageData {\n";
        str += STR_FORMAT("\tstage: %d\n", stage);
        str += "}";
        
        return str;
    }
};

typedef std::vector<StageData>           StageDataList;

#endif /* StageData_h */
