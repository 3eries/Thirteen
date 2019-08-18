//
//  AdsManager.h
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#ifndef AdsManager_h
#define AdsManager_h

#include "../../SBPluginConfig.h"

#if SB_PLUGIN_USE_ADS

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <GoogleMobileAds/GoogleMobileAds.h>

#include "cocos2d.h"
#include "AdListener.hpp"

@interface AdsManager : NSObject<GADBannerViewDelegate, GADInterstitialDelegate, GADRewardBasedVideoAdDelegate> {
    GADBannerView *bannerView;
    GADInterstitial *interstitialAd;
    GADRewardBasedVideoAd *rewardedVideoAd;
    
    NSString *interstitialUnitId;
    NSString *rewardedVideoUnitId;
    NSMutableArray *testDevices;
    
    id<GADAdNetworkExtras> vungleExtras;
    
    BOOL isBannerLoaded;
    BOOL isBannerLoading;
    BOOL isInterstitialLoading;
    BOOL isRewardedVideoLoading;
}

@property (nonatomic, readwrite) superbomb::AdListener *onBannerListener;
@property (nonatomic, readwrite) superbomb::AdListener *onInterstitialListener;
@property (nonatomic, readwrite) superbomb::RewardedVideoAdListener *onRewardedVideoListener;

+ (AdsManager*) getInstance;

- (void) initAd:(const superbomb::AdsConfig&)config;

- (void) loadBanner;
- (void) loadInterstitial;
- (void) loadRewardedVideo;

- (void) showBanner;
- (void) hideBanner;

- (void) showInterstitial;
- (void) showRewardedVideo;

- (BOOL) isBannerVisible;
- (BOOL) isBannerLoaded;
- (BOOL) isInterstitialLoaded;
- (BOOL) isRewardedVideoLoaded;

- (float) getBannerWidth;
- (float) getBannerHeight;

// GADBannerViewDelegate
- (void) adViewDidReceiveAd:(GADBannerView *)bannerView;
- (void) adView:(GADBannerView *)bannerView didFailToReceiveAdWithError:(GADRequestError *)error;
- (void) adViewWillPresentScreen:(GADBannerView *)bannerView;
- (void) adViewWillDismissScreen:(GADBannerView *)bannerView;
- (void) adViewDidDismissScreen:(GADBannerView *)bannerView;
- (void) adViewWillLeaveApplication:(GADBannerView *)bannerView;

// GADInterstitialDelegate
- (void) interstitialDidReceiveAd:(GADInterstitial *)ad;
- (void) interstitial:(GADInterstitial *)ad
didFailToReceiveAdWithError:(GADRequestError *)error;
- (void) interstitialWillPresentScreen:(GADInterstitial *)ad;
- (void) interstitialDidDismissScreen:(GADInterstitial *)ad;

// GADRewardBasedVideoAdDelegate
- (void) rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd;
- (void) rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
     didFailToLoadWithError:(NSError *)error;
- (void) rewardBasedVideoAdDidOpen:(GADRewardBasedVideoAd *)rewardBasedVideoAd;
- (void) rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd;
- (void) rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
    didRewardUserWithReward:(GADAdReward *)reward;

@end

#endif // SB_PLUGIN_USE_ADS

#endif /* AdsManager_h */
