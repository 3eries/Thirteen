//
//  SBAnalytics.cpp
//
//  Created by seongmin hwang on 20/06/2019.
//

#include "SBAnalytics.hpp"

#if SB_PLUGIN_USE_FIREBASE_ANALYTICS
#include "../firebase/FirebaseAnalytics.hpp"
#endif

USING_NS_SB;
using namespace std;

void SBAnalytics::init() {
}

void SBAnalytics::setCurrentScreen(const string &screen, const string &screenClass) {
    
#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
    
#if SB_PLUGIN_USE_FIREBASE_ANALYTICS
    FirebaseAnalytics::setCurrentScreen(screen, screenClass);
#endif // SB_PLUGIN_USE_FIREBASE_ANALYTICS
    
#endif // COCOS2D_DEBUG == 0
}

void SBAnalytics::logEvent(const string &event, const EventParams &params) {

#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
    
#if SB_PLUGIN_USE_FIREBASE_ANALYTICS
    FirebaseAnalytics::logEvent(event, params);
#endif // SB_PLUGIN_USE_FIREBASE_ANALYTICS
    
#endif // COCOS2D_DEBUG == 0
}

string SBAnalytics::getNumberRange(int n, int begin, int end, int range) {
    
    while( true ) {
        if( n >= begin && n <= end ) {
            break;
        }
        
        begin = (end + 1);
        end += range;
    }
    
    return STR_FORMAT("%d-%d", begin, end);
}
