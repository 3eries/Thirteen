package com.superbomb.plugins.ads;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;

import com.google.ads.mediation.admob.AdMobAdapter;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;
import com.superbomb.math.Size;
import com.superbomb.plugins.PluginListener;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;

public class AdsManager implements PluginListener {
    private static AdsManager instance = null;

    public static AdsManager getInstance() {
        if( instance == null ) {
            instance = new AdsManager();
        }

        return instance;
    }

    private static final String TAG = "AdsManager";

    private static final AdSize BANNER_SIZE = new AdSize(AdSize.FULL_WIDTH, 50);

    private Activity context;

    private AdView bannerView;
    private Size bannerSizeInPixels;

    private InterstitialAd interstitialAd;
    private RewardedVideoAd rewardedVideoAd;

    private String rewardedVideoUnitId = "";
    private final ArrayList<String> testDevices;

    private AdsManager() {
        testDevices = new ArrayList<>();
    }

    @Override
    public void init(Cocos2dxActivity context) {
        this.context = context;
    }

    @Override
    public void onStart() {
    }

    @Override
    public void onStop() {
    }

    @Override
    public void onResume() {
        if( bannerView != null ) bannerView.resume();
        if( rewardedVideoAd != null ) rewardedVideoAd.resume(context);
    }

    @Override
    public void onPause() {
        if( bannerView != null ) bannerView.pause();
        if( rewardedVideoAd != null ) rewardedVideoAd.pause(context);
    }

    @Override
    public void onDestroy() {
        if( bannerView != null ) bannerView.destroy();
        if( rewardedVideoAd != null ) rewardedVideoAd.destroy(context);
    }

    @Override
    public boolean onBackPressed() {
        return false;
    }

    @Override
    public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        return false;
    }

    /**
     * AdRequest.Builder 생성
     */
    private static AdRequest.Builder createRequestBuilder() {

        return new AdRequest.Builder()
                //                .addTestDevice("0A16B70F54452DD393143983A218FB9A")
                //                .addTestDevice("81E690704D782651A631FAB69551FE0C")
                .addNetworkExtrasBundle(AdMobAdapter.class, new Bundle());
    }

    /**
     * 광고 모듈 초기화
     */
    public static void initAd(final String appId,
                              final String bannerUnitId,
                              final String interstitialUnitId,
                              final String rewardedVideoUnitId) {

        Log.i(TAG, "initAd appId: " + appId);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final AdsManager mgr = instance;
                final Activity ctx = mgr.context;

                MobileAds.initialize(ctx, appId);

                mgr.initBanner(bannerUnitId);
                mgr.initInterstitial(interstitialUnitId);
                mgr.initRewardedVideo(rewardedVideoUnitId);

                // TODO: 어댑터 초기화
            }
        });
    }

    /**
     * Vungle 어댑터 초기화
     */
    //    public static void initVungleAdapter(String placement1, String placement2) {
    //
    //        String[] placements = new String[2];
    //        placements[0] = placement1;
    //        placements[1] = placement2;
    //        instance.vungleExtras = new VungleExtrasBuilder(placements).build();
    //    }

    /**
     * 테스트 기기 초기화
     */
    public static void initTestDevices(String devices[]) {

        for( String device : devices ) {
            getInstance().testDevices.add(device);
        }
    }

    /**
     * 배너 광고 초기화
     */
    private void initBanner(String unitId) {

        Log.i(TAG, "initBanner unitId: " + unitId);

        if( TextUtils.isEmpty(unitId) ) {
            return;
        }

        bannerView = new AdView(context);
        bannerView.setAdUnitId(unitId);
        bannerView.setAdSize(BANNER_SIZE);
        bannerView.setVisibility(View.GONE);

        bannerSizeInPixels = new Size();
        bannerSizeInPixels.setWidth(BANNER_SIZE.getWidthInPixels(context));
        bannerSizeInPixels.setHeight(BANNER_SIZE.getHeightInPixels(context));

        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        layoutParams.gravity = (Gravity.CENTER | Gravity.BOTTOM);

        context.addContentView(bannerView, layoutParams);

        bannerView.setAdListener(new AdListener() {

            @Override
            public void onAdLoaded() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeBannerOnAdLoaded();
                    }
                });
            }

            @Override
            public void onAdFailedToLoad(final int errorCode) {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeBannerOnAdFailedToLoad(errorCode);
                    }
                });
            }

            @Override
            public void onAdOpened() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeBannerOnAdOpened();
                    }
                });
            }

            @Override
            public void onAdClosed() {
                Log.i(TAG, "banner onAdClosed");

                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeBannerOnAdClosed();
                    }
                });

                loadBanner();
            }
        });

        loadBanner();
    }

    public static native void nativeBannerOnAdLoaded();

    public static native void nativeBannerOnAdFailedToLoad(int errorCode);

    public static native void nativeBannerOnAdOpened();

    public static native void nativeBannerOnAdClosed();

    /**
     * 전면 광고 초기화
     */
    private void initInterstitial(String unitId) {

        Log.i(TAG, "initInterstitial unitId: " + unitId);

        if( TextUtils.isEmpty(unitId) ) {
            return;
        }

        interstitialAd = new InterstitialAd(context);
        interstitialAd.setAdUnitId(unitId);
        interstitialAd.setAdListener(new AdListener() {

            @Override
            public void onAdLoaded() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeInterstitialOnAdLoaded();
                    }
                });
            }

            @Override
            public void onAdFailedToLoad(final int errorCode) {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeInterstitialOnAdFailedToLoad(errorCode);
                    }
                });
            }

            @Override
            public void onAdOpened() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeInterstitialOnAdOpened();
                    }
                });
            }

            @Override
            public void onAdClosed() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeInterstitialOnAdClosed();
                    }
                });

                loadInterstitial();
            }
        });

        loadInterstitial();
    }

    public static native void nativeInterstitialOnAdLoaded();

    public static native void nativeInterstitialOnAdFailedToLoad(int errorCode);

    public static native void nativeInterstitialOnAdOpened();

    public static native void nativeInterstitialOnAdClosed();

    /**
     * 보상형 비디오 초기화
     */
    private void initRewardedVideo(String unitId) {

        Log.i(TAG, "initRewardedVideo unitId: " + unitId);

        if( TextUtils.isEmpty(unitId) ) {
            return;
        }

        this.rewardedVideoUnitId = unitId;

        rewardedVideoAd = MobileAds.getRewardedVideoAdInstance(context);
        rewardedVideoAd.setRewardedVideoAdListener(new RewardedVideoAdListener() {
            @Override
            public void onRewardedVideoAdLoaded() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeRewardedVideoOnAdLoaded();
                    }
                });
            }

            @Override
            public void onRewardedVideoAdFailedToLoad(final int errorCode) {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeRewardedVideoOnAdFailedToLoad(errorCode);
                    }
                });
            }

            @Override
            public void onRewardedVideoAdOpened() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeRewardedVideoOnAdOpened();
                    }
                });
            }

            @Override
            public void onRewardedVideoAdClosed() {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeRewardedVideoOnAdClosed();
                    }
                });

                loadRewardedVideo();
            }

            @Override
            public void onRewarded(final RewardItem rewardItem) {
                Cocos2dxHelper.runOnGLThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeRewardedVideoOnRewarded(rewardItem.getType(), rewardItem.getAmount());
                    }
                });
            }

            @Override
            public void onRewardedVideoStarted() {
            }

            @Override
            public void onRewardedVideoAdLeftApplication() {
            }

            @Override
            public void onRewardedVideoCompleted() {
            }
        });

        loadRewardedVideo();
    }

    public static native void nativeRewardedVideoOnAdLoaded();

    public static native void nativeRewardedVideoOnAdFailedToLoad(int errorCode);

    public static native void nativeRewardedVideoOnAdOpened();

    public static native void nativeRewardedVideoOnAdClosed();

    public static native void nativeRewardedVideoOnRewarded(String type, int rewardAmount);

    /**
     * 배너 로드
     */
    public static boolean loadBanner() {

        final AdsManager mgr = instance;
        final AdView bannerView = mgr.bannerView;

        if( bannerView == null ) {
            Log.i(TAG, "banner load error: banner is null.");
            return false;
        }

        if( bannerView.isLoading() ) {
            Log.i(TAG, "banner is loading.");
            return false;
        }

        runOnUiThread(new Runnable() {

            @Override
            public void run() {

                Log.i(TAG, "banner load");

                AdRequest adRequest = createRequestBuilder().build();
                bannerView.loadAd(adRequest);
            }
        });

        return true;
    }

    /**
     * 전면 광고 로드
     */
    public static boolean loadInterstitial() {

        final AdsManager mgr = instance;
        final InterstitialAd interstitialAd = mgr.interstitialAd;

        if( interstitialAd == null ) {
            Log.i(TAG, "interstitialAd load error: interstitialAd is null.");
            return false;
        }

        if( interstitialAd.isLoading() ) {
            Log.i(TAG, "interstitialAd is loading.");
            return false;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                Log.i(TAG, "interstitialAd load");

                AdRequest adRequest = createRequestBuilder().build();
                interstitialAd.loadAd(adRequest);
            }
        });

        return true;
    }

    /**
     * 보상형 동영상 로드
     */
    public static boolean loadRewardedVideo() {

        final AdsManager mgr = instance;
        final RewardedVideoAd rewardedVideoAd = mgr.rewardedVideoAd;

        if( rewardedVideoAd == null ) {
            Log.i(TAG, "rewardedVideoAd load error: rewardedVideoAd is null.");
            return false;
        }

        if( TextUtils.isEmpty(mgr.rewardedVideoUnitId) ) {
            Log.i(TAG, "rewardedVideoAd load error: rewardedVideoUnitId is null.");
            return false;
        }

        Log.i(TAG, "rewardedVideoAd load");

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AdRequest adRequest = createRequestBuilder().build();
                rewardedVideoAd.loadAd(mgr.rewardedVideoUnitId, adRequest);
            }
        });

        return true;
    }

    /**
     * 배너 광고 노출
     */
    public static void showBanner() {

        runOnUiThread(new Runnable() {

            @Override
            public void run() {

                final AdView bannerView = instance.bannerView;

                if( bannerView != null ) {
                    bannerView.setVisibility(View.VISIBLE);
                }

                /*
                // 광고 로딩됨, 광고 노출
                if( mgr.isBannerLoaded ) {
                    bannerView.setVisibility(View.VISIBLE);
                }
                // 광고 로딩
                else {
                    Log.d(TAG, "The banner wasn't loaded yet.");
                    loadBanner();
                }
               */
            }
        });
    }

    /**
     * 배너 광고 숨김
     */
    public static void hideBanner() {

        runOnUiThread(new Runnable() {

            @Override
            public void run() {

                final AdView bannerView = instance.bannerView;

                if( bannerView != null ) {
                    bannerView.setVisibility(View.GONE);
                }
            }
        });
    }

    /**
     * 전면 광고 노출
     */
    public static void showInterstitial() {

        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                final AdsManager mgr = instance;
                final InterstitialAd interstitialAd = mgr.interstitialAd;

                if( interstitialAd == null ) {
                    return;
                }

                // 광고 로딩됨, 광고 노출
                if( interstitialAd.isLoaded() ) {
                    interstitialAd.show();
                }
                // 광고 로딩
                else {
                    Log.d(TAG, "The interstitial wasn't loaded yet.");
                    loadInterstitial();
                }
            }
        });
    }

    /**
     * 보상형 비디오 노출
     */
    public static void showRewardedVideo() {

        runOnUiThread(new Runnable() {

            @Override
            public void run() {

                final AdsManager mgr = instance;
                final RewardedVideoAd rewardedVideoAd = mgr.rewardedVideoAd;

                if( rewardedVideoAd == null ) {
                    return;
                }

                // 광고 로딩됨, 광고 노출
                if( rewardedVideoAd.isLoaded() ) {
                    rewardedVideoAd.show();
                }
                // 광고 로딩
                else {
                    Log.d(TAG, "The rewarded video wasn't loaded yet.");
                    loadRewardedVideo();
                }
            }
        });
    }

    public static float getBannerWidth() {
        return instance.bannerSizeInPixels.getWidth();
    }

    public static float getBannerHeight() {
        return instance.bannerSizeInPixels.getHeight();
    }

    public static void runOnUiThread(Runnable r) {
        instance.context.runOnUiThread(r);
    }
}
