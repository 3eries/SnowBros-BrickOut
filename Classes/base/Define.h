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

// Common Functions
void removeListeners(cocos2d::Ref *target);

// Config File
static const std::string SERVER_VERSION_FILE                    = "http://superbomb.co/3eries/rsp/version.json";
static const std::string SERVER_GAME_CONFIG_FILE                = "http://superbomb.co/3eries/rsp/game_config.json";

#define                  AES256_KEY                               "A1din7oih98dh9wef974fnsudh54ugJs"

static const std::string GAME_CONFIG_FILE                       = "game_config.json";

static const std::string INTERNAL_GAME_CONFIG_FILE              = "config/" + GAME_CONFIG_FILE;
#define                  EXTERNAL_GAME_CONFIG_FILE                 std::string(cocos2d::FileUtils::getInstance()->getWritablePath() + GAME_CONFIG_FILE)

static const std::string INTERNAL_GAME_CONFIG_FILE_VERSION      = "1.0.0";

#define                  ENCRYPT_GAME_CONFIG_FILE                 0
#define                  ENCRYPT_PLUGIN_CONFIG_FILE               0

// Plugin
static const std::string PLUGIN_CONFIG_FILE                     = "config/plugin_config.json";

// Firebase Analytics
static const std::string FA_SCREEN_SPLASH                       = "splash";
static const std::string FA_SCREEN_MAIN                         = "main";
static const std::string FA_SCREEN_GAME                         = "game";
static const std::string FA_SCREEN_GAME_RESULT                  = "game_result";

static const std::string FA_EVENT_GAME_PLAY                     = "game_play";                  // 게임 플레이 횟수
static const std::string FA_EVENT_GAME_RESULT                   = "game_result";                // 게임 결과
static const std::string FA_EVENT_CREDIT                        = "credit";                     // 크레딧 화면 진입
static const std::string FA_EVENT_SHOP                          = "shop";                       // 샵 화면 진입
static const std::string FA_EVENT_REVIEW_POPUP                  = "review_popup";               // 리뷰 팝업 진입

static const std::string FA_EVENT_ITEM_BOX_CLICK                = "item_box_click";             // 아이템 상자 클릭
static const std::string FA_EVENT_ITEM_USE                      = "item_use";                   // 아이템 사용

static const std::string FA_EVENT_PARAM_SCORE                   = "score";                      // 스코어
static const std::string FA_EVENT_PARAM_SCORE_RANGE             = "score_range";                // 스코어 범위
static const std::string FA_EVENT_PARAM_CONTINUE                = "continue";                   // 이어하기 여부 (0 or 1)
static const std::string FA_EVENT_PARAM_ITEM_NAME               = "item_name";                  // 아이템 이름

// Leaderboard
// 현재는 아이디를 사용하지만, 추후 plugin_config.json에 이름과 아이디를 정의하여 사용
static const std::string LEADER_BOARD_HIGH_SCORE                = "CgkIhM_O4eseEAIQAQ";

// cocos2d::Director Event
static const std::string DIRECTOR_EVENT_UPDATE_USER_COIN        = "DIRECTOR_EVENT_UPDATE_USER_COIN";
static const std::string DIRECTOR_EVENT_UPDATE_BALL_SKIN        = "DIRECTOR_EVENT_UPDATE_BALL_SKIN";
static const std::string DIRECTOR_EVENT_UPDATE_FRIENDS_DECK     = "DIRECTOR_EVENT_UPDATE_FRIENDS_DECK";

// 리소스 경로
static const std::string DIR_IMG                    = "images/";
static const std::string DIR_IMG_COMMON             = DIR_ADD(DIR_IMG, "common");
static const std::string DIR_IMG_SPLASH             = DIR_ADD(DIR_IMG, "splash");
static const std::string DIR_IMG_MAIN               = DIR_ADD(DIR_IMG, "main");
static const std::string DIR_IMG_MAIN_IPAD          = DIR_ADD(DIR_IMG_MAIN, "ipad");
static const std::string DIR_IMG_GAME               = DIR_ADD(DIR_IMG, "game");
static const std::string DIR_IMG_GAME_IPAD          = DIR_ADD(DIR_IMG_GAME, "ipad");
static const std::string DIR_IMG_BALL_SKIN          = DIR_ADD(DIR_IMG, "ball_skin");
static const std::string DIR_IMG_FRIEND             = DIR_ADD(DIR_IMG, "friend");
static const std::string DIR_IMG_SHOP               = DIR_ADD(DIR_IMG, "shop");
static const std::string DIR_IMG_STAGE_CLEAR        = DIR_ADD(DIR_IMG, "clear");

static const std::string DIR_CONTENT                = "content/";
static const std::string DIR_CONTENT_DATA           = DIR_ADD(DIR_CONTENT, "data");
static const std::string DIR_CONTENT_BALL           = DIR_ADD(DIR_CONTENT, "ball");
static const std::string DIR_CONTENT_BRICK          = DIR_ADD(DIR_CONTENT, "brick");
static const std::string DIR_CONTENT_FRIEND         = DIR_ADD(DIR_CONTENT, "friend");
static const std::string DIR_CONTENT_STAGE          = DIR_ADD(DIR_CONTENT, "stage");

// 스파인 애니메이션
static const std::string ANIM_EXT                   = ".json";
static const std::string ANIM_LOGO                  = DIR_IMG_SPLASH + "3eries_logo.json";
static const std::string ANIM_BOSS_WARNING          = DIR_IMG_GAME + "warning.json";
static const std::string ANIM_WITHDRAW_GUIDE        = DIR_IMG_GAME + "drag.json";
static const std::string ANIM_CONTINUE              = DIR_IMG_GAME + "continue.json";

static const std::string ANIM_NAME_RUN              = "run";
static const std::string ANIM_NAME_CLEAR            = "clear";
static const std::string ANIM_NAME_ENTER            = "enter";
static const std::string ANIM_NAME_EXIT             = "exit";

// 폰트 경로
static const std::string DIR_FONT                   = "fonts/";

static const std::string FONT_COMMODORE             = DIR_FONT + "Commodore.ttf";
static const std::string FONT_SABO                  = DIR_FONT + "Sabo-Filled.otf";
static const std::string FONT_GAME_OVER             = DIR_FONT + "game_over.ttf";
static const std::string FONT_RETRO                 = DIR_FONT + "Commodore.ttf";
static const std::string FONT_NEW_RECORD            = DIR_FONT + "Sabo-Filled.otf";
static const std::string FONT_FAMOUS_SAYING         = DIR_FONT + "game_over.ttf";
static const std::string FONT_STAGE                 = DIR_FONT + "ui_stage.fnt";
static const std::string FONT_STAGE_LARGE           = DIR_FONT + "stage_big.fnt";
static const std::string FONT_STAGE_LARGE_Y         = DIR_FONT + "stage_big_yellow.fnt";
static const std::string FONT_COIN_BLUE             = DIR_FONT + "coin_blue.fnt";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_main.mp3";
static const std::string SOUND_BUTTON_CLICK         = DIR_SOUND + "btn_click.mp3";

// 좌표 및 크기
#define BANNER_HEIGHT           superbomb::AdsHelper::getBannerHeight()

// Color
namespace Color {
    static const cocos2d::Color4B POPUP_BG          = cocos2d::Color4B(0,0,0,255*0.75f);
}

// ZOrder
namespace ZOrder {
    static const int SCENE_TOUCH_LOCKED        = SBZOrder::MIDDLE-1;
    
    static const int POPUP_BOTTOM              = SBZOrder::MIDDLE;
    static const int POPUP_MIDDLE              = POPUP_BOTTOM + 10;
    static const int POPUP_TOP                 = POPUP_MIDDLE + 10;
    
    static const int COMMON_MENU_TOP           = POPUP_TOP;
    static const int COMMON_MENU_BOTTOM        = POPUP_MIDDLE;
    
    static const int GIFT_BOX                  = POPUP_BOTTOM-1;
}

// 연출 시간
namespace EffectDuration {
    static const float POPUP_SLIDE_SLOW        = 0.4f;
    static const float POPUP_SLIDE_NORMAL      = 0.3f;
    static const float POPUP_SLIDE_FAST        = 0.25f;
    
    static const float MENU_SLIDE_NORMAL       = 0.3f;
    static const float MENU_SLIDE_FAST         = 0.25f;
}

#endif /* Define_h */
