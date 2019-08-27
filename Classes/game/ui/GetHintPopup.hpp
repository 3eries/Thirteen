//
//  GetHintPopup.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 27/08/2019.
//

#ifndef GetHintPopup_hpp
#define GetHintPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class GetHintPopup : public BasePopup {
public:
    CREATE_FUNC(GetHintPopup);
    ~GetHintPopup();
    
private:
    GetHintPopup();
    
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
    CC_SYNTHESIZE(std::function<void(int)>, onGetListener, OnGetListener);
    cocos2d::Node *popupBg;
};

#endif /* GetHintPopup_hpp */
