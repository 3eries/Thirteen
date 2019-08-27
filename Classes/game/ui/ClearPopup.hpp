//
//  ClearPopup.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 27/08/2019.
//

#ifndef ClearPopup_hpp
#define ClearPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ClearPopup : public BasePopup {
public:
    CREATE_FUNC(ClearPopup);
    ~ClearPopup();
    
private:
    ClearPopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    bool onBackKeyReleased() override;
    
    void onClick();
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(SBCallback, onNextListener, OnNextListener);
    cocos2d::Node *popupBg;
};

#endif /* ClearPopup_hpp */
