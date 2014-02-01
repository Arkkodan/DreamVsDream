#include "globals.h"
#include "sprite.h"
#include "stage.h"
#include "graphics.h"
#include "parser.h"
#include "input.h"

#ifdef SPRTOOL
namespace input {
	extern bool selectBoxAttack;
	extern bool selectAll;
	extern HitBox* selectBox;
}
#endif // SPRTOOL

namespace sprite {

	HitBox::HitBox() {}
	HitBox::HitBox(int x, int y, int w, int h) {
		pos.x = x;
		pos.y = y;
		size.x = w;
		size.y = h;
	}

#ifndef COMPILER

	bool HitBox::collidePoint(int pX, int pY) {
		pY = FLIP(pY);

		if(pos.x <= pX && pX <= pos.x + size.x &&
			pos.y <= pY && pY <= pos.y + size.y) {
			return true;
		}
		return false;
	}

	void drwbx(int x, int y, int w, int h) {
		glBegin(GL_QUADS);
		glVertex3f(x, y - h, 0);
		glVertex3f(x, y, 0);
		glVertex3f(x + w, y, 0);
		glVertex3f(x + w, y - h, 0);
		glEnd();
	}

	void HitBox::draw(int _x, int _y, bool attack, bool selected) {
		_x += WINDOW_WIDTH / 2;
		_y = FLIP(_y);

		glBindTexture(GL_TEXTURE_2D, 0);
		if(attack) {
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		} else {
			glColor4f(0.5f, 0.5f, 1.0f, 1.0f);
		}

		drwbx(_x + pos.x, _y - pos.y, size.x, 1); //Bottom
		drwbx(_x + pos.x, _y - pos.y - size.y + 1, size.x, 1); //Top
		drwbx(_x + pos.x, _y - pos.y, 1, size.y); //Left
		drwbx(_x + pos.x + size.x - 1, _y - pos.y, 1, size.y); //Right
		if(selected) {
			if(attack) {
				glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
			} else {
				glColor4f(0.5f, 0.5f, 1.0f, 0.5f);
			}
			drwbx(_x + pos.x, _y - pos.y, size.x, size.y);
		}


		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	bool HitBox::collideOther(HitBox* other, util::Vector* colpos) {
		if(pos.x + size.x < other->pos.x) {
			return false;
		}
		if(pos.x > other->pos.x + other->size.x) {
			return false;
		}
		if(pos.y + size.y < other->pos.y) {
			return false;
		}
		if(pos.y > other->pos.y + other->size.y) {
			return false;
		}

		//Calculate colpos
		if(colpos) {
			//colpos->x = (pos.x + other->pos.x) / 2 + (size.x + other->size.x) / 2;
			//colpos->y = (pos.y + other->pos.y) / 2 + (size.y + other->size.y) / 2;
			if(pos.x < other->pos.x) {
				colpos->x = other->pos.x + (pos.x + size.x - other->pos.x) / 2;
			} else {
				colpos->x = pos.x + (other->pos.x + other->size.x - pos.x) / 2;
			}
			if(pos.y < other->pos.y) {
				colpos->y = other->pos.y + (pos.y + size.y - other->pos.y) / 2;
			} else {
				colpos->y = pos.y + (other->pos.y + other->size.y - pos.y) / 2;
			}
		}
		return true;
	}

	HitBox HitBox::adjust(int _x, int _y, bool m, float scale) {
		HitBox adj;
		adj.size.x = size.x * scale;
		adj.size.y = size.y * scale;

		if(m) {
			adj.pos.x = _x + pos.x * -scale + size.x * -scale;
		} else {
			adj.pos.x = _x + pos.x * scale;
		}
		adj.pos.y = _y + pos.y * scale;

		return adj;
	}

	int Sprite::collide(int x1, int y1, int x2, int y2, bool m1, bool m2, float scale1, float scale2, Sprite* other, util::Vector* colpos) {
		//Check for attack hitbox collision with other sprite
		for(int i = 0; i < aHitBoxes.size; i++) {
			HitBox me = aHitBoxes.boxes[i].adjust(x1, y1, m1, scale1);

			//First, enemy attack hitboxes
			for(int j = 0; j < other->aHitBoxes.size; j++) {
				HitBox you = other->aHitBoxes.boxes[j].adjust(x2, y2, m2, scale2);
				if(me.collideOther(&you, colpos)) {
					return HIT_ATTACK;
				}
			}
			//Now, normal enemy hitboxes
			for(int j = 0; j < other->hitBoxes.size; j++) {
				HitBox you = other->hitBoxes.boxes[j].adjust(x2, y2, m2, scale2);
				if(me.collideOther(&you, colpos)) {
					return HIT_HIT;
				}
			}
		}
		return HIT_NOT;
	}

	void Sprite::draw(int _x, int _y, bool mirror, float scale) {
#ifdef SPRTOOL
		int x2 = _x;
		int y2 = _y;

		if(mirror) {
			_x -= (img.w - x) * scale;
		} else {
			_x -= x * scale;
		}
#else
		AtlasSprite _atlas_sprite = atlas->getSprite(atlas_sprite);

		if(mirror) {
			_x -= (_atlas_sprite.w - x) * scale;
		} else {
			_x -= x * scale;
		}
#endif
		_y -= y * scale;

		graphics::setScale(scale);
#ifdef GAME
		atlas->drawSprite(atlas_sprite, _x, _y, mirror);
#else
		img.drawSprite(_x, _y, mirror);
		extern HitBox* selectBox;
		for(int i = 0; i < hitBoxes.size; i++) {
			hitBoxes.boxes[i].draw(x2, y2, false, Input::selectAll || ((hitBoxes.boxes + i == Input::selectBox) && !Input::selectBoxAttack));
		}
		for(int i = 0; i < aHitBoxes.size; i++) {
			aHitBoxes.boxes[i].draw(x2, y2, true, Input::selectAll || ((aHitBoxes.boxes + i == Input::selectBox) && Input::selectBoxAttack));
		}
#endif
	}

	void Sprite::drawShadow(int _x, bool mirror, float scale) {
#ifndef SPRTOOL
		if(mirror) {
			_x -= atlas->getSprite(atlas_sprite).w * scale - x * scale;
		} else {
			_x -= x * scale;
		}

		graphics::setColor(0, 0, 0, 0.5f);
		graphics::setScale(scale, 0.2f * scale);
		atlas->drawSprite(atlas_sprite, _x, 0, mirror);
#endif
	}
#endif

	Sprite::Sprite() {
	}

	Sprite::~Sprite() {
	}


	HitBoxGroup::HitBoxGroup() {
		size = 0;
		boxes = NULL;
	}

	HitBoxGroup::~HitBoxGroup() {
		delete [] boxes;
	}

	void HitBoxGroup::init(int _size) {
		size = _size;
		if(size) {
			boxes = new HitBox[size];
		} else {
			boxes = NULL;
		}
	}

#ifdef SPRTOOL
	HitBox* HitBoxGroup::newHitbox() {
		HitBox* parser = boxes;
		boxes = new HitBox[size + 1];

		if(parser) {
			for(int i = 0; i < size; i++) {
				boxes[i] = parser[i];
			}
			delete [] parser;
		}
		boxes[size].size = util::Vector(15, 15);
		return &boxes[size++];
	}

	void HitBoxGroup::deleteHitbox(HitBox* box) {
		if(!box || !size) {
			return;
		}

		if(size == 1) {
			delete [] boxes;
			boxes = NULL;
			size = 0;
		} else {
			HitBox* parser = boxes;
			boxes = new HitBox[size - 1];

			int off = 0;
			for(int i = 0; i < size; i++) {
				if(parser + i == box) {
					off = 1;
					continue;
				}
				boxes[i-off] = parser[i];
			}
			size--;
			delete [] parser;
		}
	}
#endif

#ifdef GAME
	int effectCount = 0;
	EffectGroup* effects = NULL;

	void init() {
		//Read up the effects
		Parser parser;
		if(!parser.open("effects/effects.ubu")) {
			return;
		}

		//First pass (count effect groups)
		while(parser.parseLine()) {
			if(parser.isGroup()) {
				if(parser.is("GROUP", 2)) {
					effectCount++;
				}
			}
		}

		effects = new EffectGroup[effectCount];

		//Second pass (count effects within groups, set group ids)
		parser.reset();
		int groupCounter = -1;
		int effectCounter = 0;
		while(parser.parseLine()) {
			if(parser.isGroup()) {
				if(parser.is("GROUP", 2)) {
					if(groupCounter >= 0) {
						effects[groupCounter].init(effectCounter);
					}
					effectCounter = 0;
					effects[++groupCounter].id = atoi(parser.getArg(1));

					//Check the type
					const char* type = parser.getArg(2);
					if(!strcmp(type, "normal")) {
						effects[groupCounter].type = EFFECT_NORMAL;
					} else if(!strcmp(type, "scatter")) {
						effects[groupCounter].type = EFFECT_SCATTER;
					} else if(!strcmp(type, "horizontal")) {
						effects[groupCounter].type = EFFECT_HORIZONTAL;
					} else if(!strcmp(type, "vertical")) {
						effects[groupCounter].type = EFFECT_VERTICAL;
					}

				} else if(parser.getArgC() == 1) {
					effectCounter++;
				}
			}
		}
		if(groupCounter >= 0 && effectCounter > 0) {
			effects[groupCounter].init(effectCounter);
		}

		//Third pass (count frames)
		parser.reset();
		groupCounter = -1;
		effectCounter = -1;
		int frameCounter = 0;
		while(parser.parseLine()) {
			if(parser.isGroup()) {
				if(groupCounter >= 0 && effectCounter >= 0) {
					effects[groupCounter].effects[effectCounter].init(frameCounter);
				}
				if(parser.is("GROUP", 2)) {
					effectCounter = -1;
					frameCounter = 0;
					groupCounter++;
				} else if(parser.getArgC() == 1) {
					effectCounter++;
					frameCounter = 0;
				}
				continue;
			}
			frameCounter++;
		}
		if(groupCounter >= 0 && effectCounter >= 0 && frameCounter > 0) {
			effects[groupCounter].effects[effectCounter].init(frameCounter);
		}

		//Fourth pass (load effects)
		parser.reset();
		groupCounter = -1;
		effectCounter = -1;
		frameCounter = 0;
		while(parser.parseLine()) {
			if(parser.isGroup()) {
				if(parser.getArgC() == 3 && !strcmp(parser.getArg(0), "GROUP")) {
					groupCounter++;
					effectCounter = -1;
					frameCounter = 0;
				} else if(parser.getArgC() == 1) {
					effects[groupCounter].effects[++effectCounter].img.createFromFile("effects/" + std::string(parser.getArg(0)) + ".png");
					frameCounter = 0;
				}
				continue;
			}

			//Frame data
			effects[groupCounter].effects[effectCounter].frames[frameCounter] = parser.getArgInt(0);
			effects[groupCounter].effects[effectCounter].speeds[frameCounter] = parser.getArgInt(1);
			frameCounter++;
		}
	}

	void deinit() {
		delete [] effects;
	}

	Effect::Effect() {
		frameC = 0;
		frames = NULL;
		speeds = NULL;
	}

	Effect::~Effect() {
		delete [] frames;
		delete [] speeds;
	}

	void Effect::init(int _frameC) {
		frameC = _frameC;
		if(frameC) {
			frames = new int[frameC];
			speeds = new int[frameC];
		}
	}

	void Effect::draw(int x_, int y_, int frame_, bool mirror_, int type_, unsigned int inception_) {
		graphics::setRect(0, frames[frame_] * img.w, img.w, img.w);

		unsigned int _delta = os::gameFrame - inception_;

		float _x_scale = 1.0f;
		float _y_scale = 1.0f;

		if(type_ == EFFECT_SCATTER) {
			_x_scale = _y_scale = 0.5;
		} else if(type_ == EFFECT_HORIZONTAL) {
			_x_scale = 1.0f + 0.5f * _delta;
			_y_scale = 1.0f - _delta * 0.01f;
		} else if(type_ == EFFECT_VERTICAL) {
			_x_scale = 1.0f - _delta * 0.01f;
			_y_scale = 1.0f + 0.5f * _delta;
		}

		graphics::setRender(RENDER_ADDITIVE);
		graphics::setScale(_x_scale, _y_scale);
		img.drawSprite(x_ - img.w * _x_scale / 2, y_ - img.w * _y_scale / 2, mirror_);
	}

	EffectGroup::EffectGroup() {
		effects = NULL;
		id = 0;
		type = 0;
		size = 0;
	}

	EffectGroup::~EffectGroup() {
		delete [] effects;
	}

	void EffectGroup::init(int _size) {
		size = _size;
		if(size) {
			effects = new Effect[size];
		}
	}

	int sparkCounter = 0;
	Spark sparks[SPARK_COUNT];

	Spark::Spark() {
		group = -1;
		effect = -1;
		frame = 0;
		timer = 0;
	}

	void Spark::think() {
		if(group == -1) {
			return;
		}

		if(vel.x || vel.y) {
			pos.x += vel.x;
			pos.y += vel.y;
			vel.y -= 1;
		}

		if(++timer >= effects[group].effects[effect].speeds[frame]) {
			timer = 0;
			if(vel.x || vel.y || effects[group].type == EFFECT_HORIZONTAL || effects[group].type == EFFECT_VERTICAL) {
				if(frame < effects[group].effects[effect].frameC - 1) {
					frame++;
				} else {
					alpha -= 0.25;
					if(alpha <= 0.0) {
						group = -1;
						return;
					}
				}
			} else if(++frame >= effects[group].effects[effect].frameC) {
				group = -1;
				frame--;
				return;
			}
		}
	}

	void Spark::draw() {
		if(group == -1) {
			return;
		}

		//graphics::setColor(r * alpha, g * alpha, b * alpha, 1.0);
		glColor4f(alpha, alpha, alpha, 1.0f);
		int _type = effects[group].type;
		if(_type == EFFECT_SCATTER) {
			if(vel.x == 0 && vel.y == 0) {
				_type = EFFECT_NORMAL;
			}
		}
		effects[group].effects[effect].draw(pos.x, pos.y, frame, mirror, _type, inception);
	}

	//void newSpark(int x, int y, int vx, int vy, int group, bool mirror, ubyte_t r, ubyte_t g, ubyte_t b)
	void newSpark(int x, int y, int vx, int vy, int group, bool mirror) {
		if(group == -1) {
			return;
		}

		sparks[sparkCounter].group = group;
		sparks[sparkCounter].effect = util::roll(effects[group].size);
		sparks[sparkCounter].frame = 0;
		sparks[sparkCounter].timer = 0;
		sparks[sparkCounter].pos.x = x;
		sparks[sparkCounter].pos.y = y;
		sparks[sparkCounter].vel.x = vx;
		sparks[sparkCounter].vel.y = vy;
		sparks[sparkCounter].mirror = mirror;
		/*sparks[sparkCounter].r = r;
		sparks[sparkCounter].g = g;
		sparks[sparkCounter].b = b;*/
		sparks[sparkCounter].alpha = 1.0f;
		sparks[sparkCounter].inception = os::gameFrame;

		if(++sparkCounter >= SPARK_COUNT) {
			sparkCounter = 0;
		}
	}

	void newSpark(int x, int y, int group, bool mirror) {
		newSpark(x, y, 0, 0, group, mirror);

		if(effects[group].type == EFFECT_SCATTER) {
			int particles = util::roll(1, 5);
			for(int i = 0; i < particles; i++) {
				newSpark(x, y, util::roll(3, 13) * (mirror) ? -1 : 1, util::roll(5, 15), 1, mirror);
			}
		}
	}

	void think() {
		for(int i = 0; i < SPARK_COUNT; i++) {
			sparks[i].think();
		}
	}

	void draw() {
		for(int i = 0; i < SPARK_COUNT; i++) {
			sparks[i].draw();
		}
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
#endif
