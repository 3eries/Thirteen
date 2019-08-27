//
//  HintButton.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 20/08/2019.
//

#ifndef HintButton_hpp
#define HintButton_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class HintButton : public cocos2d::ui::Widget {
public:
    CREATE_FUNC(HintButton);
    ~HintButton();
    
private:
    HintButton();
    
    bool init() override;
    
    void onClick();
    void updateHintCount();
    
private:
    CC_SYNTHESIZE(SBCallback, onHintListener, OnHintListener);
    
    cocos2d::Node *hintCountView;
    cocos2d::Label *hintCountLabel;
};

#endif /* HintButton_hpp */
