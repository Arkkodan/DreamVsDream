#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "globals.h"

#include "sound.h"
#include "parser.h"
#include "font.h"
#include "util.h"

#define MENU menus[menu]
#define FIGHT ((MenuFight*)menus[MENU_FIGHT])

extern Image imgLoading;

enum {
	MENU_FIGHT,

	MENU_INTRO,

	MENU_TITLE,
	MENU_SELECT,
	MENU_VERSUS,

	MENU_OPTIONS,

#ifndef NO_NETWORK
	MENU_NETPLAY,
#endif

	MENU_CREDITS,

	MENU_MAX,

	MENU_QUIT, //Dummy menu for quitting
};

class MenuImage {
public:
	MenuImage(Image& _image, float _x, float _y, float _parallax, char _render, float _xvel, float _yvel, bool wrap, int round);
	~MenuImage();

	Image image;
	float x, y;
	float parallax; //for stages
	float xOrig, yOrig;
	float xvel, yvel;
	char render;
	bool wrap;
	int round;

	MenuImage* next;

	void think();
	void reset();
	void draw(bool _stage);
};

class Menu {
public:
	Menu(std::string name_);
	virtual ~Menu();

	//Members
	std::string name;
	MenuImage* images;

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

	static void setMenu(int _menu);

	static bool input(uint16_t in);

	//Init/deinit
	static void ginit();
	static void deinit();
};

class MenuIntro : public Menu {
public:
	MenuIntro();
	~MenuIntro();

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

class MenuTitle : public Menu {
public:
	MenuTitle();
	~MenuTitle();

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

class Cursor {
public:
	int pos;
	int posOld;
	int posDefault;

	int frame;
	int timer;

	int timerPortrait;

	bool locked;

	ubyte_t r;
	ubyte_t g;
	ubyte_t b;

	//int sprFrame;
	//int sprTimer;

	Cursor();

	int getGroup(int w, int gW, int gH);
};

class MenuSelect : public Menu {
public:
	MenuSelect();
	~MenuSelect();

	void init();

	//Members
	int width, height;
	int gWidth, gHeight;
	//Image* sprites;
	MenuImage* gui;
	util::Vector* grid;
	int* gridFighters;
	int gridC;

	Cursor cursors[2];

	CursorData* curData;

	int state;

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

class MenuVersus : public Menu {
public:
	MenuVersus();
	~MenuVersus();

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


class MenuOptions : public Menu {
public:
	MenuOptions();
	~MenuOptions();

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

class MenuMeter {
public:
	MenuMeter();
	~MenuMeter();

	void draw(float pct, bool mirror, bool flip);

	Image img;
	util::Vector pos;
};

enum {
	GAMETYPE_VERSUS,
	GAMETYPE_TRAINING,
};

class MenuFight : public Menu {
public:
	MenuFight();
	~MenuFight();

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
	MenuMeter meterHp;
	MenuMeter meterSuper;
	MenuMeter meterTag;
	MenuMeter meterStun;
	MenuMeter meterGuard;
	MenuMeter meterDpm;

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

class MenuNetplay : public Menu {
public:
	MenuNetplay();
	~MenuNetplay();

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

class MenuCredits : public Menu {
public:

	MenuCredits();
	~MenuCredits();

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

extern Menu* menus[MENU_MAX];
extern int menu;

#endif // MENU_H_INCLUDED
