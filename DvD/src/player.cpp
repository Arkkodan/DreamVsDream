#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "globals.h"
#include "input.h"
#include "stage.h"
#include "graphics.h"
#include "effect.h"

extern void pause(int);
extern void shake(int);
extern int framePauseTimer;

namespace game {
	#define HITSTUN 14
	#define JHITSTUN 26
	#define BLOCKSTUN 14
	#define TECHSTUN 14

	#define CHIP_DAMAGE_SCALAR 0.1f
	#define JUGGLE 10
	#define JUGGLE_MIN 0.1f
	#define JUGGLE_DEC 0.05f
	#define GRAVITY 1.0f

	#define FRICTION 1.0f
	#define PAUSE_AMPLITUDE 2
	#define BOUNCE_VELOCITY 5
	#define FORCE_GROUND_CAP 10

	#define TECH_FORCE_X 2
	#define TECH_FORCE_Y 5
	#define TECH_GROUND_FORCE_X 5
	#define TECH_GROUND_FORCE_Y 10

	#define STAGE_BUFFER 10

	Projectile::Projectile() :
		fighter(nullptr),
		pos(0, 0),
		vel(0, 0),
		dir(RIGHT),
		flags(F_VISIBLE),
		scale(1.0f),
		idealScale(1.0f),
		spriteAlpha(1.0f),
		idealAlpha(1.0f),
		sprite(0),
		wait(0),
		timer(0),
		frameHit(false),
		state(STATE_NONE),
		onhit(STATE_NONE),
		step(0),
		type('N'),
		movetype('S'),
		knockdownOther(false),
		stunOther(0),
		drawPriorityFrame(0),
		flash(0.0f) {
	}

	void Projectile::think() {
		advanceFrame();

		pos.x += vel.x;
		pos.y += vel.y;
		if(flags & F_GRAVITY) {
			vel.y -= 1.0f;
		}
		
		//Scale!
		if(idealScale != scale) {
			scale = (scale + idealScale) / 2;
		}
		
		//Alpha!
		if(idealAlpha != spriteAlpha) {
			spriteAlpha = (spriteAlpha + idealAlpha) /2 ;
		}
	}

	void Projectile::advanceFrame() {
		timer++;
		if(timer >= wait) {
			wait = 0;
			timer = 0;

			if(step < fighter->states[state].size) {
				frameHit = false;

				while(!wait) {
					if(state == STATE_NONE) {
						return;
					}
					if(step < fighter->states[state].size) {
						handleFrame();
					} else {
						break;
					}
				}
			}
		}

		if(!isPlayer() && step >= fighter->states[state].size && !wait) {
			step = 0;
		}
	}

	void Projectile::handleFrame(ubyte_t command_) {
		int mirror = 1;
		if(dir == LEFT) {
			mirror = -1;
		}

		if(command_ == STEP_None) {
			command_ = readByte();
		}

		switch(command_) {
		case STEP_Sprite:
			sprite = readWord();
			wait = readWord();

			shootProjectile();
			break;
		case STEP_Move:
			pos.x += readFloat() * mirror;
			pos.y += readFloat();
			break;
		case STEP_Scale:
			idealScale = readFloat();
			break;
		case STEP_Type:
			type = readByte();
			if(type == 'A') {
				//Set the draw priority frame
				drawPriorityFrame = os::frame;
			}
			movetype = readByte();
			//if(type == 'A')
			//combo[comboCounter++] = state;
			break;
		case STEP_Attack:
			attack.damage = readFloat();
			attack.type = readByte();
			attack.vX = readFloat();
			attack.vY = readFloat();
			attack.sound = readWord();
			attack.cancel = readByte();
			break;
		case STEP_Sound:
			playSound(readWord());
			break;
		case STEP_Force:
			vel.x = readFloat() * mirror;
			vel.y = readFloat();
			if(vel.y > 0) {
				flags &= ~F_ON_GROUND;
			}
			break;
		case STEP_OnHit:
			onhit = readWord();
			break;
		case STEP_Destroy:
			state = STATE_NONE;
			return;
		case STEP_Visible:
			if(readByte()) {
				flags |= F_VISIBLE;
			} else {
				flags &= ~F_VISIBLE;
			}
			break;
		case STEP_Invincible:
			if(readByte()) {
				flags |= F_INVINCIBLE;
			} else {
				flags &= ~F_INVINCIBLE;
			}
			break;
		case STEP_Gravity:
			if(readByte()) {
				flags |= F_GRAVITY;
			} else {
				flags &= ~F_GRAVITY;
			}
			break;
		case STEP_Airdash:
			if(readByte()) {
				flags |= F_AIRDASH;
			} else {
				flags &= ~F_AIRDASH;
			}
			if(flags & F_ON_GROUND) {
				flags &= ~F_AIRDASH;
			}
			break;
		case STEP_DoubleJump:
			if(readByte()) {
				flags |= F_DOUBLEJUMP;
			} else {
				flags &= ~F_DOUBLEJUMP;
			}
			if(flags & F_ON_GROUND) {
				flags &= ~F_DOUBLEJUMP;
			}
			break;
		case STEP_Mirror:
			if(readByte()) {
				flags |= F_MIRROR;
			} else {
				flags &= ~F_MIRROR;
			}
			break;
		case STEP_Bounce:
			bounceOther.force.x = readFloat();
			bounceOther.force.y = readFloat();
			bounceOther.pause = readByte();
			break;
		case STEP_HitSpark:
			spark = readString();
			break;
		case STEP_Effect:
		    {
                std::string effect = readString();
                int x = readWord();
                int y = readWord();
                int type = readByte();
                bool mirror = readByte();
                int speed = readByte();
                int loops = readByte();
                bool realMirror = (type > 0 && ((dir == LEFT) != mirror)) || (type == 0 && mirror);
                effect::newEffect(effect, x * (realMirror ? -1 : 1) + (type == 1 ? pos.x : 0), y + (type == 1 ? pos.y: 0), type > 0, realMirror, speed, loops, type == 2 ? this : nullptr);
		    }
			break;
		case STEP_Knockdown:
			knockdownOther = true;
			break;
		case STEP_Alpha:
			idealAlpha = readFloat();
			break;
		case STEP_Stun:
			stunOther = readWord();
			break;
		}
	}

	void Projectile::shootProjectile() {
	}

	void Projectile::draw(unsigned int palette) {
		if(flags & F_VISIBLE) {
			fighter->draw(sprite, pos.x, pos.y, isMirrored(), scale, palette, spriteAlpha, 0.0f, 0.0f, 0.0f, 0.0f);
		}
	}


	void Projectile::setState(int state_) {
		if(state_ < 0 || state_ >= fighter->nStates) {
			return;
		}

		//Set the new state
		state = state_;
		step = 0;
		timer = 0;
		wait = 0;
		attack.damage = 0.0f;
		bounceOther.force.x = 0.0f;
		bounceOther.force.y = 0.0f;
		bounceOther.pause = 0;
		knockdownOther = false;
		stunOther = false;
		if(isPlayer()) {
			flags |= F_GRAVITY;
			flags &= ~F_MIRROR;
			((Player*)this)->nCancels = 0;
		}

		//Advance frame
		advanceFrame();
	}

	void Projectile::setStandardState(unsigned int sstate) {
		if(sstate >= STATE_MAX) {
			return;
		}

		setState(fighter->statesStandard[sstate]);
	}

	void Projectile::playSound(int id) {
		//Play a random sound
		if(id < 0 || id >= fighter->nSounds) {
			return;
		}
		fighter->sounds[id].sounds[util::roll(fighter->sounds[id].size)].play();
	}

	void Projectile::say(int id) {
		//HACK
		if(!isPlayer()) {
			return;
		}
		Player* p = (Player*)this;

		//Play a random sound
		if(id < 0 || id >= fighter->nVoices) {
			return;
		}
		//Randomize on < 50% chance
		if(util::roll(100) < fighter->voices[id].pct) {
			p->speaker.play(&fighter->voices[id].voices[util::roll(fighter->voices[id].size)]/*, fighter->voices[id].pct < 30*/);
		}
	}

	bool Projectile::inStandardState(unsigned int sstate) {
		return fighter->statesStandard[sstate] == state;
	}

	bool Projectile::isPlayer() {
		return false;
	}

	Player::Player() : Projectile(),
		playerNum(0),
		nInputs(0),
		frameInput(0),
		input(0),
		netBuffCounter(0),

		juggle(1.0f),
		hitstun(0),
		pausestun(0),

		nCancels(0),

		hp(0),
		super(0),

		special(0),
		ender(false) {
		state = 0;
		flags = F_CTRL | F_ON_GROUND | F_VISIBLE | F_GRAVITY;
		shoot.state = STATE_NONE;
	}

	Player::~Player() {
	}

	void Player::reset() {
		nInputs = 0;

		juggle = 1.0f;
		hitstun = 0;
		pausestun = 0;

		hp = getMaxHp();
		super = SUPER_DEFAULT;

		special = 0;
		ender = false;

		bounce.force.x = 0;
		bounce.force.y = 0;
		bounce.pause = 0;
		bounceOther = bounce;

		setStandardState(STATE_STAND);
		flags = F_CTRL | F_ON_GROUND | F_VISIBLE | F_GRAVITY;
		attack.damage = 0.0f;
		shoot.state = STATE_NONE;

		flash = 0.0f;

		vel.x = 0;
		vel.y = 0;
		pos.y = 0;
		if(playerNum == 0) {
			pos.x = WINDOW_WIDTH / -4;
			dir = RIGHT;
		} else {
			pos.x = WINDOW_WIDTH / 4;
			dir = LEFT;
		}
	}

	InputBuff::InputBuff() {
		frame = 0;
		input = 0;
	}

	void Player::applyInput() {
		input |= frameInput & INPUT_PRESSMASK;
		input &= ~((frameInput & INPUT_RELMASK) >> INPUT_RELSHIFT);

		//Adjust buffered input
		if(dir == LEFT && menu == MENU_FIGHT) {
			frameInput = flipInput(frameInput);
		}

		if(frameInput) {
			inputs[nInputs].input = frameInput;
			inputs[nInputs].frame = os::frame;
			//Adjust queue if necessary
			if(++nInputs >= INBUFF_SIZE) {
				//Move the input queue back one, then add it
				for(int i = 1; i < INBUFF_SIZE; i++) {
					inputs[i-1] = inputs[i];
				}
				nInputs--;
			}
		}
	}

	void Player::handleInput() {
		//Input
		uint16_t input = this->input;
		if(dir == LEFT) {
			input = flipInput(this->input);
		}

		if(menu == MENU_FIGHT) {
			if(FIGHT->timer_round_in || FIGHT->timer_round_out || FIGHT->timer_ko || FIGHT->ko_player) {
				return;
			}
		}

		if(flags & F_CTRL) {
			//PROCESS COMMANDS
			bool executed = false;

			//Before executing fighter-specific commands, consider the built-in ones first
			//Crouching stuff
			if(!inStandardState(STATE_BEGIN_CROUCH) && isStanding() && (input & INPUT_DOWN)) {
				setStandardStateByInput(STATE_BEGIN_CROUCH);
				//executed = true;
			} else if(!inStandardState(STATE_BEGIN_STAND) && isCrouching() && !(input & INPUT_DOWN)) {
				setStandardStateByInput(STATE_BEGIN_STAND);
				//executed = true;
			}

			//Dashing stuff
			if(inStandardState(STATE_DASH_FORWARD)) {
				if((input & INPUT_DIRMASK) != INPUT_RIGHT) {
					setStandardStateByInput(STATE_DASH_FORWARD_END);
				}
			} else if(inStandardState(STATE_DASH_BACK)) {
				if((input & INPUT_DIRMASK) != INPUT_LEFT) {
					setStandardStateByInput(STATE_DASH_BACK_END);
				}
			}

			//Walking stuff
			else if(isStanding() && !isAttacking() && !inStandardState(STATE_DASH_FORWARD_BEGIN) && !inStandardState(STATE_DASH_BACK_BEGIN)) {
				if((input & INPUT_DIRMASK) == INPUT_LEFT && !inStandardState(STATE_WALK_BACK)) {
					setStandardStateByInput(STATE_WALK_BACK);
					//executed = true;
				} else if((input & INPUT_DIRMASK) == INPUT_RIGHT && !inStandardState(STATE_WALK_FORWARD)) {
					setStandardStateByInput(STATE_WALK_FORWARD);
					//executed = true;
				}

				if((input & INPUT_DIRMASK) != INPUT_LEFT && (input & INPUT_DIRMASK) != INPUT_RIGHT && (inStandardState(STATE_WALK_BACK) || inStandardState(STATE_WALK_FORWARD))) {
					setStandardStateByInput(STATE_STAND);
					//executed = true;
				}
			}

			//More walking stuff
			if(isCrouching() && !isAttacking()) {
				if((input & INPUT_DIRMASK) == (INPUT_LEFT | INPUT_DOWN) && !inStandardState(STATE_CROUCH_WALK_BACK)) {
					setStandardStateByInput(STATE_CROUCH_WALK_BACK);
					//executed = true;
				} else if((input & INPUT_DIRMASK) == (INPUT_RIGHT | INPUT_DOWN) && !inStandardState(STATE_CROUCH_WALK_FORWARD)) {
					setStandardStateByInput(STATE_CROUCH_WALK_FORWARD);
					//executed = true;
				}

				if((input & INPUT_DIRMASK) != (INPUT_LEFT | INPUT_DOWN) && (input & INPUT_DIRMASK) != (INPUT_RIGHT | INPUT_DOWN) && (inStandardState(STATE_CROUCH_WALK_BACK) || inStandardState(STATE_CROUCH_WALK_FORWARD))) {
					setStandardStateByInput(STATE_CROUCH);
					//executed = true;
				}
			}

			//First, do multi-step commands.
			//if(!executed)
			{
				for(int i = 0; i < fighter->nCommands; i++) {
					if(fighter->commands[i].comboC > 1) {
						if(fighter->commands[i].comboC <= nInputs) {
							//Directional keys first
							bool equal = false;
							int c = fighter->commands[i].comboC - 1;
							for(int j = nInputs - 1; j >= 0; j--) {
								if(keycmp(inputs[j].input, fighter->commands[i].combo[c], fighter->commands[i].generic & (1<<c))) {
									if(!c) {
										if(os::frame - inputs[j].frame < 15) {
											equal = true;
										}
										break;
									}
									c--;
								}
								//else if(!(fighter->commands[i].optional & (1<<c))) break;
							}

							//Command executed successfully! Check the conditions.
							if(equal) {
								if(executeCommand(i)) {
									executed = true;
									nInputs = 0;
									break;
								}
							}
						}
					}
				}
			}


			if(!executed) {
				//Now do single-step commands
				for(int i = 0; i < fighter->nCommands; i++) {
					if(fighter->commands[i].comboC == 1) {
						uint16_t cmp = input;
						if(!(fighter->commands[i].combo[0] & INPUT_DIRMASK)) {
							cmp &= INPUT_KEYMASK;
						}
						if(keycmp(cmp, fighter->commands[i].combo[0], fighter->commands[i].generic & 1)) {
							//Clear out the UP bit if the command used it
							if(fighter->commands[i].combo[0] & INPUT_UP) {
								this->input &= ~INPUT_UP;
							}
							if(executeCommand(i)) {
								executed = true;
								break;
							}
						}
					}
				}
			}
		}

		//Teching

		//See if a key was pressed within the last 3 frames
		bool press = false;
		for(int i = 0; i < nInputs; i++) {
			if(os::frame - inputs[i].frame <= 3 && inputs[i].input & INPUT_KEYMASK) {
				press = true;
				break;
			}
		}

		if(!(flags & (F_DEAD | F_ON_GROUND)) && isBeingHit() && (press || (playerNum == 1 && FIGHT->gametype == GAMETYPE_TRAINING))) {
			if(pos.y + vel.y <= 0.0 && bounce.force.x == 0 && bounce.force.y == 0 && !(flags & F_KNOCKDOWN)) {
				//We're about to hit the ground, so do a ground tech
				pos.y = 0;
				juggle = 1.0f;
				setStandardStateByInput(STATE_JTECH);
				techstun = TECHSTUN;
				flags |= F_INVINCIBLE;

				int mirror = (dir == RIGHT ? 1 : -1);
				if(input & INPUT_LEFT) {
					vel.x = TECH_GROUND_FORCE_X * -mirror;
				} else if(input & INPUT_RIGHT) {
					vel.x = TECH_GROUND_FORCE_X * mirror;
				} else {
					vel.x = 0;
				}
				vel.y = TECH_GROUND_FORCE_Y;
				
				flags &= ~F_OTG;
			} else if(!hitstun && !pausestun && !(flags & F_KNOCKDOWN)) {
				//Enter air tech
				juggle = 1.0f;
				setStandardStateByInput(STATE_JTECH);
				techstun = TECHSTUN;
				flags |= F_INVINCIBLE;
				vel.x = TECH_FORCE_X * (dir == RIGHT ? -1 : 1);
				vel.y = TECH_FORCE_Y;
				
				flags &= ~F_OTG;
			}
		}

		//Clear out certain bits, including buttons (A, B, C)
		this->input &= ~INPUT_KEYMASK;
	}

	void Player::think() {
		if(!framePauseTimer) {
			if(pausestun) {
				pausestun--;
			} else {
				advanceFrame();
				pos.x += vel.x;

				int w = (dir == RIGHT ? fighter->widthRight : fighter->widthLeft);
				if(pos.x + w + STAGE_BUFFER > STAGE.width) {
					//vel.x = 0.0f;
					pos.x = STAGE.width - fighter->widthRight - STAGE_BUFFER;
				}

				w = (dir == LEFT ? fighter->widthRight : fighter->widthLeft);
				if(pos.x - w - STAGE_BUFFER < -STAGE.width) {
					//vel.x = 0.0f;
					pos.x = -STAGE.width + fighter->widthLeft + STAGE_BUFFER;
				}

				if(!(flags & F_ON_GROUND)) {
					pos.y += vel.y;

					float oldvel = vel.y;
					if(flags & F_GRAVITY) {
						vel.y -= fighter->gravity;
					}
					if(oldvel > 0 && vel.y < 0) {
						if(!isAttacking() && !isBeingHit()) {
							setStandardState(STATE_BEGIN_FALL);
						}
						//else if(inStandardState(STATE_KB_BOUNCING)) setStandardState(STATE_KB_FALL);
					}
					if(pos.y < 0) {
						pos.y = 0;


                        effect::newEffect("DustShockWave", pos.x, pos.y, true, dir == LEFT, 1, 1, nullptr);
						flags |= F_ON_GROUND;
						flags &= ~(F_DOUBLEJUMP | F_AIRDASH);

						if(fabs(vel.x) < FRICTION) {
							vel.x = 0;
						} else {
							if(vel.x > 0) {
								vel.x -= FRICTION;
							} else {
								vel.x += FRICTION;
							}
						}

						if(bounce.force.x != 0 || bounce.force.y != 0) {
							vel.x = bounce.force.x;
							vel.y = bounce.force.y;
							bounce.force.x = 0;
							bounce.force.y = 0;
							hitstun = JHITSTUN;
							if(isKnockedBack()) {
								setStandardState(STATE_KB_BOUNCE);
							} else if(isKnockedProne()) {
								setStandardState(STATE_KP_FALLING);
							}
							//If character was already knocked to the ground, make her invincible
							flags &= ~F_ON_GROUND;
							if(flags & F_OTG) {
								flags |= F_INVINCIBLE;
								flags &= ~F_OTG;
							} else {
								flags |= F_OTG;
							}
						} else {
							vel.y = 0;
							if(isKnockedBack()) {
								//If character was already knocked to the ground, make her invincible
								if(flags & F_OTG) {
									flags |= F_INVINCIBLE;
								}

								//Bounced already?
								if(flags & F_BOUNCE) {
									setStandardState(STATE_KB_LAND);
									flags &= ~F_BOUNCE;
									flags |= F_OTG;
								} else {
									setStandardState(STATE_KB_BOUNCE);
									if(vel.y == 0.0f) {
										vel.y = BOUNCE_VELOCITY;
									}
									flags &= ~F_ON_GROUND;
									flags |= F_BOUNCE;
									juggle = JUGGLE_MIN;
								}
							} else if(isKnockedProne()) {
								//If character was already knocked to the ground, make her invincible
								if(flags & F_OTG) {
									flags |= F_INVINCIBLE;
								}

								//Bounced already?
								setStandardState(STATE_KP_LAND);
								flags |= F_OTG;
								juggle = JUGGLE_MIN;
							} else {
								setStandardState(STATE_LAND);
								juggle = 1.0f;
							}
						}
						if(bounce.pause) {
							pausestun = bounce.pause;
							bounce.pause = 0;
						}
					}
				} else {
					if(flags & F_GRAVITY) {
						if(fabs(vel.x) < FRICTION) {
							vel.x = 0.0f;
						} else {
							if(vel.x > 0) {
								vel.x -= FRICTION;
							} else {
								vel.x += FRICTION;
							}
						}
					}
				}

				//Hitstun
				if(techstun) {
					techstun--;
				}
				if(hitstun) {
					hitstun--;
				}
			}

			if(FIGHT->gametype == GAMETYPE_TRAINING && !isBeingHit() && !isKnocked()) {
				hp = getMaxHp();
			}

			//advanceFrame();

			//Think projectiles
			for(int i = 0; i < MAX_PROJECTILES; i++) {
				if(projectiles[i].state != STATE_NONE) {
					projectiles[i].think();
				}
			}
		}
		
		//Scale!
		if(idealScale != scale) {
			scale = (scale + idealScale) / 2;
		}
		
		//Alpha!
		if(idealAlpha != spriteAlpha) {
			spriteAlpha = (spriteAlpha + idealAlpha) /2 ;
		}

		//Color flash
		if(flash) {
			flash -= 0.05f;
			if(flash < 0.0f) {
				flash = 0.0f;
			}
		}
	}

	void Player::advanceFrame() {
		Projectile::advanceFrame();

		if(step >= fighter->states[state].size && !wait) {
			//End attacks/land/stop recoiling
			if(isBeingHit() || isInBlock()) {
				if((inStandardState(STATE_HIT_MID) || inStandardState(STATE_HIT_HIGH)) && !(flags & F_ON_GROUND)) {
					setStandardState(STATE_KB);
				} else if(inStandardState(STATE_HIT_LOW) && !(flags & F_ON_GROUND)) {
					setStandardState(STATE_KP);
				} else if(inStandardState(STATE_KB_LAND)) {
					setStandardState(STATE_ON_BACK);
				} else if(inStandardState(STATE_KP_LAND)) {
					setStandardState(STATE_PRONE);
				} else if(inStandardState(STATE_ON_BACK)) {
					if(flags & F_ON_GROUND && hp) {
						juggle = 1.0f;
						setStandardState(STATE_RECOVER_BACK);
						flags &= ~(F_OTG | F_INVINCIBLE | F_KNOCKDOWN);
					}
				} else if(inStandardState(STATE_PRONE)) {
					if((flags & F_ON_GROUND) && !(flags & F_DEAD)) {
						juggle = 1.0f;
						setStandardState(STATE_RECOVER_PRONE);
						flags &= ~(F_OTG | F_INVINCIBLE | F_KNOCKDOWN);
					}
				} else if(!hitstun) {
					becomeIdle();
				}
			} else if(inStandardState(STATE_JTECH)) {
				if(techstun) {
					step = 0;
				} else {
					flags |= F_CTRL;
					flags &= ~F_INVINCIBLE;
					becomeIdle();
				}
			} else if(isAttacking()) {
				becomeIdle();
			} else if(isJumping() || inStandardState(STATE_LAND) || inStandardState(STATE_RECOVER_BACK) || inStandardState(STATE_RECOVER_PRONE) || inStandardState(STATE_DASH_FORWARD_END) || inStandardState(STATE_DASH_BACK_END)) {
				becomeIdle();
			} else if(inStandardState(STATE_BEGIN_CROUCH)) {
				setStandardState(STATE_CROUCH);
			} else if(inStandardState(STATE_BEGIN_STAND)) {
				setStandardState(STATE_STAND);
			} else if(inStandardState(STATE_DASH_FORWARD_BEGIN)) {
				setStandardState(STATE_DASH_FORWARD);
			} else if(inStandardState(STATE_DASH_BACK_BEGIN)) {
				setStandardState(STATE_DASH_BACK);
			} else {
				step = 0;
			}
		}
	}

	void Player::handleFrame(ubyte_t command_) {
		if(command_ == STEP_None) {
			command_ = readByte();
		}

		switch(command_) {
		case STEP_Ctrl:
			if(readByte()) {
				flags |= F_CTRL;
			} else {
				flags &= ~F_CTRL;
			}
			break;
		case STEP_Say:
			say(readWord());
			break;
		case STEP_Super:
			break;
		case STEP_Special:
			special = 2500 * SPF;
			pause(2500 * SPF);
			effect::newEffect("Actionlines", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, false, false, 1, 5, nullptr);
			switch(fighter->group) {
			case 0:
				sndTransformYn.play();
				effect::newEffect("Transform_yn", 0, fighter->height / 2, true, dir == LEFT, 1, 1, this);
				break;
			case 1:
				sndTransform2kki.play();
				effect::newEffect("Transform_2kki", 0, fighter->height, true, dir == LEFT, 1, 1, this);
				break;
			case 2:
				sndTransformFlow.play();
				effect::newEffect("Transform_flow", 0, fighter->height / 2, true, dir == LEFT, 1, 1, this);
				break;
			}
			flash = 1.0f;
			break;
		case STEP_Shoot:
			shoot.state = readWord();
			shoot.force.x = readFloat();
			shoot.force.y = readFloat();
			attack.damage = 0.0f;
			break;
		case STEP_Cancel:
			cancels[nCancels++] = readWord();
			if(nCancels >= CANCEL_MAX) {
				nCancels = CANCEL_MAX - 1;
			}
			break;

		default:
			Projectile::handleFrame(command_);
			break;
		}
	}

	void Player::shootProjectile() {
		//Calculate hotspot
		if(fighter->sprites[sprite].aHitBoxes.size && shoot.state != STATE_NONE) {
			int mirror1 = 1;
			int mirror2 = 1;
			if(isMirrored()) {
				mirror1 = -1;
			}
			if(dir == LEFT) {
				mirror2 = -1;
			}

			util::Vector hotspot;
			hotspot.x = fighter->sprites[sprite].aHitBoxes.boxes[0].pos.x + fighter->sprites[sprite].aHitBoxes.boxes[0].size.x / 2;
			hotspot.y = fighter->sprites[sprite].aHitBoxes.boxes[0].pos.y + fighter->sprites[sprite].aHitBoxes.boxes[0].size.y / 2;

			projectiles[projectileId].fighter = fighter;
			projectiles[projectileId].vel.x = shoot.force.x * mirror2;
			projectiles[projectileId].vel.y = shoot.force.y;
			projectiles[projectileId].pos.x = pos.x + hotspot.x * mirror1;
			projectiles[projectileId].pos.y = pos.y + hotspot.y;
			projectiles[projectileId].flags = F_VISIBLE;
			projectiles[projectileId].dir = dir;
			projectiles[projectileId].flash = 0.0f;
			projectiles[projectileId].setState(shoot.state);
			if(++projectileId >= MAX_PROJECTILES) {
				projectileId = 0;
			}

			shoot.state = STATE_NONE;
		}
	}

	void Projectile::interact(Projectile* other) {
		//Check for hitboxes
		Player* pother = nullptr;
		Player* pself = nullptr;
		if(other->isPlayer()) {
			pother = (Player*)other;
		}
		if(isPlayer()) {
			pself = (Player*)this;
		}
		if(fighter->sprites[sprite].aHitBoxes.size && !(other->flags & F_INVINCIBLE)) {
			if(!frameHit && (attack.damage || stunOther)) {
				util::Vector colpos;
				int hit = fighter->sprites[sprite].collide(
					      (int)pos.x, (int)pos.y, (int)other->pos.x, (int)other->pos.y,
					      isMirrored(), other->isMirrored(),
					      scale, other->scale,
					      other->fighter->sprites + other->sprite,
					      &colpos, false);

				if(hit == sprite::HIT_HIT) {
					//Automatically reset draw priority
					drawPriorityFrame = os::frame;

					frameHit = true;
					if(attack.cancel && pself) {
						pself->flags |= F_CTRL;
					}

					pause(6);
					shake(6);
					int mirror = 1;
					if(dir == LEFT) {
						mirror = -1;
					}

					if(pother) {
						bool blocked = false;
						if(pother->isBlocking()) {
							if(attack.type == HT_MID) {
								blocked = true;
							} else if(pother->isJumping()) {
								blocked = true;
							} else if(attack.type == HT_HIGH) {
								if(pother->isStanding()) {
									blocked = true;
								}
							} else if(attack.type == HT_LOW) {
								if(pother->isCrouching()) {
									blocked = true;
								}
							}
						}

						if(blocked) {
							effect::newEffect("BlockHit", colpos.x, colpos.y, true, dir == LEFT, 1, 1, nullptr);

							//Horizontal velocity caps when on ground
							float _force = attack.vX;
							if(pother->flags & F_ON_GROUND && fabs(_force) > FORCE_GROUND_CAP) {
								if(_force > 0) {
									_force = FORCE_GROUND_CAP;
								} else {
									_force = -FORCE_GROUND_CAP;
								}
							}

							if(_force > 0 && (flags & F_ON_GROUND)) {
								if(pother->pos.x + pother->fighter->widthLeft + STAGE_BUFFER >= STAGE.width || pother->pos.x - pother->fighter->widthLeft - STAGE_BUFFER <= -STAGE.width) {
									vel.x = _force * -mirror;
								} else {
									pother->vel.x = _force * mirror;
								}
							} else {
								pother->vel.x = _force * mirror;
							}

							playSound(0);

							if(pother->isCrouching()) {
								pother->setStandardState(STATE_CROUCH_BLOCK);
							} else if(pother->isJumping()) {
								pother->setStandardState(STATE_JUMP_BLOCK);
							} else {
								pother->setStandardState(STATE_BLOCK);
							}

							pother->hitstun = BLOCKSTUN;

							pother->takeDamage(attack.damage * CHIP_DAMAGE_SCALAR);
						} else {
							if(pself) {
								pself->comboCounter++;
							}
							
							pother->pausestun = 0;

                            if(spark != "none")
                                effect::newEffect(spark, colpos.x, colpos.y, true, dir == LEFT, 1, 1, nullptr);

							pother->takeDamage(attack.damage);
							if(!(pother->flags & F_ON_GROUND)) {
								pother->bounce.force.x = bounceOther.force.x * mirror;
								pother->bounce.force.y = bounceOther.force.y;
								pother->bounce.pause = bounceOther.pause;
							}
							if(knockdownOther) {
								pother->flags |= F_KNOCKDOWN;
							}

							if((!(pother->flags & F_ON_GROUND) || (pother->flags & (F_OTG | F_DEAD))) && attack.vY == 0) {
								pother->vel.y = (attack.vY + JUGGLE) * pother->juggle;
							} else if(attack.vY > 0) {
								pother->vel.y = attack.vY * pother->juggle;
							} else {
								pother->vel.y = attack.vY;
							}
							if(pother->vel.y > 0) {
								pother->flags &= ~F_ON_GROUND;
							}

							//Horizontal velocity caps when on ground
							float _force = attack.vX;
							if(pother->flags & F_ON_GROUND && fabs(_force) > FORCE_GROUND_CAP) {
								if(_force > 0) {
									_force = FORCE_GROUND_CAP;
								} else {
									_force = -FORCE_GROUND_CAP;
								}
							}

							if(_force > 0 && (flags & F_ON_GROUND)) {
								if(pother->pos.x + pother->fighter->widthLeft + STAGE_BUFFER >= STAGE.width || pother->pos.x - pother->fighter->widthLeft - STAGE_BUFFER <= -STAGE.width) {
									vel.x = _force * -mirror;
								} else {
									pother->vel.x = _force * mirror;
								}
							} else {
								pother->vel.x = _force * mirror;
							}

							playSound(attack.sound);

							switch(attack.type) {
							case HT_HIGH:
								pother->setStandardState(STATE_HIT_HIGH);
								break;
							case HT_MID:
								pother->setStandardState(STATE_HIT_MID);
								break;
							case HT_LOW:
								pother->setStandardState(STATE_HIT_LOW);
								break;
							}

							if(stunOther) {
								pother->pausestun = stunOther;
							}
							if(pother->flags & F_ON_GROUND) {
								pother->hitstun = HITSTUN;
							} else {
								pother->hitstun = JHITSTUN;
								if(pother->juggle > JUGGLE_MIN) {
									pother->juggle -= JUGGLE_DEC;
									if(pother->juggle < JUGGLE_MIN) {
										pother->juggle = JUGGLE_MIN;
									}
								}
							}
					}
					}

					if(onhit != STATE_NONE) {
						setState(onhit);
						onhit = STATE_NONE;
					}
				} else if(hit == sprite::HIT_ATTACK) {
					frameHit = true;
					pother->frameHit = true;
					pause(12);
					shake(12);
					effect::newEffect("BlockHit", colpos.x, colpos.y, true, dir == LEFT, 1, 1, nullptr);
					playSound(0);
				}
			}
		}

		//Check for character collisions
		if(pother && pself) {
			sprite::HitBox me;
			me.size.x = fighter->widthLeft + fighter->widthRight;
			me.pos.y = pos.y;
			me.size.y = fighter->height;

			if(dir == RIGHT) {
				me.pos.x = pos.x - fighter->widthLeft;
			} else {
				me.pos.x = pos.x - fighter->widthRight;
			}

			sprite::HitBox you;
			you.size.x = other->fighter->widthLeft + other->fighter->widthRight;
			you.pos.y = other->pos.y;
			you.size.y = other->fighter->height;
			if(other->dir == RIGHT) {
				you.pos.x = other->pos.x - other->fighter->widthLeft;
			} else {
				you.pos.x = other->pos.x - other->fighter->widthRight;
			}

			util::Vector c;
			if(me.collideOther(&you, &c, true)) {
				//Move us away from each other
				if(me.pos.x < you.pos.x) {
					//pos.x = other->pos.x - other->fighter->widthLeft * 2 - fighter->widthRight * 2;
					pos.x = c.x - fighter->widthRight;
					other->pos.x = c.x + fighter->widthLeft;
				} else if(me.pos.x > you.pos.x) {
					//pos.x = other->pos.x + other->fighter->widthRight * 2 + fighter->widthLeft * 2;
					pos.x = c.x + fighter->widthLeft;
					other->pos.x = c.x - fighter->widthRight;
				} else {
					//UGLY HACK
					//Whomever is higher gets precedence
					if(me.pos.y > you.pos.y) {
						if(dir == LEFT) {
							pos.x = c.x - fighter->widthRight;
							other->pos.x = c.x + fighter->widthLeft;
						} else {
							pos.x = c.x + fighter->widthLeft;
							other->pos.x = c.x - fighter->widthRight;
						}
					}
				}
			}

			//Confine to screen
			if(fabs(pos.x - pother->pos.x) >= WINDOW_WIDTH) {
				//Find the center, assign positions outwards from there
				float _center = (pos.x + pother->pos.x) / 2.0f;

				if(pos.x < pother->pos.x) {
					pos.x = _center - WINDOW_WIDTH / 2;
					pother->pos.x = _center + WINDOW_WIDTH / 2;
				} else {
					pos.x = _center + WINDOW_WIDTH / 2;
					pother->pos.x = _center - WINDOW_WIDTH / 2;
				}
			}

			//Check combo counter
			if(!pother->isBeingHit()) {
				pself->comboCounter = 0;
			}
		}
	}

	void Player::draw(unsigned int palette, bool shadow) {
		float pct = flash;
		float r = 250 / 256.0f;
		float g = 80 / 256.0f;
		float b = 230 / 256.0f;

		if(inStandardState(STATE_JTECH)) {
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
			if(optionEpilepsy) {
				pct = 0.5f;
			} else {
				if(os::frame % 3 == 0) {
					pct = 0.9f;
				}
			}
		}

		if(flags & F_VISIBLE) {
			if(shadow) {
				fighter->drawShadow(sprite, pos.x, isMirrored(), scale);
			} else {
				fighter->draw(sprite, pos.x + (pausestun % 4) * PAUSE_AMPLITUDE * 2 - PAUSE_AMPLITUDE, pos.y, isMirrored(), scale, palette, spriteAlpha, r, g, b, pct);
			}
		}
	}

	void Player::drawSpecial() {
		if(!special) {
			return;
		}

		float alpha = 0.5f;
		if(special > 2200 * SPF) {
			alpha = 1.0 - (special - 2200 * SPF) / (300 * SPF) * 0.5 - 0.5;
		} else if(special <= 1000 * SPF) {
			alpha = 1.0 - ((1000 * SPF - special) / (500 * SPF)) * 0.5 - 0.5;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(0.0f, 0.0f, 0.0f, alpha);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, WINDOW_HEIGHT, 0.0f);
		glVertex3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f);
		glVertex3f(WINDOW_WIDTH, 0.0f, 0.0f);
		glEnd();
		glColor4f(1, 1, 1, 1);

		if(special > 2200 * SPF) {
			float scalar = (special - 2200 * SPF) / (300 * SPF);
			graphics::setColor(255, 255, 255, 1.0 - scalar);
			graphics::setScale(1.0 + scalar * 0.5);
			if(ender) {
				if(dir == RIGHT) {
					fighter->ender.draw(40 - (fighter->ender.w * scalar) / 4, FLIP(46) - fighter->ender.h - (fighter->ender.h * scalar) / 4);
				} else {
					fighter->ender.draw(WINDOW_WIDTH - fighter->ender.w - (40 - (fighter->ender.w * scalar) / 4), FLIP(46) - fighter->ender.h - (fighter->ender.h * scalar) / 4, true);
				}
			} else {
				if(dir == RIGHT) {
					fighter->special.draw(40 - (fighter->special.w * scalar) / 4, FLIP(46) - fighter->special.h - (fighter->special.h * scalar) / 4);
				} else {
					fighter->special.draw(WINDOW_WIDTH - fighter->special.w - (40 - (fighter->special.w * scalar) / 4), FLIP(46) - fighter->special.h - (fighter->special.h * scalar) / 4, true);
				}
			}
		} else if(special > 1000 * SPF) {
			if(ender) {
				if(dir == RIGHT) {
					fighter->ender.draw(40, FLIP(46) - fighter->ender.h);
				} else {
					fighter->ender.draw(WINDOW_WIDTH - fighter->ender.w - 40, FLIP(46) - fighter->ender.h, true);
				}
			} else {
				if(dir == RIGHT) {
					fighter->special.draw(40, FLIP(46) - fighter->special.h);
				} else {
					fighter->special.draw(WINDOW_WIDTH - fighter->special.w - 40, FLIP(46) - fighter->special.h, true);
				}
			}
		} else {
			float scalar = 1.0 - ((1000 * SPF - special) / (500 * SPF));
			graphics::setColor(255, 255, 255, scalar);
			if(ender) {
				if(dir == RIGHT) {
					fighter->ender.draw(40 + (1000 * SPF - special) * 30, FLIP(46) - fighter->ender.h);
				} else {
					fighter->ender.draw(WINDOW_WIDTH - fighter->ender.w - (40 + (1000 * SPF - special) * 30), FLIP(46) - fighter->ender.h, true);
				}
			} else {
				if(dir == RIGHT) {
					fighter->special.draw(40 + (1000 * SPF - special) * 30, FLIP(46) - fighter->special.h);
				} else {
					fighter->special.draw(WINDOW_WIDTH - fighter->special.w - (40 + (1000 * SPF - special) * 30), FLIP(46) - fighter->special.h, true);
				}
			}
		}
		special--;
	}

	void Player::takeDamage(float damage) {
		if(damage > 0) {
			if(hp) {
				hp -= damage * DEFAULT_HP_MAX;
				if(hp <= 0) {
					hp = 0;
					if(flags & F_ON_GROUND) {
						flags &= ~F_ON_GROUND;
						vel.y += 5.0;
					}
					if(!isBeingHit()) {
						setStandardState(STATE_HIT_HIGH);
					}
					think();
					flags |= F_DEAD;
					FIGHT->knockout(playerNum);
				}
			}
		} else {
			hp -= damage;
			if(hp > getMaxHp()) {
				hp = getMaxHp();
			}
		}
	}

	bool Player::executeCommand(int cmd) {
		for(int i = 0; i < fighter->commands[cmd].targetC; i++) {
			bool cndTrue = false;
			for(int j = 0; j < fighter->commands[cmd].targets[i].conditionC; j++) {
				ubyte_t cnd = fighter->commands[cmd].targets[i].conditions[j];

				//Check the conditions
				bool cndNot = false;
				if(cnd & CND_NOT) {
					cndNot = true;
					cnd &= ~CND_NOT;
				}

				switch(cnd) {
				case CND_ON_GROUND:
					if(flags & F_ON_GROUND) {
						cndTrue = true;
					} else {
						cndTrue = false;
					}
					break;

				case CND_CROUCHING:
					cndTrue = isCrouching();
					break;

				case CND_AIRDASH:
					cndTrue = flags & F_AIRDASH;
					break;

				case CND_DOUBLEJUMP:
					cndTrue = flags & F_DOUBLEJUMP;
					break;
				}
				if(cndNot) {
					cndTrue = !cndTrue;
				}

				//If not true, skip to the next target
				if(!cndTrue) {
					break;
				}
			}

			//If the conditions were all true, then this is a valid state to enter
			//Make sure the hitlevel's good
			if(cndTrue) {
				setStateByInput(fighter->commands[cmd].targets[i].state);
				return true;
			}
		}
		return false;
	}

	void Player::becomeIdle() {
		//Become idle based on current state (i.e. jumping idle, crouching idle, standing idle)
		switch(movetype) {
		case 'S':
			setStandardState(STATE_STAND);
			break;

		case 'C':
			setStandardState(STATE_CROUCH);
			break;

		case 'J':
			if(isKnockedBack()) {
				if(vel.y > 0) {
					if(inStandardState(STATE_KB_BOUNCE) || inStandardState(STATE_KB_BOUNCING)) {
						setStandardState(STATE_KB_BOUNCING);
					} else {
						setStandardState(STATE_KB);
					}
				} else {
					if(inStandardState(STATE_KB)) {
						setStandardState(STATE_KB_FALL);
					} else {
						setStandardState(STATE_KB_FALLING);
					}
				}
			} else if(isKnockedProne()) {
				if(vel.y > 0) {
					setStandardState(STATE_KP);
				} else if(inStandardState(STATE_KP)) {
					setStandardState(STATE_KP_FALL);
				} else {
					setStandardState(STATE_KP_FALLING);
				}
			} else {
				if(vel.y > 0) {
					setStandardState(STATE_JUMPING);
				} else {
					setStandardState(STATE_FALLING);
				}
			}
			break;
		}
	}

	//Helper functions
	bool Player::isAttacking() {
		return type == 'A';
	}

	bool Player::isDashing() {
		return type == 'D';
	}

	bool Player::isIdle() {
		return type == 'N';
	}

	bool Player::isBeingHit() {
		return type == 'H';
	}

	bool Player::isInBlock() {
		return type == 'B';
	}

	bool Player::isKnockedBack() {
		return inStandardState(STATE_HIT_HIGH)
		       || inStandardState(STATE_HIT_MID)
		       || inStandardState(STATE_KB)
		       || inStandardState(STATE_KB_FALL)
		       || inStandardState(STATE_KB_FALLING)
		       || inStandardState(STATE_KB_BOUNCE)
		       || inStandardState(STATE_KB_BOUNCING)
		       || inStandardState(STATE_KB_LAND)
		       || inStandardState(STATE_ON_BACK);
	}

	bool Player::isKnockedProne() {
		return inStandardState(STATE_HIT_LOW)
		       || inStandardState(STATE_KP)
		       || inStandardState(STATE_KP_FALL)
		       || inStandardState(STATE_KP_FALLING)
		       || inStandardState(STATE_KP_LAND)
		       || inStandardState(STATE_PRONE);
	}

	bool Player::isKnocked() {
		return isKnockedBack() || isKnockedProne();
	}

	bool Player::isStanding() {
		return movetype == 'S';
	}

	bool Player::isCrouching() {
		return movetype == 'C';
	}

	bool Player::isJumping() {
		return movetype == 'J';
	}

	bool Player::isBlocking() {
		//Check to see if the player is trying to block rather than in a block state
		if(flags & F_DEAD) {
			return false;
		}
		if(isAttacking() || (!(flags & F_CTRL) && !isInBlock())) {
			return false;
		}
		if(dir == LEFT && (input & INPUT_RIGHT)) {
			return true;
		}
		if(dir == RIGHT && (input & INPUT_LEFT)) {
			return true;
		}
		return false;
	}

	int Player::getMaxHp() {
		return DEFAULT_HP_MAX * fighter->defense;
	}

	int8_t Projectile::readByte() {
		int8_t value = *((int8_t*)((char*)fighter->states[state].steps + step));
		step += 1;
		return value;
	}

	int16_t Projectile::readWord() {
		int16_t value = *((int16_t*)((char*)fighter->states[state].steps + step));
		step += 2;
		return value;
	}

	int32_t Projectile::readDword() {
		int32_t value = *((int32_t*)((char*)fighter->states[state].steps + step));
		step += 4;
		return value;
	}

	float Projectile::readFloat() {
		int32_t value = *((int32_t*)((char*)fighter->states[state].steps + step));
		step += 4;
		return value / (float)FLOAT_ACCURACY;
	}

    std::string Projectile::readString() {
		ubyte_t size = readByte();
		char* ptr = (char*)fighter->states[state].steps + step;
		std::string str = std::string(ptr, ptr + size);
		step += size;
		return str;
	}

	bool Projectile::isMirrored() {
		if(flags & F_MIRROR) {
			return dir == RIGHT;
		}
		return dir == LEFT;
	}

	void Player::setDir(char _dir) {
		if(dir != _dir) {
			dir = _dir;
		}
	}

	void Player::setStateByInput(int state) {
		if(Player::isAttacking()) {
			//Only set the state if this is on the list of cancels
			for(int i = 0; i < nCancels; i++)
				if(state == cancels[i]) {
					setState(state);
					return;
				}
		} else {
			setState(state);
		}
	}

	void Player::setStandardStateByInput(unsigned int sstate) {
		if(sstate >= STATE_MAX) {
			return;
		}

		setStateByInput(fighter->statesStandard[sstate]);
	}

	bool Player::keycmp(uint16_t key1, uint16_t key2, bool generic) {
		if(generic) {
			if(key1 & key2 & INPUT_DIRMASK) {
				return (key1 & INPUT_KEYMASK) == (key2 & INPUT_KEYMASK);
			}
			return false;
		} else {
			return key1 == key2;
		}
		//return dir1 == dir2;
	}

	uint16_t Player::flipInput(uint16_t in) {
		if(in & INPUT_LEFT) {
			in &= ~INPUT_LEFT;
			in |= INPUT_RIGHT;
		} else if(in & INPUT_RIGHT) {
			in &= ~INPUT_RIGHT;
			in |= INPUT_LEFT;
		}
		if(in & INPUT_LEFT_R) {
			in &= ~INPUT_LEFT_R;
			in |= INPUT_RIGHT_R;
		} else if(in & INPUT_RIGHT_R) {
			in &= ~INPUT_RIGHT_R;
			in |= INPUT_LEFT_R;
		}
		return in;
	}

	bool Player::isPlayer() {
		return true;
	}
}
