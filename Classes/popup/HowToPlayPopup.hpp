//
//  HowToPlayPopup.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 30/08/2019.
//

#ifndef HowToPlayPopup_hpp
#define HowToPlayPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class HowToPlayPopup : public BasePopup {
public:
    CREATE_FUNC(HowToPlayPopup);
    ~HowToPlayPopup();
    
private:
    HowToPlayPopup();
    
    bool init() override;
    void onEnter() override;
    bool onBackKeyReleased() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    SBSkeletonAnimation *anim;
    int animLoopCount;
    
    SB_SYNTHESIZE_BOOL(tutorial, Tutorial);
};

#endif /* HowToPlayPopup_hpp */
