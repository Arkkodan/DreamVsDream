#include "sprite.h"
#include "fighter.h"
#include "error.h"
#include "parser.h"
#include "player.h"
#include "stage.h"
#include "graphics.h"
#include "file.h"

namespace game {
	Fighter fighters[FIGHTERS_MAX];

#ifdef GAME
	void init() {
		fighters[0].create("madotsuki");
		fighters[1].create("maddysucky");
		fighters[2].create("sabitsuki");
	}

	void deinit() {
	}

	//Load/create a fighter
	void Fighter::create(std::string name_) {
		name = std::move(name_);

		//Get that file opened
		File file;
		if(!file.open(FILE_READ_NORMAL, "chars/" + name + ".char")) {
			return;
		}

		//Read header
		dname = file.readStr();
		group = file.readByte();
		defense = file.readFloat();
		height = file.readWord();
		widthLeft = file.readWord();
		widthRight = file.readWord();
		gravity = file.readFloat();
		nPalettes = file.readByte();

		//Read palettes
		if(graphics::shader_support) {
			palettes = new GLuint[nPalettes * 2];
			glGenTextures(nPalettes * 2, palettes);
		} else {
			palettes = nullptr;
		}

		ubyte_t palette_first[256 * 3];
		ubyte_t palette[256 * 3];

		//Initialize first colors to 0
		for(int i = 0; i < 3; i++) {
			palette_first[i] = 0;
			palette[i] = 0;
		}

		for(int i = 0; i < nPalettes * 2; i++) {
			ubyte_t* pal;
			if(i == 0) {
				pal = palette_first;
			} else {
				pal = palette;
			}

			/*char buff[255 * 3];
			file.read(buff, 255 * 3);

			//Copy into the palette
			for(int j = 0; j < 255; j++)
			{
			    pal[4 * j + 4] = buff[3 * j + 0];
			    pal[4 * j + 5] = buff[3 * j + 1];
			    pal[4 * j + 6] = buff[3 * j + 2];
			    pal[4 * j + 7] = 255;
			}*/
			file.read(pal + 3, 255 * 3);

			//Make data a palette
			if(graphics::shader_support) {
				glBindTexture(GL_TEXTURE_2D, palettes[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pal);
			}
		}

		//Read sprite::Sprites
		nSprites = file.readWord();
		sprites = new sprite::Sprite[nSprites];
		for(int i = 0; i < nSprites; i++) {
			sprites[i].atlas = &atlas_sprites;
			sprites[i].atlas_sprite = file.readWord();
			sprites[i].x = file.readWord();
			sprites[i].y = file.readWord();

			sprites[i].hitBoxes.init(file.readByte());
			for(int j = 0; j < sprites[i].hitBoxes.size; j++) {
				sprites[i].hitBoxes.boxes[j].pos.x = file.readWord();
				sprites[i].hitBoxes.boxes[j].pos.y = file.readWord();
				sprites[i].hitBoxes.boxes[j].size.x = file.readWord();
				sprites[i].hitBoxes.boxes[j].size.y = file.readWord();
			}
			sprites[i].aHitBoxes.init(file.readByte());
			for(int j = 0; j < sprites[i].aHitBoxes.size; j++) {
				sprites[i].aHitBoxes.boxes[j].pos.x = file.readWord();
				sprites[i].aHitBoxes.boxes[j].pos.y = file.readWord();
				sprites[i].aHitBoxes.boxes[j].size.x = file.readWord();
				sprites[i].aHitBoxes.boxes[j].size.y = file.readWord();
			}
		}
		if(graphics::shader_support) {
			atlas_sprites.create("chars/" + name + ".atlas");
		} else {
			atlas_sprites.createFromPalette("chars/" + name + ".atlas", palette_first);
		}

		//Read sounds
		nSounds = file.readWord();
		sounds = new SoundGroup[nSounds];
		for(int i = 0; i < nSounds; i++) {
			sounds[i].init(file.readWord());
			for(int j = 0; j < sounds[i].size; j++) {
				sounds[i].sounds[j].createFromFile("chars/" + name + "/sounds/" + file.readStr() + ".wav");
			}
		}

		//Read voices
		nVoices = file.readWord();
		voices = new VoiceGroup[nVoices];
		for(int i = 0; i < nVoices; i++) {
			voices[i].init(file.readWord());
			voices[i].pct = file.readByte();
			for(int j = 0; j < voices[i].size; j++) {
				voices[i].voices[j].createFromFile("chars/" + name + "/voices/" + file.readStr() + ".wav");
			}
		}

		//Read states (gah)
		nStates = file.readWord();
		states = new State[nStates];
		for(int state = 0; state < nStates; state++) {
			states[state].size = file.readWord();
			states[state].steps = malloc(states[state].size);
			file.read(states[state].steps, states[state].size);
		}

		//Read commands
		nCommands = file.readWord();
		commands = new Command[nCommands];
		for(int i = 0; i < nCommands; i++) {
			commands[i].generic = file.readWord();
			commands[i].comboC = file.readWord();
			for(int j = 0; j < commands[i].comboC; j++) {
				commands[i].combo[j] = file.readWord();
			}
			commands[i].targetC = file.readWord();
			commands[i].targets = new CommandTarget[commands[i].targetC];
			for(int j = 0; j < commands[i].targetC; j++) {
				commands[i].targets[j].state = file.readWord();
				commands[i].targets[j].conditionC = file.readWord();
				for(int k = 0; k < commands[i].targets[j].conditionC; k++) {
					commands[i].targets[j].conditions[k] = file.readWord();
				}
			}
		}

		for(int i = 0; i < STATE_MAX; i++) {
			statesStandard[i] = file.readWord();
		}

		//Load UI portrait
		int width = file.readWord();
		int height = file.readWord();
		ubyte_t* data = (ubyte_t*)malloc(width * height);
		file.read(data, width * height);

        portrait_ui.createFromMemory(data, width, height, COLORTYPE_INDEXED, graphics::shader_support ? nullptr : palette_first, true);
		free(data);

		//Portraits
		select.createFromFile("chars/" + name + "/portraits/select.png");
		portrait.createFromFile("chars/" + name + "/portraits/portrait.png");
		special.createFromFile("chars/" + name + "/portraits/special.png");
		ender.createFromFile("chars/" + name + "/portraits/super.png");
	}
#endif

	Fighter::Fighter() :
	    nPalettes(0), palettes(nullptr),
	    nSprites(0), sprites(nullptr)
#ifndef SPRTOOL
        ,
        nSounds(0), sounds(nullptr),
        nVoices(0), voices(nullptr),
        nCommands(0), commands(nullptr),
        nStates(0), states(nullptr)
#endif
    {}

	Fighter::~Fighter() {
#ifdef GAME
		if(graphics::shader_support) {
			glDeleteTextures(nPalettes, palettes);
		}
#endif
		delete [] palettes;
		delete [] sprites;
#ifndef SPRTOOL
		delete [] states;
		delete [] commands;
		delete [] sounds;
        delete [] voices;
#endif
	}

#ifdef GAME

	void Fighter::draw(int sprite, int x, int y, bool mirror, float scale, unsigned int palette, float alpha, float r, float g, float b, float pct) {
		if(graphics::shader_support) {
			graphics::setPalette(palettes[palette], alpha, r, g, b, pct);
			sprites[sprite].draw(x, y, mirror, scale);
			glUseProgram(0);
		} else {
			if(palette) {
				graphics::setColor(150, 150, 150, 1.0);
			}
			sprites[sprite].draw(x, y, mirror, scale);
		}
	}

	void Fighter::drawShadow(int sprite, int x, bool mirror, float scale) {
		if(graphics::shader_support) {
			graphics::setPalette(0, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f);
			sprites[sprite].drawShadow(x, mirror, scale);
			glUseProgram(0);
		} else {
			graphics::setColor(0.0f, 0.0f, 0.0f, 0.5f);
			sprites[sprite].drawShadow(x, mirror, scale);
		}
	}

#endif

#ifndef SPRTOOL
	SoundGroup::SoundGroup() {
		size = 0;
		sounds = nullptr;
	}

	SoundGroup::~SoundGroup() {
		delete [] sounds;
	}

	void SoundGroup::init(int size_) {
		size = size_;
		if(size) {
#ifdef COMPILER
			sounds = new std::string[size];
#else
			sounds = new audio::Sound[size];
#endif
		}
	}
#endif

#ifndef SPRTOOL
	VoiceGroup::VoiceGroup() :
	    size(0),
	    pct(100),
	    voices(nullptr)
    {}

	VoiceGroup::~VoiceGroup() {
		delete [] voices;
	}

	void VoiceGroup::init(int _size) {
		size = _size;
		if(size) {
#ifdef COMPILER
			voices = new std::string[size];
#else
			voices = new audio::Voice[size];
#endif
		}
	}
#endif

	Command::Command() :
	    comboC(0),
	    generic(0),
	    targetC(0), targets(nullptr)
    {}

	Command::~Command() {
		delete [] targets;
	}

	CommandTarget::CommandTarget() {
		state = 0;
		conditionC = 0;
	}

	State::State() :
	    size(0),
	    steps(nullptr)
	{}

	State::~State() {
		free(steps);
	}

	void setBool(bool& dst, char src) {
		if(src == SBOOL_UNDEFINED) {
			return;
		}
		if(src == SBOOL_TRUE) {
			dst = true;
		} else {
			dst = false;
		}
	}

	void setStateBool(char& dst, bool src) {
		if(src) {
			dst = SBOOL_TRUE;
		} else {
			dst = SBOOL_FALSE;
		}
	}
}
