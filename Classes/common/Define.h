//
//  Define.h
//  Define
//
//  Created by seongmin hwang
//

#ifndef Define_h
#define Define_h

#include "cocos2d.h"
#include "superbomb.h"

static const std::string DEV_VERSION = "v0.0.0.1";

// Common Functions
void removeListeners(cocos2d::Ref *target);

// Config File
static const std::string SERVER_VERSION_FILE                    = "http://superbomb.co/3eries/rsp/version.json";
static const std::string SERVER_GAME_CONFIG_FILE                = "http://superbomb.co/3eries/rsp/game_config.json";

#define                  AES256_KEY                               "A1din7aah98dh9wef974fnsudh54ugJs"

static const std::string GAME_CONFIG_FILE                       = "game_config.json";

static const std::string INTERNAL_GAME_CONFIG_FILE              = "config/" + GAME_CONFIG_FILE;
#define                  EXTERNAL_GAME_CONFIG_FILE                 std::string(cocos2d::FileUtils::getInstance()->getWritablePath() + GAME_CONFIG_FILE)

static const std::string INTERNAL_GAME_CONFIG_FILE_VERSION      = "1.0.0";

#define                  ENCRYPT_GAME_CONFIG_FILE                 0
#define                  ENCRYPT_PLUGIN_CONFIG_FILE               0

// Plugin
static const std::string PLUGIN_CONFIG_FILE                     = "config/plugin_config.json";

// Analytics
static const std::string ANALYTICS_SCREEN_SPLASH                    = "splash";
static const std::string ANALYTICS_SCREEN_MAIN                      = "main";
static const std::string ANALYTICS_SCREEN_GAME                      = "game";

static const std::string ANALYTICS_EVENT_CREDIT                     = "credit";                     // 크레딧 화면 진입
static const std::string ANALYTICS_EVENT_MORE_GAMES                 = "more_games";                 // 게임 더보기 클릭

static const std::string ANALYTICS_EVENT_NEW_PUZZLE_CLEAR           = "new_puzzle_clear";
static const std::string ANALYTICS_EVENT_ALL_NORMAL_PUZZLE_CLEAR    = "all_normal_puzzle_clear";
static const std::string ANALYTICS_EVENT_ALL_PUZZLE_CLEAR           = "all_puzzle_clear";
static const std::string ANALYTICS_EVENT_PUZZLE_SELECT              = "puzzle_select";
static const std::string ANALYTICS_EVENT_PUZZLE_VIEW_ADS            = "puzzle_view_ads";
static const std::string ANALYTICS_EVENT_PUZZLE_PLAY                = "puzzle_play";                // 퍼즐 시작
static const std::string ANALYTICS_EVENT_PUZZLE_RETRY               = "puzzle_retry";               // 퍼즐 다시하기
static const std::string ANALYTICS_EVENT_PUZZLE_GIVE_UP             = "puzzle_give_up";             // 퍼즐 포기
static const std::string ANALYTICS_EVENT_PUZZLE_CLEAR               = "puzzle_clear";               // 퍼즐 클리어
static const std::string ANALYTICS_EVENT_PUZZLE_THUMBNAIL           = "puzzle_thumbnail";           // 인게임 -> 퍼즐 썸네일 클릭

static const std::string ANALYTICS_EVENT_REVIEW_POPUP               = "review_popup";               // 리뷰 팝업 진입
static const std::string ANALYTICS_EVENT_ITEM_BOX_CLICK             = "item_box_click";             // 아이템 상자 클릭
static const std::string ANALYTICS_EVENT_ITEM_USE                   = "item_use";                   // 아이템 사용

static const std::string ANALYTICS_EVENT_PARAM_PUZZLE_ID            = "puzzle_id";                  // 퍼즐 아이디
static const std::string ANALYTICS_EVENT_PARAM_DIFFICULTY           = "difficulty";                 // 난이도
static const std::string ANALYTICS_EVENT_PARAM_SUMMARY              = "summary";                    // 요약
static const std::string ANALYTICS_EVENT_PARAM_ITEM_NAME            = "item_name";                  // 아이템 이름

// Leaderboard
// 현재는 아이디를 사용하지만, 추후 plugin_config.json에 이름과 아이디를 정의하여 사용
static const std::string LEADER_BOARD_HIGH_SCORE                = "CgkIhM_O4eseEAIQAQ";

// cocos2d::Director Event
static const std::string DIRECTOR_EVENT_REMOVE_ADS              = "DIRECTOR_EVENT_REMOVE_ADS";
static const std::string DIRECTOR_EVENT_UPDATE_HINT_COUNT       = "DIRECTOR_EVENT_UPDATE_HINT_COUNT";
static const std::string DIRECTOR_EVENT_UPDATE_USER_COIN        = "DIRECTOR_EVENT_UPDATE_USER_COIN";
static const std::string DIRECTOR_EVENT_UNLOCK_ALL_PUZZLES      = "DIRECTOR_EVENT_UNLOCK_ALL_PUZZLES";

// 리소스 경로
static const std::string DIR_IMG                    = "images/";
static const std::string DIR_IMG_COMMON             = DIR_ADD(DIR_IMG, "common");
static const std::string DIR_IMG_SPLASH             = DIR_ADD(DIR_IMG, "splash");
static const std::string DIR_IMG_MAIN               = DIR_ADD(DIR_IMG, "main");
static const std::string DIR_IMG_MAIN_IPAD          = DIR_ADD(DIR_IMG_MAIN, "ipad");
static const std::string DIR_IMG_GAME               = DIR_ADD(DIR_IMG, "game");
static const std::string DIR_IMG_GAME_IPAD          = DIR_ADD(DIR_IMG_GAME, "ipad");
static const std::string DIR_IMG_EXIT               = DIR_ADD(DIR_IMG, "exit");
static const std::string DIR_IMG_SETTING            = DIR_ADD(DIR_IMG, "setting");
static const std::string DIR_IMG_PAUSE              = DIR_ADD(DIR_IMG, "pause");
static const std::string DIR_IMG_UPDATE             = DIR_ADD(DIR_IMG, "update");

static const std::string DIR_CONTENT                = "content/";
static const std::string DIR_CONTENT_DATA           = DIR_ADD(DIR_CONTENT, "data");
static const std::string DIR_CONTENT_WORLD_MAP      = DIR_ADD(DIR_CONTENT, "world_map");
static const std::string DIR_CONTENT_PUZZLE         = DIR_ADD(DIR_CONTENT, "puzzle");

// 스파인 애니메이션
static const std::string ANIM_EXT                   = ".json";
static const std::string ANIM_LOGO                  = DIR_IMG_SPLASH + "3eries_logo.json";
static const std::string ANIM_CLOUD                 = DIR_IMG_GAME + "RSP_cloud.json";
static const std::string ANIM_CONTINUE              = DIR_IMG_GAME + "continue.json";

static const std::string ANIM_NAME_RUN              = "run";
static const std::string ANIM_NAME_CLEAR            = "clear";
static const std::string ANIM_NAME_ENTER            = "enter";
static const std::string ANIM_NAME_EXIT             = "exit";

// 폰트 경로
static const std::string DIR_FONT                   = "fonts/";

static const std::string FONT_COMMODORE             = DIR_FONT + "Commodore.ttf";
static const std::string FONT_SABO                  = DIR_FONT + "Sabo-Filled.otf";
static const std::string FONT_MANIA                 = DIR_FONT + "mania.ttf";
static const std::string FONT_GAME_OVER             = DIR_FONT + "game_over.ttf";
static const std::string FONT_RETRO                 = DIR_FONT + "Commodore.ttf";
static const std::string FONT_NEW_RECORD            = DIR_FONT + "Sabo-Filled.otf";
static const std::string FONT_FAMOUS_SAYING         = DIR_FONT + "game_over.ttf";
static const std::string FONT_ROBOTO_BLACK          = DIR_FONT + "Roboto-Black.ttf";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_01.mp3";
static const std::string SOUND_BGM_GAME             = DIR_SOUND + "bgm_03.mp3";
static const std::string SOUND_BGM_COMING_SOON      = DIR_SOUND + "bgm_03.mp3";

static const std::string SOUND_BUTTON_CLICK         = DIR_SOUND + "effect_01.mp3";
static const std::string SOUND_TILE_DROP            = DIR_SOUND + "effect_08.mp3";
static const std::string SOUND_PUZZLE_CLEAR_STAMP   = DIR_SOUND + "effect_09.mp3";
static const std::string SOUND_PUZZLE_CLEAR         = DIR_SOUND + "effect_10.mp3";
static const std::string SOUND_THANK_YOU_PURCHASED  = DIR_SOUND + "effect_11.mp3";

// 좌표 및 크기
#define BANNER_HEIGHT           superbomb::AdsHelper::getBannerHeight()

// Color
namespace Color {
    static const cocos2d::Color4B POPUP_BG          = cocos2d::Color4B(0,0,0,255*0.5f);
}

// ZOrder
namespace ZOrder {
    static const int SCENE_TOUCH_LOCKED        = SBZOrder::MIDDLE-1;
    
    static const int POPUP_BOTTOM              = SBZOrder::MIDDLE;
    static const int POPUP_MIDDLE              = POPUP_BOTTOM + 10;
    static const int POPUP_TOP                 = POPUP_MIDDLE + 10;
    
    static const int COMMON_MENU_TOP           = POPUP_TOP;
    static const int COMMON_MENU_BOTTOM        = POPUP_MIDDLE;
}

// 연출 시간
namespace EffectDuration {
    static const float POPUP_SLIDE_SLOW        = 0.4f;
    static const float POPUP_SLIDE_NORMAL      = 0.3f;
    static const float POPUP_SLIDE_FAST        = 0.25f;
    
    static const float MENU_SLIDE_NORMAL       = 0.3f;
    static const float MENU_SLIDE_FAST         = 0.25f;
}

// UI
namespace ButtonZoomScale {
    static const float WEAK                    = -0.03f;
    static const float NORMAL                  = -0.05f;
    static const float HARD                    = -0.07f;
}

#endif /* Define_h */
