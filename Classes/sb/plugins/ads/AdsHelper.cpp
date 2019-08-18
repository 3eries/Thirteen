//
//  AdsManager.cpp
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#include "AdsHelper.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

static const string AUTO_LOAD_SCHEDULER = "AUTO_LOAD_SCHEDULER";

static AdsHelper *instance = nullptr;
AdsHelper* AdsHelper::getInstance() {
    
    if( !instance ) {
        instance = new AdsHelper();
    }
    
    return instance;
}

void AdsHelper::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

AdsHelper::AdsHelper() :
isInitialized(false),
eventDispatcher(nullptr),
autoLoadInterval(0),
adOpened(false),
banner(Ad(AdType::BANNER)),
interstitial(Ad(AdType::INTERSTITIAL)),
rewardedVideo(Ad(AdType::REWARDED_VIDEO)) {
}

AdsHelper::~AdsHelper() {
    
    Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
    
    CC_SAFE_RELEASE(banner.listener);
    CC_SAFE_RELEASE(interstitial.listener);
    CC_SAFE_RELEASE(rewardedVideo.listener);
    CC_SAFE_DELETE(eventDispatcher);
}

/**
 * 광고 모듈 초기화
 */
void AdsHelper::init(const AdsConfig &config) {
    
#if SB_PLUGIN_USE_ADS
    if( isInitialized ) {
        return;
    }
    
    isInitialized = true;
    this->config = config;
    
    initListeners();
    initImpl(config);
    
    setAutoLoad(config.autoLoadInterval);
#endif
}

void AdsHelper::initListeners() {
    
#if SB_PLUGIN_USE_ADS
    eventDispatcher = new AdsEventDispatcher();
    
    // Banner
    banner.listener = AdListener::create(AdType::BANNER);
    banner.listener->retain();
    banner.listener->onAdLoaded = [=]() {
        CCLOG("Banner onAdLoaded");
        this->eventDispatcher->dispatchLoadedEvent(AdType::BANNER);
    };
    banner.listener->onAdFailedToLoad = [=](int errorCode) {
        CCLOG("Banner onAdFailedToLoad errorCode: %d", errorCode);
        this->eventDispatcher->dispatchFailedToLoadEvent(AdType::BANNER, errorCode);
    };
    banner.listener->onAdOpened = [=]() {
        CCLOG("Banner onAdOpened");
        
        this->adOpened = true;
        this->banner.listener->setOpened(true);
        
        this->eventDispatcher->dispatchOpenedEvent(AdType::BANNER);
    };
    banner.listener->onAdClosed = [=]() {
        CCLOG("Banner onAdClosed");
        
        this->adOpened = false;
        this->banner.listener->setOpened(false);
        
        this->eventDispatcher->dispatchClosedEvent(AdType::BANNER);
    };
    
    // Interstitial
    interstitial.listener = AdListener::create(AdType::INTERSTITIAL);
    interstitial.listener->retain();
    interstitial.listener->onAdLoaded = [=]() {
        CCLOG("Interstitial onAdLoaded");
        this->eventDispatcher->dispatchLoadedEvent(AdType::INTERSTITIAL);
    };
    interstitial.listener->onAdFailedToLoad = [=](int errorCode) {
        CCLOG("Interstitial onAdFailedToLoad errorCode: %d", errorCode);
        this->eventDispatcher->dispatchFailedToLoadEvent(AdType::INTERSTITIAL, errorCode);
    };
    interstitial.listener->onAdOpened = [=]() {
        CCLOG("Interstitial onAdOpened");
        
        this->adOpened = true;
        this->interstitial.listener->setOpened(true);
        
        this->eventDispatcher->dispatchOpenedEvent(AdType::INTERSTITIAL);
    };
    interstitial.listener->onAdClosed = [=]() {
        CCLOG("Interstitial onAdClosed");
        
        this->adOpened = false;
        this->interstitial.listener->setOpened(false);
        
        this->eventDispatcher->dispatchClosedEvent(AdType::INTERSTITIAL);
    };
    
    // Rewarded Video
    auto rewardedVideoAdListener = RewardedVideoAdListener::create();
    rewardedVideoAdListener->retain();
    rewardedVideoAdListener->onAdLoaded = [=]() {
        CCLOG("RewardedVideo onAdLoaded");
        this->eventDispatcher->dispatchLoadedEvent(AdType::REWARDED_VIDEO);
    };
    rewardedVideoAdListener->onAdFailedToLoad = [=](int errorCode) {
        CCLOG("RewardedVideo onAdFailedToLoad errorCode: %d", errorCode);
        this->eventDispatcher->dispatchFailedToLoadEvent(AdType::REWARDED_VIDEO, errorCode);
    };
    rewardedVideoAdListener->onAdOpened = [=]() {
        CCLOG("RewardedVideo onAdOpened");
        
        this->adOpened = true;
        this->rewardedVideo.listener->setOpened(true);
        
        this->eventDispatcher->dispatchOpenedEvent(AdType::REWARDED_VIDEO);
    };
    rewardedVideoAdListener->onAdClosed = [=]() {
        CCLOG("RewardedVideo onAdClosed");
        
        this->adOpened = false;
        this->rewardedVideo.listener->setOpened(false);
        
        this->eventDispatcher->dispatchClosedEvent(AdType::REWARDED_VIDEO);
    };
    rewardedVideoAdListener->onRewarded = [=](string type, int rewardAmount) {
        CCLOG("RewardedVideo onRewarded type: %s amount: %d", type.c_str(), rewardAmount);
        this->eventDispatcher->dispatchRewardedEvent(type, rewardAmount);
    };
    
    rewardedVideo.listener = rewardedVideoAdListener;
#endif
}

/**
 * 광고 활성화 설정
 */
void AdsHelper::setActiveBanner(bool active) {
    banner.active = active;
}

void AdsHelper::setActiveInterstitial(bool active) {
    interstitial.active = active;
}

void AdsHelper::setActiveRewardedVideo(bool active) {
    rewardedVideo.active = active;
}

/**
 * 광고 자동 로드 스케줄러 실행
 */
void AdsHelper::setAutoLoad(float interval) {
    
#if SB_PLUGIN_USE_ADS
    if( autoLoadInterval == interval ) {
        return;
    }
    
    autoLoadInterval = interval;
    
    Director::getInstance()->getScheduler()->unschedule(AUTO_LOAD_SCHEDULER, this);
    
    if( interval == 0 ) {
        return;
    }
    
    Director::getInstance()->getScheduler()->schedule([=](float dt) {
        
        if( !isInitialized ) {
            return;
        }
        
        // 배너 광고 로드
        if( banner.active && !this->isBannerLoaded() ) {
            this->loadBanner();
        }
        
        // 전면 광고 로드
        if( interstitial.active && !interstitial.listener->isOpened() &&
           !this->isInterstitialLoaded() ) {
            this->loadInterstitial();
        }
        
        // 보상형 비디오 광고 로드
        if( rewardedVideo.active && !rewardedVideo.listener->isOpened() &&
           !this->isRewardedVideoLoaded() ) {
            this->loadRewardedVideo();
        }
        
    }, this, interval, false, AUTO_LOAD_SCHEDULER);
#endif
}

NS_SB_END;

