//
//  SettingPopup.hpp
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#ifndef SettingPopup_hpp
#define SettingPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class SettingPopup : public BasePopup {
public:
    enum Tag {
        LEADER_BOARD       = 100,   // 리더 보드
        RESTORE_PURCHASE,           // 구매 아이템 복원
        REMOVE_ADS,                 // 광고 제거 아이템 구매
        HOW_TO_PLAY,                // 게임 가이드
    };
    
public:
    CREATE_FUNC(SettingPopup);
    ~SettingPopup();
    
private:
    SettingPopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void performListener(Tag tag);
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(std::function<void(Tag)>, onClickMenuListener,
                  OnClickMenuListener);
    cocos2d::Node *stoneBg;
};

#endif /* SettingPopup_hpp */
