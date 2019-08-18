//
//  AdsHelper.hpp
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#ifndef AdsHelper_hpp
#define AdsHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBMacros.h"
#include "../../base/SBTypes.hpp"
#include "../SBPluginConfig.h"

#include "AdsDefine.h"
#include "AdsEventDispatcher.hpp"

NS_SB_BEGIN;

class AdsHelper : public cocos2d::Ref {
public:
    static AdsHelper* getInstance();
    static void destroyInstance();
    
    ~AdsHelper();
    
private:
    AdsHelper();
    
    void initListeners();
    
public:
    void init(const AdsConfig &config);
    void initImpl(const AdsConfig &config);
    
    void setActiveBanner(bool active);
    void setActiveInterstitial(bool active);
    void setActiveRewardedVideo(bool active);
    
    void setAutoLoad(float interval);
    
    void loadBanner();
    void loadInterstitial();
    void loadRewardedVideo();

    static void showBanner(AdListener *listener = nullptr);
    static void hideBanner();
    
    static void showInterstitial(AdListener *listener = nullptr);
    static void showRewardedVideo(AdListener *listener = nullptr);
    
    static bool isBannerVisible();
    
    static bool isBannerLoaded();
    static bool isInterstitialLoaded();
    static bool isRewardedVideoLoaded();
    
    static float getBannerWidth();
    static float getBannerHeight();
    
private:
    CC_SYNTHESIZE_READONLY(AdsConfig, config, Config);
    
    bool isInitialized;
    CC_SYNTHESIZE_READONLY(AdsEventDispatcher*, eventDispatcher, EventDispatcher);
    
    float autoLoadInterval;     // 광고 자동 로드 시간 간격 (단위 : 초)
    SB_SYNTHESIZE_READONLY_BOOL(adOpened, AdOpened); // 광고 오픈 여부
    
    struct Ad {
        AdType type;            // 광고 타입
        AdListener *listener;   // 광고 리스너
        bool active;            // 광고 활성화 여부
        
        Ad(AdType _type) : type(_type), listener(nullptr), active(true) {}
    };
    
    CC_SYNTHESIZE_READONLY(Ad, banner, Banner);
    CC_SYNTHESIZE_READONLY(Ad, interstitial, Interstitial);
    CC_SYNTHESIZE_READONLY(Ad, rewardedVideo, RewardedVideo);
};

NS_SB_END;

#endif /* AdsHelper_hpp */
