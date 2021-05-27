#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "globals.h"

#include "fighter.h"

namespace game {
	#define INPUT_DIRMASK 0xF
	#define INPUT_KEYMASK 0x70
	#define INPUT_PRESSMASK 0x00FF
	#define INPUT_RELMASK 0xFF00
	#define INPUT_RELSHIFT 8

	#define INPUT_LEFT (1<<0)
	#define INPUT_RIGHT (1<<1)
	#define INPUT_UP (1<<2)
	#define INPUT_DOWN (1<<3)
	#define INPUT_A (1<<4)
	#define INPUT_B (1<<5)
	#define INPUT_C (1<<6)
	#define INPUT_LEFT_R (INPUT_LEFT<<INPUT_RELSHIFT)
	#define INPUT_RIGHT_R (INPUT_RIGHT<<INPUT_RELSHIFT)
	#define INPUT_UP_R (INPUT_UP<<INPUT_RELSHIFT)
	#define INPUT_DOWN_R (INPUT_DOWN<<INPUT_RELSHIFT)
	#define INPUT_A_R (INPUT_A<<INPUT_RELSHIFT)
	#define INPUT_B_R (INPUT_B<<INPUT_RELSHIFT)
	#define INPUT_C_R (INPUT_C<<INPUT_RELSHIFT)

	//Settable flags
	#define F_CTRL  (1<<0)
	#define F_VISIBLE (1<<1)
	#define F_GRAVITY (1<<2)
	#define F_INVINCIBLE (1<<3)
	#define F_AIRDASH (1<<4)
	#define F_DOUBLEJUMP (1<<5)
	#define F_MIRROR (1<<6)

	//Un-settable flags
	#define F_ON_GROUND (1<<7)
	#define F_KNOCKDOWN (1<<8)
	#define F_BOUNCE (1<<9)
	#define F_OTG (1<<10)
	#define F_DEAD (1<<11)

	#define LEFT 0
	#define RIGHT 1

	#define INBUFF_SIZE 32
	#define NETBUFF_SIZE 256

	#define CANCEL_MAX 32

	#define DEFAULT_HP_MAX 1000
	#define SUPER_MAX 1000
	#define SUPER_DEFAULT (SUPER_MAX * 0.20)

	class InputBuff {
	public:
		uint32_t frame;
		uint16_t input;

		InputBuff();
	};

	class Projectile {
	public:
		int palette;
		Fighter* fighter;

		util::Vectorf pos;
		util::Vectorf vel;

		char dir;
		uint32_t flags;

		float scale;
		float idealScale;
		float spriteAlpha;
		float idealAlpha;

		//State control stuff
		int sprite;
		int wait;
		int timer;
		bool frameHit; //True if attack hit already this frame
		unsigned int state;
		unsigned int onhit;
		unsigned int step;

		char type;
		char movetype;

		//Attack stuff
		std::string spark;
		StepAttack attack;
		StepShoot shoot;
		StepBounce bounce;
		StepBounce bounceOther;
		bool knockdownOther;
		int stunOther;

		//Draw above priority frame
		unsigned int drawPriorityFrame;

		//Flash intensity
		float flash;

		Projectile();

		//Virtual
		virtual void think();
		virtual void interact(Projectile* other);
		virtual void advanceFrame();
		virtual void handleFrame(ubyte_t command = 0);
		virtual void shootProjectile();
		virtual void draw();

		//Non-virtual
		void setState(int state);
		void setStandardState(unsigned int sstate);
		void playSound(int id);
		void say(int id);

		//Read from step memory
		int8_t readByte();
		int16_t readWord();
		int32_t readDword();
		float readFloat();
		std::string readString();

		bool isMirrored();

		bool inStandardState(unsigned int sstate);
		virtual bool isPlayer();
	};

	#define MAX_PROJECTILES 128

	class Player : public Projectile {
	public:
		char playerNum;

		audio::Speaker speaker;

		//ubyte_t combo[256];
		int comboCounter;

		int nInputs;
		InputBuff inputs[INBUFF_SIZE];

		//List of key inputs on this frame (gotten from either SFML or the network)
		uint16_t frameInput;
		uint16_t input; //Persistent input; what keys are held down at the moment
		InputBuff netBuff[NETBUFF_SIZE];
		int netBuffCounter;

		//States and stuff
		int stateCombo;
		float juggle;
		int hitstun;
		int pausestun; //fancy hitstun, essentially
		int techstun; //How long techs last

		//What can we cancel into?
		int nCancels;
		int cancels[CANCEL_MAX];

		//Meters
		int hp;
		int super;

		//Special portrait
		int special;
		bool ender;

		//Projectiles
		Projectile projectiles[MAX_PROJECTILES];
		int projectileId;

		Player();
		~Player();

		void reset(); //Resets back to beginning-of-round state

		void think();
		void advanceFrame();
		void handleFrame(ubyte_t command = 0);
		void shootProjectile();
		void applyInput();
		void handleInput();
		void draw(bool shadow);
		void drawSpecial();

		void takeDamage(float damage);

		bool executeCommand(int cmd);
		//void executeFrame(StateFrame* sf, bool mine);

		void becomeIdle();

		bool isAttacking();
		bool isDashing();
		bool isIdle();
		bool isBeingHit();
		bool isInBlock();
		bool isKnockedBack();
		bool isKnockedProne();
		bool isKnocked();

		bool isStanding();
		bool isCrouching();
		bool isJumping();
		bool isBlocking();

		int getMaxHp();

		void setDir(char _dir);

		void setStateByInput(int state);
		void setStandardStateByInput(unsigned int sstate);

		static bool keycmp(uint16_t key1, uint16_t key2, bool generic);
		static uint16_t flipInput(uint16_t in);
		bool isPlayer();
	};
}

#endif // PLAYER_H_INCLUDED
