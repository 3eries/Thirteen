//
//  AdsManager.m
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#import "AdsManager.h"

#if SB_PLUGIN_USE_ADS

#import "AppController.h"
#import "RootViewController.h"

#include "../AdsHelper.hpp"
#include "../../../base/SBDirector.hpp"
#include "../../../platform/SBPlatformMacros.h"

USING_NS_CC;
USING_NS_SB;
using namespace std;

@interface AdsManager (Private)

- (id) init;
- (GADRequest*) createRequest;

- (void) initTestDevices:(vector<string>)devices;
- (void) initBanner:(const string&)unitId;
- (void) initInterstitial:(const string&)unitId;
- (void) initRewardedVideo:(const string&)unitId;

@end

@implementation AdsManager

+ (AdsManager*) getInstance {
    
    static AdsManager *instance = nil;
    static dispatch_once_t oncePredicate;
    
    dispatch_once(&oncePredicate, ^{
        instance = [[AdsManager alloc] init];
    });
    
    return instance;
}

- (id) init {
    
    if( self = [super init] ) {
        bannerView = nil;
        interstitialAd = nil;
        rewardedVideoAd = nil;
        
        interstitialUnitId = nil;
        rewardedVideoUnitId = nil;
        testDevices = nil;
        
        vungleExtras = nil;
    }
    
    return self;
}

- (GADRequest*) createRequest {
    
    GADRequest *request = [GADRequest request];
    request.testDevices = testDevices;
    
    return request;
}

/**
 * 광고 모듈 초기화
 */
- (void) initAd:(const AdsConfig&)config {
    
    // Initialize the Google Mobile Ads SDK.
    // [GADMobileAds configureWithApplicationID:NS_STRING(config.appId.c_str())];
    [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
  
    // 테스트 기기 초기화
    [self initTestDevices:config.testDevices];
    
    // 광고 단위 초기화
    [self initBanner:config.bannerUnitId];
    [self initInterstitial:config.interstitialUnitId];
    [self initRewardedVideo:config.rewardedVideoUnitId];
    
    // TODO: 어댑터 초기화
}

/**
 * Vungle 어댑터 초기화
 */
//- (void) initVungleAdapter:(vector<string>)placements {
//
//    NSMutableArray *array = [NSMutableArray array];
//
//    for( string placement : placements ) {
//        [array addObject:NS_STRING(placement)];
//    }
//
////    VungleAdNetworkExtras *extras = [[VungleAdNetworkExtras alloc] init];
////    extras.allPlacements = array;
////
////    vungleExtras = extras;
//}

/**
 * 테스트 기기 초기화
 */
- (void) initTestDevices:(vector<string>)devices {
    
    testDevices = [[NSMutableArray alloc] init];
    [testDevices addObject:kGADSimulatorID];
    
    for( string device : devices ) {
        [testDevices addObject:NS_STRING(device.c_str())];
    }
}

/**
 * 배너 광고 초기화
 */
- (void) initBanner:(const string&)unitId {
    
    NSLog(@"initBanner unitId: %s", unitId.c_str());
    
    if( unitId == "" ) {
        return;
    }
    
    GADAdSize bannerSize =
                    !SBDirector::isPadResolution() ? kGADAdSizeBanner : kGADAdSizeSmartBannerPortrait;
    
    bannerView = [[GADBannerView alloc] initWithAdSize:bannerSize];
    bannerView.adUnitID = NS_STRING(unitId.c_str());
    bannerView.hidden = YES;
    bannerView.translatesAutoresizingMaskIntoConstraints = NO; // example
    bannerView.delegate = self;
    bannerView.rootViewController = ROOT_VIEW_CONTROLLER;
    
    UIView *view = ROOT_VIEW_CONTROLLER.view;
    [view addSubview:bannerView];
    
    // set frame
    /*
    CGRect frame = bannerView.frame;
    frame.origin.x = 0;
    frame.origin.y = view.frame.size.height - frame.size.height;
    
    bannerView.frame = frame;
    */
    
    // example
    if( @available(iOS 11.0, *) ) {
        // In iOS 11, we need to constrain the view to the safe area.
        // Position the banner. Stick it to the bottom of the Safe Area.
        // Make it constrained to the edges of the safe area.
        UILayoutGuide *guide = view.safeAreaLayoutGuide;
        
        // align screen bottom
        [NSLayoutConstraint activateConstraints:@[
                                                  [guide.leftAnchor constraintEqualToAnchor:bannerView.leftAnchor],
                                                  [guide.rightAnchor constraintEqualToAnchor:bannerView.rightAnchor],
                                                  [guide.bottomAnchor constraintEqualToAnchor:bannerView.bottomAnchor]
                                                  ]];
        // align screen top
        /*
        [NSLayoutConstraint activateConstraints:@[
                                                  [guide.leftAnchor constraintEqualToAnchor:bannerView.leftAnchor],
                                                  [guide.rightAnchor constraintEqualToAnchor:bannerView.rightAnchor],
                                                  [guide.topAnchor constraintEqualToAnchor:bannerView.topAnchor]
                                                  ]];
         */
        
    } else {
        // In lower iOS versions, safe area is not available so we use
        // bottom layout guide and view edges.
        [view addConstraint:[NSLayoutConstraint constraintWithItem:bannerView
                                                         attribute:NSLayoutAttributeLeading
                                                         relatedBy:NSLayoutRelationEqual
                                                            toItem:view
                                                         attribute:NSLayoutAttributeLeading
                                                        multiplier:1
                                                          constant:0]];
        [view addConstraint:[NSLayoutConstraint constraintWithItem:bannerView
                                                         attribute:NSLayoutAttributeTrailing
                                                         relatedBy:NSLayoutRelationEqual
                                                            toItem:view
                                                         attribute:NSLayoutAttributeTrailing
                                                        multiplier:1
                                                          constant:0]];
        // align screen bottom
        [view addConstraint:[NSLayoutConstraint constraintWithItem:bannerView
                                                         attribute:NSLayoutAttributeBottom
                                                         relatedBy:NSLayoutRelationEqual
                                                            toItem:ROOT_VIEW_CONTROLLER.bottomLayoutGuide
                                                         attribute:NSLayoutAttributeTop
                                                        multiplier:1
                                                          constant:0]];
        // align screen top
        /*
        [view addConstraint:[NSLayoutConstraint constraintWithItem:bannerView
                                                         attribute:NSLayoutAttributeTop
                                                         relatedBy:NSLayoutRelationEqual
                                                            toItem:ROOT_VIEW_CONTROLLER.topLayoutGuide
                                                         attribute:NSLayoutAttributeBottom
                                                        multiplier:1
                                                          constant:0]];
         */
    }
    
    /*
    [view addConstraints:@[
                           [NSLayoutConstraint constraintWithItem:bannerView
                                                        attribute:NSLayoutAttributeBottom
                                                        relatedBy:NSLayoutRelationEqual
                                                           toItem:self.bottomLayoutGuide
                                                        attribute:NSLayoutAttributeTop
                                                       multiplier:1
                                                         constant:0],
                           [NSLayoutConstraint constraintWithItem:bannerView
                                                        attribute:NSLayoutAttributeCenterX
                                                        relatedBy:NSLayoutRelationEqual
                                                           toItem:view
                                                        attribute:NSLayoutAttributeCenterX
                                                       multiplier:1
                                                         constant:0]
                           ]];
    */
    
    // load
    [self loadBanner];
}

/**
 * OnAdLoaded
 * Tells the delegate an ad request loaded an ad.
 */
- (void) adViewDidReceiveAd:(GADBannerView *)bannerView {
    
    AdsHelper::getInstance()->getBanner()->onAdLoaded();
}

/**
 * OnAdFailedToLoad
 * Tells the delegate an ad request failed.
 */
- (void) adView:(GADBannerView *)bannerView
didFailToReceiveAdWithError:(GADRequestError *)error {
    
    NSLog(@"adView:didFailToReceiveAdWithError: %@", [error localizedDescription]);

    int errorCode = [[NSNumber numberWithInteger:error.code] intValue];
    AdsHelper::getInstance()->getBanner()->onAdFailedToLoad(errorCode);
}

/**
 * OnAdOpened
 * Tells the delegate that a full-screen view will be presented in response
 * to the user clicking on an ad.
 */
- (void) adViewWillPresentScreen:(GADBannerView *)bannerView {
    NSLog(@"adViewWillPresentScreen");
    
    AdsHelper::getInstance()->getBanner()->onAdOpened();
}

/**
 *
 * Tells the delegate that the full-screen view will be dismissed.
 */
- (void) adViewWillDismissScreen:(GADBannerView *)bannerView {
    NSLog(@"adViewWillDismissScreen");
}

/**
 * OnAdClosed
 * Tells the delegate that the full-screen view has been dismissed.
 */
- (void) adViewDidDismissScreen:(GADBannerView *)bannerView {
    NSLog(@"adViewDidDismissScreen");
    
    AdsHelper::getInstance()->getBanner()->onAdClosed();
    [self loadBanner];
}

/**
 * Tells the delegate that a user click will open another app (such as
 * the App Store), backgrounding the current app.
 */
- (void) adViewWillLeaveApplication:(GADBannerView *)bannerView {
    NSLog(@"adViewWillLeaveApplication");
}

/**
 * 전면 광고 초기화
 */
- (void) initInterstitial:(const string&)unitId {
    
    if( unitId == "" ) {
        return;
    }
    
    interstitialUnitId = NS_STRING(unitId.c_str());
    [self loadInterstitial];
}

/**
 * OnAdLoaded
 */
- (void) interstitialDidReceiveAd:(GADInterstitial *)ad {
    AdsHelper::getInstance()->getInterstitial()->onAdLoaded();
}

/**
 * OnAdFailedToLoad
 */
- (void) interstitial:(GADInterstitial *)ad
didFailToReceiveAdWithError:(GADRequestError *)error {
    
    NSLog(@"interstitial didFailToReceiveAdWithError: %@", [error description]);
    
    int errorCode = [[NSNumber numberWithInteger:error.code] intValue];
    AdsHelper::getInstance()->getInterstitial()->onAdFailedToLoad(errorCode);
}

/**
 * OnAdOpened
 */
- (void) interstitialWillPresentScreen:(GADInterstitial *)ad {
    
    Application::getInstance()->applicationDidEnterBackground();
    
    AdsHelper::getInstance()->getInterstitial()->onAdOpened();
}

/**
 * OnAdClosed
 */
- (void) interstitialDidDismissScreen:(GADInterstitial *)ad {
    
    AdsHelper::getInstance()->getInterstitial()->onAdClosed();
    [self loadInterstitial];
    
    Application::getInstance()->applicationWillEnterForeground();
}

/**
 * 보상형 비디오 초기화
 */
- (void) initRewardedVideo:(const string&)unitId {
    
    if( unitId == "" ) {
        return;
    }
    
    rewardedVideoUnitId = NS_STRING(unitId.c_str());
    
    rewardedVideoAd = [GADRewardBasedVideoAd sharedInstance];
    rewardedVideoAd.delegate = self;
    
    [self loadRewardedVideo];
}

/**
 * OnAdLoaded
 */
- (void) rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    AdsHelper::getInstance()->getRewardedVideo()->onAdLoaded();
}

/**
 * OnAdFailedToLoad
 */
- (void) rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
     didFailToLoadWithError:(NSError *)error {

    NSLog(@"rewardBasedVideoAd didFailToLoadWithError: %@", [error description]);
    
    int errorCode = [[NSNumber numberWithInteger:error.code] intValue];
    AdsHelper::getInstance()->getRewardedVideo()->onAdFailedToLoad(errorCode);
}

/**
 * OnAdOpened
 */
- (void) rewardBasedVideoAdDidOpen:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    
    Application::getInstance()->applicationDidEnterBackground();
    // SBAudioEngine::pauseAll();
    
    AdsHelper::getInstance()->getRewardedVideo()->onAdOpened();
}

/**
 * OnAdClosed
 */
- (void) rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    
    AdsHelper::getInstance()->getRewardedVideo()->onAdClosed();
    [self loadRewardedVideo];
    
    Application::getInstance()->applicationWillEnterForeground();
}

/**
 * OnRewarded
 */
- (void) rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
    didRewardUserWithReward:(GADAdReward *)reward {
    AdsHelper::getInstance()->getRewardedVideo()->onRewarded([reward.type UTF8String],
                                                             [reward.amount intValue]);
}

- (bool) loadBanner {
    
    NSLog(@"AdsManager::loadBanner bannerView: %d", bannerView != nil);
    
    if( bannerView == nil ) {
        return false;
    }
    
    GADRequest *request = [self createRequest];
    [bannerView loadRequest:request];
    
    return true;
}

- (bool) loadInterstitial {
    
    NSLog(@"AdsManager::loadInterstitial");

    if( !interstitialUnitId || [interstitialUnitId isEqualToString:@""] ) {
        return false;
    }
    
    interstitialAd = [[GADInterstitial alloc] initWithAdUnitID:interstitialUnitId];
    interstitialAd.delegate = self;
    
    GADRequest *request = [self createRequest];
    [interstitialAd loadRequest:request];
    
    return true;
}

- (bool) loadRewardedVideo {
    
    NSLog(@"AdsManager::loadRewardedVideo");
    
    if( !rewardedVideoAd || !rewardedVideoUnitId || [rewardedVideoUnitId isEqualToString:@""] ) {
        return false;
    }
    
    GADRequest *request = [self createRequest];
    [rewardedVideoAd loadRequest:request withAdUnitID:rewardedVideoUnitId];
    
    return true;
}

/**
 * 배너 광고 노출
 */
- (void) showBanner {
    
    if( bannerView ) {
        bannerView.hidden = NO;
    }
    
    /*
    // 광고 로드됨
    if( banner->isLoaded() ) {
        bannerView.hidden = NO;
    }
    // 광고 로드 필요
    else {
        NSLog(@"The banner wasn't loaded yet.");
        [self loadBanner];
    }
    */
}

/**
 * 배너 광고 숨김
 */
- (void) hideBanner {
    
    if( bannerView ) {
        bannerView.hidden = YES;
    }
}

/**
 * 전면 광고 노출
 */
- (void) showInterstitial {
    
    if( !interstitialAd ) {
        return;
    }
    
    // 광고 로딩됨, 광고 노출
    if( interstitialAd.isReady ) {
        [interstitialAd presentFromRootViewController:ROOT_VIEW_CONTROLLER];
    }
    // 광고 로딩
    else {
        NSLog(@"The interstitial wasn't loaded yet.");
        [self loadInterstitial];
    }
}

/**
 * 보상형 비디오 노출
 */
- (void) showRewardedVideo {

    if( !rewardedVideoAd ) {
        return;
    }
    
    // 광고 로딩됨, 광고 노출
    if( rewardedVideoAd.isReady ) {
        [rewardedVideoAd presentFromRootViewController:ROOT_VIEW_CONTROLLER];
    }
    // 광고 로딩
    else {
        NSLog(@"The rewarded video wasn't loaded yet.");
        [self loadRewardedVideo];
    }
}

- (float) getBannerWidth {
    return bannerView.frame.size.width;
}

- (float) getBannerHeight {
    return bannerView.frame.size.height;
}

@end

#endif // SB_PLUGIN_USE_ADS
