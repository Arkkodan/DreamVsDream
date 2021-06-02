#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "globals.h"

#include "sound.h"
#include "parser.h"
#include "font.h"
#include "util.h"

#define SCENE scenes[scene]
#define FIGHT ((SceneFight*)scenes[SCENE_FIGHT])

extern Image imgLoading;

enum {
	SCENE_FIGHT,

	SCENE_INTRO,

	SCENE_TITLE,
	SCENE_SELECT,
	SCENE_VERSUS,

	SCENE_OPTIONS,

#ifndef NO_NETWORK
	SCENE_NETPLAY,
#endif

	SCENE_CREDITS,

	SCENE_MAX,

	SCENE_QUIT, // Dummy scene for quitting
};

/// @brief Image for displaying, intended for scenes
class SceneImage {
public:
	SceneImage(Image& _image, float _x, float _y, float _parallax, char _render, float _xvel, float _yvel, bool wrap, int round);
	~SceneImage();

	Image image;
	float x, y;
	float parallax; //for stages
	float xOrig, yOrig;
	float xvel, yvel;
	char render;
	bool wrap;
	int round;

	SceneImage* next;

	void think();
	void reset();
	void draw(bool _stage);
};

/// @brief Scene base class
class Scene {
public:
	Scene(std::string name_);
	virtual ~Scene();

	//Members
	std::string name;
	SceneImage* images;

	bool initialized;

	audio::Music bgm;
	bool bgmPlaying;

	//Video
	//Video* video;

	//audio::Sounds
	audio::Sound sndMenu;
	audio::Sound sndSelect;
	audio::Sound sndBack;
	audio::Sound sndInvalid;

	//Functions
	virtual void init();

	virtual void think();
	virtual void reset();
	virtual void draw();

	void parseFile(std::string szFileName);
	virtual void parseLine(Parser& parser);

	std::string getResource(std::string szFileName, std::string extension);

	//Static stuff
	static float fade;
	static bool fadeIn;
	static void drawFade();

	static void setScene(int _scene);

	static bool input(uint16_t in);

	//Init/deinit
	static void ginit();
	static void deinit();
};

/// @brief Scene for the introductory sceen
class SceneIntro : public Scene {
public:
	SceneIntro();
	~SceneIntro();

	audio::Sound sfx;
	Image instructions;
	Image disclaimer_en;
	Image disclaimer_ja;
	Image shader_error;

	int timer;
	int state;

	void think();
	void draw();

	void parseLine(Parser& parser);
};

/// @brief Scene for the title menu
class SceneTitle : public Scene {
public:
	SceneTitle();
	~SceneTitle();

	void init();

	std::string* themes;
	int nThemes;

	//Members
	int menuX, menuY;
	Font menuFont;
	int menuXOffset;

	ubyte_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	int choiceTimer;
	ubyte_t choiceLast;
	ubyte_t choice;
	ubyte_t submenu;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

/// @brief Helper data structure for SceneSelect
class CursorData {
public:
	util::Vector off;
	Image img;
	Image imgSelect;
	audio::Sound sndSelect;
	audio::Sound sndDeselect;

	int frameC;
	int speed;
	bool grow;

	CursorData();
	~CursorData();
};

enum {
	CURSOR_UNLOCKED,
	CURSOR_COLORSWAP,
	CURSOR_LOCKED,
};

/// @brief Helper object for SceneSelect
class Cursor {
public:
	int pos;
	int posOld;
	int posDefault;

	int frame;
	int timer;

	int timerPortrait;

	int lockState;

	ubyte_t r;
	ubyte_t g;
	ubyte_t b;

	//int sprFrame;
	//int sprTimer;

	Cursor();

	int getGroup(int w, int gW, int gH);
};

/// @brief Character and stage selection
class SceneSelect : public Scene {
public:
	SceneSelect();
	~SceneSelect();

	void init();

	//Members
	int width, height;
	int gWidth, gHeight;
	//Image* sprites;
	SceneImage* gui;
	util::Vector* grid;
	int* gridFighters;
	int gridC;

	Cursor cursors[2];

	CursorData* curData;

	Font font_stage;
	int cursor_stage;
	float cursor_stage_offset;

	//Functions
	void think();
	void reset();
	void draw();

	void newEffect(int player, int group);
	void drawEffect(int player, int group, int _x, int _y, bool spr = false);

	void parseLine(Parser& parser);
};

/// @brief Versus splash cutscene
class SceneVersus : public Scene {
public:
	SceneVersus();
	~SceneVersus();

	void init();

	//Members
	Image* portraits[2];
	int timer1;
	int timer2;
	int timer3;
	int timer4;
	int timer5;
	int timer6;

	float timerF;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

/// @brief Scene for modifying options
class SceneOptions : public Scene {
public:
	SceneOptions();
	~SceneOptions();

	void init();

	//Members
	Font menuFont;

	ubyte_t cursor;
	ubyte_t cursorLast;
	int cursorTimer;

	int madoPos;
	int madoDir;
	int madoFrame;

	Image madoImg;
	audio::Sound madoSfxStep;
	audio::Sound madoSfxPinch;

	audio::Voice dame;
	audio::Voice muri;

	int madoWakeTimer;

	ubyte_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	std::string* themes;
	int nThemes;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

/// @brief Helper object for drawing meters, primarily for SceneFight
class SceneMeter {
public:
	SceneMeter();
	~SceneMeter();

	void draw(float pct, bool mirror, bool flip);

	Image img;
	util::Vector pos;
};

enum {
	GAMETYPE_VERSUS,
	GAMETYPE_TRAINING,
};

/// @brief Fighting game scene
class SceneFight : public Scene {
public:
	SceneFight();
	~SceneFight();

	void init();

	Image hud;
	Image hud_tag;
	Image portraits;
	Image portraits_tag;
	Image timer;
	Font timer_font;
	Image shine;
	Image shine_tag;

	//Bars and stuff
	SceneMeter meterHp;
	SceneMeter meterSuper;
	SceneMeter meterTag;
	SceneMeter meterStun;
	SceneMeter meterGuard;
	SceneMeter meterDpm;

	Image staticImg;
	audio::Sound staticSnd;
	audio::Sound fadeinSnd;
	audio::Sound fadeoutSnd;

	Image round_splash[5];
	Image round_hud[5];
	int x_round_hud;
	int y_round_hud;

	Image ko[3]; //KO, timeout, draw

	Font combo;
	Image comboLeft;
	Image comboRight;
	unsigned int comboLeftOff;
	unsigned int comboRightOff;
	int comboLeftLast;
	int comboRightLast;
	int comboLeftTimer;
	int comboRightTimer;

	util::Vector portraitPos;

	Image win;
	Font win_font;
	audio::Music win_bgm;

	Image orb_null;
	Image orb_win;
	Image orb_draw;
	util::Vector orb_pos;

	//Timers for various effects
	int timer_flash;
	int timer_round_in;
	int timer_round_out;
	int timer_ko; //timeout/draw too

	//Logic
	int round; //What round is it?
	int wins[2]; //How many times each player has on (including draws)
	int win_types[2][3]; //What type of win was it? Win or draw?
	int winner; //Who won the game?

	int ko_player; //Which player was ko'd/lost? 2 = draw
	int ko_type;   //Was it a ko or was it a timeout?

	int game_timer; //this is the actual game timer

	int gametype;

	//Functions
	void think();
	void draw();
	void reset();

	void knockout(int player);

	void parseLine(Parser& parser);
};

/// @brief Scene for navigating netplay and connections
class SceneNetplay : public Scene {
public:
	SceneNetplay();
	~SceneNetplay();

	int choice;
	int mode;
	int digit;
	bool waiting;

	char portStr[5];
	uint16_t port;
	char ipStr[12];
	uint32_t ip;

	int flashTimer;
	int flashDir;
	int drawShake;
	int barPos;

	Image imgScanlines;
	Image imgStatic;
	Image imgLogo;

	Image imgCursor;
	Font menuFont;

	audio::Sound sndOn;
	audio::Sound sndOff;

	audio::Sound sndConStart;
	audio::Sound sndConSuccess;

	audio::Music bgmWait;

	//void init();

	void updatePort(bool toint);
	void updateIp(bool toint);

	void think();
	void draw();
	void reset();
	void parseLine(Parser& parser);
};

/// @brief In-game credits
class SceneCredits : public Scene {
public:

	SceneCredits();
	~SceneCredits();

	//Logic
	bool done;
	float oy;
	int timer_start;
	int timer_scroll;
	float secret_alpha;

	//Data
	Image logo;
	Font font;
	int c_lines;
	char** sz_lines;

	ubyte_t title_r;
	ubyte_t title_g;
	ubyte_t title_b;

	ubyte_t name_r;
	ubyte_t name_g;
	ubyte_t name_b;

	void think();
	void draw();
	void reset();
	void parseLine(Parser& parser);
};

extern Scene* scenes[SCENE_MAX];
extern int scene;

#endif // SCENE_H_INCLUDED
