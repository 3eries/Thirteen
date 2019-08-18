//
//  StageProgressBar.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef StageProgressBar_hpp
#define StageProgressBar_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../GameManager.hpp"
#include "../../content/data/model/StageData.h"

class StageProgressBar : public cocos2d::Node {
public:
    CREATE_FUNC(StageProgressBar);
    ~StageProgressBar();
    
private:
    StageProgressBar();
    
    bool init() override;
    
    void initUI();
    void initGameListener();
    
public:
    void setPercentage(float percentage);
    
// Game Event
public:
    void onStageChanged(const StageData &stage);
    void onStageRestart(const StageData &stage);
    void onStageClear(const StageData &stage);
    
private:
    cocos2d::ProgressTimer *gage;
    cocos2d::Label *currStageLabel;
    cocos2d::Label *nextStageLabel;
};

#endif /* StageProgressBar_hpp */
