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
    initBanner();
    initGameView();
    initMenu();
    initGameListener();
    
    // FIXME: 다시 하기 버튼
    {
        auto btn = SBNodeUtils::createTouchNode();
        btn->setAnchorPoint(ANCHOR_MT);
        btn->setPosition(Vec2TC(0, 0));
        btn->setContentSize(Size(SB_WIN_SIZE.width*0.6f, 100));
        addChild(btn, INT_MAX);
        
        btn->addClickEventListener([=](Ref*) {
            GameManager::onStageChanged();
        });
    }
    
    return true;
}

void GameScene::onEnter() {
    
    BaseScene::onEnter();
    
    GameManager::onGameEnter();
    GAME_MANAGER->setStage(User::getClearStage()+1);
}

void GameScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    GameManager::onGameStart();
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
        case Tag::BTN_PAUSE: {
            // GameManager::onGamePause();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
}

/**
 * 배너 초기화
 */
void GameScene::initBanner() {
    
    // 배너
    if( !User::isRemovedAds() ) {
//        auto bannerView = BannerView::create();
//        bannerView->setTag(Tag::BANNER);
//        addChild(bannerView, SBZOrder::TOP);
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
}

/**
 * 게임 이벤트 리스너 초기화
 */
void GameScene::initGameListener() {
    
    string eventNames[] = {
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_CLEAR,
    };
    
    for( string eventName : eventNames ) {
        auto listener = EventListenerCustom::create(eventName, [=](EventCustom *event) {
            
            if( eventName == GAME_EVENT_STAGE_CHANGED ) {
                auto stage = (StageData*)event->getUserData();
                this->onStageChanged(*stage);
            }
            else if( eventName == GAME_EVENT_STAGE_CLEAR ) {
                auto stage = (StageData*)event->getUserData();
                this->onStageClear(*stage);
            }
        });
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
}
