//
//  AdsHelper-ios.mm
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#include "AdsHelper.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import "ios/AdsManager.h"

#include "../../platform/SBPlatformMacros.h"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

/**
 * 광고 모듈 초기화
 */
void AdsHelper::initImpl(const AdsConfig &config) {
    
#if SB_PLUGIN_USE_ADS
    // 매니저 초기화
    AdsManager *mgr = [AdsManager getInstance];
    [mgr initAd:config];
    
    mgr.onBannerListener = banner.listener;
    mgr.onInterstitialListener = interstitial.listener;
    mgr.onRewardedVideoListener = (RewardedVideoAdListener*)rewardedVideo.listener;
#endif
}

/**
 * 배너 광고 로드
 */
void AdsHelper::loadBanner() {
    
#if SB_PLUGIN_USE_ADS
    if( !banner.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadBanner];
#endif
}

/**
 * 전면 광고 로드
 */
void AdsHelper::loadInterstitial() {
    
#if SB_PLUGIN_USE_ADS
    if( !interstitial.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadInterstitial];
#endif
}

/**
 * 보상형 동영상 광고 로드
 */
void AdsHelper::loadRewardedVideo() {
    
#if SB_PLUGIN_USE_ADS
    if( !rewardedVideo.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadRewardedVideo];
#endif
}

/**
 * 배너 광고
 */
void AdsHelper::showBanner(AdListener *listener) {
    
#if SB_PLUGIN_USE_ADS
    if( !getInstance()->getBanner().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::BANNER, listener);
    
    [[AdsManager getInstance] showBanner];
#endif
}

void AdsHelper::hideBanner() {

#if SB_PLUGIN_USE_ADS
    /*
    if( !banner.active ) {
        return;
    }
     */
    
    [[AdsManager getInstance] hideBanner];
#endif
}

/**
 * 전면 광고
 */
void AdsHelper::showInterstitial(AdListener *listener) {
    
#if SB_PLUGIN_USE_ADS
    if( !getInstance()->getInterstitial().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::INTERSTITIAL, listener);
    
    [[AdsManager getInstance] showInterstitial];
#endif
}

/**
 * 보상형 비디오
 */
void AdsHelper::showRewardedVideo(AdListener *listener) {
    
#if SB_PLUGIN_USE_ADS
    if( !getInstance()->getRewardedVideo().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::REWARDED_VIDEO, listener);
    
    [[AdsManager getInstance] showRewardedVideo];
#endif
}

bool AdsHelper::isBannerVisible() {
#if SB_PLUGIN_USE_ADS
    return [[AdsManager getInstance] isBannerVisible];
#else
    return false;
#endif
}

bool AdsHelper::isBannerLoaded() {
#if SB_PLUGIN_USE_ADS
    return [[AdsManager getInstance] isBannerLoaded];
#else
    return false;
#endif
}

bool AdsHelper::isInterstitialLoaded() {
#if SB_PLUGIN_USE_ADS
    return [[AdsManager getInstance] isInterstitialLoaded];
#else
    return false;
#endif
}

bool AdsHelper::isRewardedVideoLoaded() {
#if SB_PLUGIN_USE_ADS
    return [[AdsManager getInstance] isRewardedVideoLoaded];
#else
    return false;
#endif
}

float AdsHelper::getBannerWidth() {
#if SB_PLUGIN_USE_ADS
    float w = ptToPx([[AdsManager getInstance] getBannerWidth]);
    w /= Director::getInstance()->getOpenGLView()->getScaleX();
    
    return w;
#else
    return 0;
#endif
}

float AdsHelper::getBannerHeight() {
#if SB_PLUGIN_USE_ADS
    float h = ptToPx([[AdsManager getInstance] getBannerHeight]);
    h /= Director::getInstance()->getOpenGLView()->getScaleY();
    
    return h;
#else
    return 0;
#endif
}

NS_SB_END;

#endif // CC_PLATFORM_IOS
