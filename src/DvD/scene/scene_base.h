#ifndef DVD_SCENE_SCENE_BASE_H
#define DVD_SCENE_SCENE_BASE_H

#include "../image.h"
#include "../sound.h"
#include "../parser.h"

#include <list>
#include <array>
#include <memory>

/// @brief Image for displaying, intended for scenes
class SceneImage {
public:
	SceneImage(Image& _image, float _x, float _y, float _parallax, Image::Render _render, float _xvel, float _yvel, bool wrap, int round);
	~SceneImage();

	Image image;
	float x, y;
	float parallax; //for stages
	float xOrig, yOrig;
	float xvel, yvel;
	Image::Render render;
	bool wrap;
	int round;

	void think();
	void reset();
	void draw(bool _stage) const;
};

/// @brief Scene base class
class Scene {
public:
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

	static std::array<std::unique_ptr<Scene>, SCENE_MAX> scenes;
	static int scene;
	static int sceneNew;

	static Image imgLoading;

public:
	Scene(std::string name_);
	virtual ~Scene();

	//Members
	std::string name;
	std::list<SceneImage> images;

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
	virtual void draw() const;

	void parseFile(std::string szFileName);
	virtual void parseLine(Parser& parser);

	std::string getResource(std::string szFileName, std::string extension) const;

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

#endif // DVD_SCENE_SCENE_BASE_H
