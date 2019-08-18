//
//  MainScene.cpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "GameUIHelper.hpp"

#include "../test/TestHelper.hpp"
#include "../game/GameManager.hpp"

#include "CommonLoadingBar.hpp"
#include "ExitAlertPopup.hpp"
#include "SettingPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
}

bool MainScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    SBAnalytics::setCurrentScreen(ANALYTICS_SCREEN_MAIN);
    
    initBg();
    initMenu();
    
    initIAPListener();
    initPopupListener();
    
    // 개발 버전 표기
    auto versionLabel = Label::createWithTTF(DEV_VERSION, FONT_ROBOTO_BLACK, 30, Size::ZERO,
                                             TextHAlignment::RIGHT, TextVAlignment::BOTTOM);
    versionLabel->setTextColor(Color4B::WHITE);
    versionLabel->setAnchorPoint(ANCHOR_BR);
    versionLabel->setPosition(Vec2BR(0,0));
    addChild(versionLabel, INT_MAX);
    
    return true;
}

void MainScene::onEnter() {
    
    BaseScene::onEnter();
    
    // bgm
    scheduleOnce([=](float) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
    }
}

void MainScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        ConsentForm::create()
        ->setTitle("Important")
        ->setMessage("We don't collect your personal data,\nhowever our ads suppliers may use\nthem to personalise ads for you. You'll\nfind links to our partner's privacy\npolicies in our Privacy Policy.\n\nYou can opt-out ads tracking from\ngame & device settings.")
        ->setPolicyUrl("http://www.3eries.com/privacy_policy")
        ->show();
    }
}

void MainScene::onExit() {
    
    BaseScene::onExit();
}

bool MainScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    // 앱 종료 알림 팝업 생성
    if( PopupManager::getInstance()->getPopupCount() == 0 ) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        auto popup = (ExitAlertPopup*)PopupManager::show(PopupType::EXIT_APP);
        popup->setOnExitAppListener([=]() {
            SBSystemUtils::exitApp();
        });
        
        return true;
    }
    
    return false;
}

/**
 * 버튼 클릭
 */
void MainScene::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 크레딧
        case Tag::BTN_TITLE: {        
            SBAnalytics::logEvent(ANALYTICS_EVENT_CREDIT);
            
            auto popup = PopupManager::show(PopupType::CREDIT);
            popup->setLocalZOrder(ZOrder::POPUP_TOP);
            popup->setOnDismissListener([=](Node*) {
            });
        } break;
            
        // 게임 시작
        case Tag::BTN_START: {
            GAME_MANAGER->init();
            replaceScene(SceneType::GAME);
        } break;
            
        // 설정
        case Tag::BTN_SETTING: {
            showSettingPopup();
        } break;
        
        // 광고 제거 아이템
        case Tag::BTN_REMOVE_ADS: {
            if( iap::IAPHelper::isReady() ) {
                auto loadingBar = CommonLoadingBar::create();
                loadingBar->setUIDelay(0.1f);
                loadingBar->show();
                
                auto listener = iap::PurchaseListener::create();
                listener->setTarget(this);
                listener->onPurchased = [=](const iap::Item &item) {
                };
                listener->onFinished = [=](bool result) {
                    loadingBar->dismissWithDelay(0);
                };

                iap::IAPHelper::purchaseRemoveAds(listener);
            }
        } break;
            
        // test
        case Tag::BTN_TEST: {
        } break;
            
        default:
            break;
    }
}

/**
 * 설정 팝업 노출
 */
void MainScene::showSettingPopup() {
    
    auto popup = SettingPopup::create();
    popup->setOnClickMenuListener([=](SettingPopup::Tag tag) {
        
        switch( tag ) {
            // leader board
            case SettingPopup::Tag::LEADER_BOARD: {
                if( superbomb::PluginPlay::isSignedIn() ) {
                    superbomb::PluginPlay::showAllLeaderboards();
                } else {
                    superbomb::PluginPlay::signIn();
                }
                // popup->dismissWithAction();
            } break;
                
            // restore purchase
            case SettingPopup::Tag::RESTORE_PURCHASE: {
                if( iap::IAPHelper::isReady() ) {
                    iap::IAPHelper::restore(nullptr);
                }
            } break;
                
            // remove ads
            case SettingPopup::Tag::REMOVE_ADS: {
                if( iap::IAPHelper::isReady() ) {
                    auto loadingBar = CommonLoadingBar::create();
                    loadingBar->setUIDelay(0.1f);
                    loadingBar->show();
                    
                    auto listener = iap::PurchaseListener::create();
                    listener->setTarget(this);
                    listener->onPurchased = [=](const iap::Item &item) {
                    };
                    listener->onFinished = [=](bool result) {
                        loadingBar->dismissWithDelay(0);
                    };
                    
                    iap::IAPHelper::purchaseRemoveAds(listener);
                }
            } break;
                
            // how to play
            case SettingPopup::Tag::HOW_TO_PLAY: {
            } break;
                
            default: break;
        }
    });
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

void MainScene::initBg() {
    
    addChild(LayerColor::create(Color4B::BLACK));
    
    auto title = SBButton::create(DIR_IMG_MAIN + "main_title.png");
    title->setZoomScale(0);
    title->setTag(Tag::BTN_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 250));
    addChild(title, SBZOrder::BOTTOM);
    
    title->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {
    
    // 탭하여 시작
    // auto tapToStart = Sprite::create(DIR_IMG_MAIN + "scratch_taptostart.png");
    auto tapToStart = Label::createWithTTF("TAP TO START", FONT_ROBOTO_BLACK, 50, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    tapToStart->setTextColor(Color4B::WHITE);
    tapToStart->setAnchorPoint(ANCHOR_M);
    tapToStart->setPosition(Vec2BC(0, 250));
    addChild(tapToStart);
    
    auto btn = SBNodeUtils::createTouchNode();
    btn->setTag(Tag::BTN_START);
    addChild(btn);
    
    btn->addClickEventListener([=](Ref*) {
        this->onClick(btn);
    });
    
    // blink
    {
        auto delay = DelayTime::create(0.5f);
        auto callFunc = CallFunc::create([=]() {
            auto blink = RepeatForever::create(Blink::create(1.0f, 1));
            tapToStart->runAction(blink);
        });
        tapToStart->runAction(Sequence::create(delay, callFunc, nullptr));
    }
    
    // 메인 화면 전용 메뉴
    /*
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_START,        ANCHOR_M,    Vec2BC(0, 100),    DIR_IMG_MAIN + "RSP_btn_start.png"),
        SBUIInfo(Tag::BTN_SETTING,      ANCHOR_MR,   Vec2TR(-66 + (100*0.5f), -62),     DIR_IMG_COMMON + "RSP_btn_option.png"),
        SBUIInfo(Tag::BTN_REMOVE_ADS,   ANCHOR_M,    Vec2MC(0, 25),     DIR_IMG_COMMON + "RSP_btn_remove_ads.png"),
#if ENABLE_TEST_MENU
        SBUIInfo(Tag::BTN_TEST,         ANCHOR_TL,   Vec2TL(10, -10),   DIR_IMG_COMMON + "RSP_btn_test.png"),
#endif
    };
    
    if( SBDirector::isPadResolution() ) {
        infos[0].file = SBStringUtils::replaceAll(infos[0].file, DIR_IMG_MAIN, DIR_IMG_MAIN_IPAD);
    }
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(info.file);
        btn->setZoomScale(ButtonZoomScale::NORMAL);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
    }
    
    if( User::isRemovedAds() ) {
        getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
    }
    */
}

/**
 * 인앱 결제 리스너 초기화
 */
void MainScene::initIAPListener() {
    
    auto onRemoveAds = [=]() {
        
        this->getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
    };
    
    // purchase listener
    auto purchaseListener = iap::PurchaseListener::create();
    purchaseListener->setForever(true);
    purchaseListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
    
    // restore listener
    auto restoreListener = iap::RestoreListener::create();
    restoreListener->setForever(true);
    restoreListener->onRemoveAds = onRemoveAds;
    
    iap::IAPHelper::getInstance()->addListener(this, restoreListener);
}

/**
 * 팝업 리스너 초기화
 */
void MainScene::initPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType type) {
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        // 팝업 연출에 따른 메인화면 메뉴 처리
        // TODO:
    };
    
    PopupManager::getInstance()->addListener(listener);
}
