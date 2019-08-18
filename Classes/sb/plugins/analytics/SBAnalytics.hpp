//
//  SBAnalytics.hpp
//
//  Created by seongmin hwang on 20/06/2019.
//

#ifndef SBAnalytics_hpp
#define SBAnalytics_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBMacros.h"
#include "../../base/SBTypes.hpp"
#include "../SBPluginConfig.h"

class SBAnalytics {
public:
    static void init();
    static void setCurrentScreen(const std::string &screen,
                                 const std::string &screenClass = "");
    
    typedef cocos2d::ValueMap EventParams;
    typedef cocos2d::Value    EventParam;
    static void logEvent(const std::string &event, const EventParams &params = EventParams());
    
    static std::string getNumberRange(int n, int begin, int end, int range);
};

#endif /* SBAnalytics_hpp */
