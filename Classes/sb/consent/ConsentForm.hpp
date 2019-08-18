//
//  ConsentForm.hpp
//
//  Created by seongmin hwang on 2018. 9. 26..
//

#ifndef ConsentForm_hpp
#define ConsentForm_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../base/SBMacros.h"

NS_SB_BEGIN;

class ConsentForm : public cocos2d::Node {
public:
    static ConsentForm* create();
    virtual ~ConsentForm();
    
private:
    ConsentForm();
    virtual bool init() override;
    
public:
    ConsentForm* show();
    static void showImpl(const std::string &title, const std::string &message,
                         const std::string &policyUrl);
    
private:
    SB_SYNTHESIZE_BUILDER(ConsentForm*, std::string, title, Title);
    SB_SYNTHESIZE_BUILDER(ConsentForm*, std::string, message, Message);
    SB_SYNTHESIZE_BUILDER(ConsentForm*, std::string, policyUrl, PolicyUrl);
};

NS_SB_END;

#endif /* ConsentForm_hpp */
