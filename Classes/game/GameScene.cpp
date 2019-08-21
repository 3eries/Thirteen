//
//  GameScene.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameScene.hpp"
#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "BannerView.hpp"

#include "GameManager.hpp"
#include "GameView.hpp"

#include "CommonLoadingBar.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

GameScene* GameScene::create() {
    
    auto scene = new GameScene();
    
    if( scene && scene->init() ) {
        scene->autorelease();
        return scene;
    }
    
    delete scene;
    return nullptr;
}

GameScene::GameScene() :
gameView(nullptr) {
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    SBAnalytics::setCurrentScreen(ANALYTICS_SCREEN_GAME);
    
    initBg();
    initGameView();
    initMenu();
    initGameListener();
    
    // 개발 버전 표기
    auto versionLabel = Label::createWithTTF(DEV_VERSION, FONT_ROBOTO_BLACK, 30, Size::ZERO,
                                             TextHAlignment::RIGHT, TextVAlignment::BOTTOM);
    versionLabel->setTextColor(Color4B::WHITE);
    versionLabel->setAnchorPoint(ANCHOR_BR);
    versionLabel->setPosition(Vec2BR(0,120));
    addChild(versionLabel, INT_MAX);
    
    return true;
}

void GameScene::onEnter() {
    
    BaseScene::onEnter();
    
    GameManager::onGameEnter();
    // GAME_MANAGER->setStage(User::getClearStage()+1);
    GAME_MANAGER->setStage(1);
    GameManager::onGameStart();
}

void GameScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
}

void GameScene::onExit() {
    
    BaseScene::onExit();
}

void GameScene::cleanup() {
    
    BaseScene::cleanup();
}

bool GameScene::onApplicationEnterBackground() {
    
    if( !BaseScene::onApplicationEnterBackground() ) {
        return false;
    }
    
    GameManager::onGamePause();
    
    return true;
}

bool GameScene::onApplicationEnterForeground() {
    
    if( !BaseScene::onApplicationEnterForeground() ) {
        return false;
    }
    
    GameManager::onGameResume();
    
    return true;
}

bool GameScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    return true;
}

/**
 * 스테이지 변경
 */
void GameScene::onStageChanged(const StageData &stage) {
}

/**
 * 스테이지 클리어
 */
void GameScene::onStageClear(const StageData &stage) {

    // 다음 스테이지 버튼 노출
    auto nextBtn = SBButton::create(DIR_IMG_GAME + "scratch_btn_next.png");
    nextBtn->setZoomScale(ButtonZoomScale::WEAK);
    // nextBtn->setZoomScale(0);
    nextBtn->setAnchorPoint(ANCHOR_M);
    nextBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        nextBtn->removeFromParent();

        // 마지막 스테이지 클리어한 경우, 1스테이지로 복귀
        if( stage.stage == Database::getLastStage().stage ) {
            GAME_MANAGER->setStage(User::getClearStage()+1);
            GameManager::onStageChanged();
        } else {
            GameManager::onMoveNextStage();
            GameManager::onMoveNextStageFinished();
        }
    });
    addChild(nextBtn);
    
    // 연출 진행되는 동안 버튼 터치 방지
    nextBtn->setTouchEnabled(false);
    
    // 화면 아래에서 올라오는 연출
    nextBtn->setPosition(Vec2BC(0, -nextBtn->getContentSize().height*0.5f));
    
    auto moveIn = MoveTo::create(0.2f, Vec2BC(0, 205));
    auto callFunc = CallFunc::create([=]() {
        nextBtn->setTouchEnabled(true);
    });
    nextBtn->runAction(Sequence::create(moveIn, callFunc, nullptr));
}

/**
 * Scene 전환
 */
void GameScene::replaceScene(SceneType type) {
    
    GameManager::onGameExit();
    GameManager::destroyInstance();
    removeListeners(this);
    
    BaseScene::replaceScene(type);
}

/**
 * 리뷰 체크
 * 조건 충족 시 리뷰 작성 알림 팝업 노출
 */
void GameScene::checkReview() {
    
    if( /*리뷰 노출 조건*/ false ) {
        User::checkReview(0.5f);
    }
}

/**
 * 버튼 클릭
 */
void GameScene::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_SETTING: {
            // GameManager::onGamePause();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
    addChild(LayerColor::create(Color4B(GAME_BG_COLOR)));
    
    // 배너
    if( !User::isRemovedAds() ) {
        auto bannerView = BannerView::create();
        bannerView->setTag(Tag::BANNER);
        addChild(bannerView, SBZOrder::TOP);
    }
}

/**
 * 게임뷰 초기화
 */
void GameScene::initGameView() {
    
    gameView = GameView::create();
    addChild(gameView);
    
    // GAME_MANAGER->setView(gameView);
}

/**
 * 메뉴 초기화
 */
void GameScene::initMenu() {
    
    // 설정 버튼
    // common_btn_more.png Vec2TR(-56, -54) , Size(74, 74)
    auto settingBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_more.png");
    settingBtn->setTag(Tag::BTN_SETTING);
    settingBtn->setZoomScale(ButtonZoomScale::NORMAL);
    settingBtn->setAnchorPoint(ANCHOR_M);
    settingBtn->setPosition(Vec2TR(-56, -54));
    addChild(settingBtn);
    
    settingBtn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
}

/**
 * 게임 이벤트 리스너 초기화
 */
void GameScene::initGameListener() {

    StringList events({
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_CLEAR,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::STAGE_CHANGED: {
                auto stage = (StageData*)userData;
                this->onStageChanged(*stage);
            } break;
                
            case GameEvent::STAGE_CLEAR: {
                auto stage = (StageData*)userData;
                this->onStageClear(*stage);
            } break;
                
            default: break;
        }
    }, this);
}
