#ifndef FIGHTER_H_INCLUDED
#define FIGHTER_H_INCLUDED

#include "../util/vec2.h"
#include "sprite.h"
#ifdef GAME
#include "sound.h"
#include "image.h"
#include "atlas.h"
#endif // GAME

#include <vector>
#include <array>
#include <string>

namespace game {
#ifdef GAME
	/// @brief Create fighters
	/// @details Also creates transform sounds
	void init();
	/// @brief Deinit
	void deinit();
	
	extern audio::Sound sndTransformYn;
	extern audio::Sound sndTransform2kki;
	extern audio::Sound sndTransformFlow;
#endif
	enum {
		SBOOL_UNDEFINED = -1,
		SBOOL_FALSE,
		SBOOL_TRUE,
	};

	constexpr auto CND_NOT = 1 << 7;

	enum {
		CND_NONE,
		CND_ON_GROUND,
		CND_CROUCHING,
		CND_AIRDASH,
		CND_DOUBLEJUMP,
	};

	/// @brief Data structure containing a command target
	class CommandTarget {
	public:
		CommandTarget();

		int state;
		int conditionC;
		uint8_t conditions[16];
	};

	/// @brief Data structure containing a command
	class Command {
	public:
		Command();
		~Command();

		int comboC;
		uint16_t combo[16];
		uint16_t generic;
		int targetC;
		std::vector<CommandTarget> targets;
	};

#ifndef SPRTOOL
	/// @brief Data structure containing a group of sounds
	class SoundGroup {
	public:
		SoundGroup();
		~SoundGroup();

		void init(int _size);

		int size;
#ifdef COMPILER
		std::string name;
		std::vector<std::string> sounds;
#else
		std::vector<audio::Sound> sounds;
#endif
	};

	/// @brief Data structure containing a group of voice sounds
	class VoiceGroup {
	public:
		VoiceGroup();
		~VoiceGroup();

		void init(int _size);

		int size;
		int pct;
#ifdef COMPILER
		std::string name;
		std::vector<std::string> voices;
#else
		std::vector<audio::Voice> voices;
#endif
	};
#endif

	/// @brief Data structure containing a state
	class State {
	public:
		State();
		~State();

#ifdef COMPILER
		std::string name;
#endif

		unsigned int size;
		std::vector<std::uint8_t> steps;
	};

	//State steps! Crazy stuff.
	constexpr auto SVT_CONST = 0;
	constexpr auto SVT_VAR = 1;
	constexpr auto SVT_GLOBAL = 2;
	/// @brief Unused
	class StepVar {
	public:
		int8_t type;
		float value;

		float getValue();
	};

	enum {
		STEP_None,
		STEP_Sprite,
		STEP_Move,
		STEP_Scale,
		STEP_Type,
		STEP_Ctrl,
		STEP_Attack,
		STEP_Sound,
		STEP_Say,
		STEP_Heal,
		STEP_Special,
		STEP_Super,
		STEP_Force,
		STEP_Shoot,
		STEP_OnHit,
		STEP_Destroy,
		STEP_Visible,
		STEP_Invincible,
		STEP_Gravity,
		STEP_Airdash,
		STEP_DoubleJump,
		STEP_Mirror,
		STEP_Bounce,
		STEP_HitSpark,
		STEP_Effect,
		STEP_Cancel,
		STEP_Knockdown,
		STEP_Alpha,
		STEP_Stun,
	};

	constexpr auto HT_LOW = 0;
	constexpr auto HT_MID = 1;
	constexpr auto HT_HIGH = 2;

	/// @brief Data structure containing information about an attack
	struct StepAttack {
		float damage;
		char type;
		float vX;
		float vY;
		int sound;
		bool cancel;
	};

	/// @brief Data structure containing information about a shoot action
	struct StepShoot {
		unsigned int state;
		util::Vectorf force;
	};

	/// @brief Data structure containing information about a bounce action
	struct StepBounce {
		util::Vectorf force;
		int pause;
	};

	enum {
		STATE_STAND,
		STATE_CROUCH,
		STATE_BEGIN_STAND,
		STATE_BEGIN_CROUCH,

		STATE_WALK_FORWARD,
		STATE_WALK_BACK,

		STATE_CROUCH_WALK_FORWARD,
		STATE_CROUCH_WALK_BACK,

		STATE_DASH_FORWARD_BEGIN,
		STATE_DASH_FORWARD,
		STATE_DASH_FORWARD_END,
		STATE_DASH_BACK_BEGIN,
		STATE_DASH_BACK,
		STATE_DASH_BACK_END,

		STATE_JUMPING,
		STATE_BEGIN_FALL,
		STATE_FALLING,
		STATE_LAND,

		STATE_JTECH,

		STATE_HIT_HIGH,
		STATE_HIT_MID,
		STATE_HIT_LOW,

		STATE_BLOCK,
		STATE_CROUCH_BLOCK,
		STATE_JUMP_BLOCK,

		STATE_KB,
		STATE_KB_FALL,
		STATE_KB_FALLING,
		STATE_KB_BOUNCE,
		STATE_KB_BOUNCING,
		STATE_KB_LAND,
		STATE_ON_BACK,
		STATE_RECOVER_BACK,

		STATE_KP,
		STATE_KP_FALL,
		STATE_KP_FALLING,
		STATE_KP_LAND,
		STATE_PRONE,
		STATE_RECOVER_PRONE,

		STATE_VICTORY,
		STATE_DEFEAT,

		STATE_MAX,
	};

	extern const std::array<std::string, STATE_MAX> stateNames;

	constexpr auto STATE_NONE = UINT_MAX;

	/// @brief Fighter class containing data of different fighters and helper
	/// methods
	/// @details Do not confuse with Projectile or Player
	class Fighter {
	public:
		//General fighter stuff
		std::string name; //Data name
		std::string dname; //Display name
		int group;
		float defense;
		int height;
		int widthLeft;
		int widthRight;
		float gravity;

		//Sprites & palettes
		int nPalettes;
#ifdef COMPILER
		std::vector<uint8_t> palettes;
#else
		std::vector<unsigned int> palettes;
#endif
		int nSprites;
		std::vector<sprite::Sprite> sprites;
#ifdef GAME
		Atlas atlas_sprites;
#endif

#ifndef SPRTOOL
		//Sounds
		int nSounds;
		std::vector<SoundGroup> sounds;

		//Voices
		int nVoices;
		std::vector<VoiceGroup> voices;

		//Commands
		int nCommands;
		std::vector<Command> commands;

		//States
		int nStates;
		std::vector<State> states;

		//Standard states
		std::array<unsigned int, STATE_MAX> statesStandard;
#endif

#ifdef GAME
		//Portraits
		Image select;
		Image portrait;
		Image special;
		Image ender;
		Image portrait_ui;
#endif

		Fighter();
		~Fighter();

		void create(std::string name);

#ifdef SPRTOOL
		void saveSpr();
#endif

#ifndef COMPILER
		void draw(int sprite, int x, int y, bool mirror, float scale, unsigned int palette, float alpha, float r, float g, float b, float pct) const;
		void drawShadow(int sprite, int x, bool mirror, float scale) const;
#endif

	protected:
	};

	constexpr auto FIGHTERS_MAX = 3;
	extern std::array<Fighter, FIGHTERS_MAX> fighters;

	/// @brief Set a boolean output parameter based on an enum input parameter
	void setBool(bool& dst, char src);
	/// @brief Set an enum output parameter based on a boolean input parameter
	void setStateBool(char& dst, bool src);
}

#endif // FIGHTER_H_INCLUDED
