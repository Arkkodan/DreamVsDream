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
	extern sprite::HitBox* selectBox;
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
			
			if(colpos->x < other->pos.x) {
				colpos->x = other->pos.x;
			} else if(colpos->x > other->pos.x + other->size.x - 1) {
				colpos->x = other->pos.x + other->size.x - 1;
			}
			if(colpos->y < other->pos.y) {
				colpos->y = other->pos.y;
			} else if(colpos->y > pos.y + other->size.y - 1) {
				colpos->y = other->pos.y + other->size.y - 1;
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
		for(int i = 0; i < hitBoxes.size; i++) {
			hitBoxes.boxes[i].draw(x2, y2, false, input::selectAll || ((hitBoxes.boxes + i == input::selectBox) && !input::selectBoxAttack));
		}
		for(int i = 0; i < aHitBoxes.size; i++) {
			aHitBoxes.boxes[i].draw(x2, y2, true, input::selectAll || ((aHitBoxes.boxes + i == input::selectBox) && input::selectBoxAttack));
		}
#endif
	}

#ifndef SPRTOOL
	void Sprite::drawShadow(int _x, bool mirror, float scale) {
		if(mirror) {
			_x -= atlas->getSprite(atlas_sprite).w * scale - x * scale;
		} else {
			_x -= x * scale;
		}

		graphics::setColor(0, 0, 0, 0.5f);
		graphics::setScale(scale, 0.2f * scale);
		atlas->drawSprite(atlas_sprite, _x, 0, mirror);
	}
#endif
#endif

	Sprite::Sprite() {
	}

	Sprite::~Sprite() {
	}


	HitBoxGroup::HitBoxGroup() {
		size = 0;
		boxes = nullptr;
	}

	HitBoxGroup::~HitBoxGroup() {
		delete [] boxes;
	}

	void HitBoxGroup::init(int size) {
		this->size = size;
		if(size) {
			boxes = new HitBox[size];
		} else {
			boxes = nullptr;
		}
	}

#ifdef SPRTOOL
	HitBox* HitBoxGroup::newHitbox() {
		HitBox* foo = boxes;
		boxes = new HitBox[size + 1];

		if(foo) {
			for(int i = 0; i < size; i++) {
				boxes[i] = foo[i];
			}
			delete [] foo;
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
			boxes = nullptr;
			size = 0;
		} else {
			HitBox* foo = boxes;
			boxes = new HitBox[size - 1];

			int off = 0;
			for(int i = 0; i < size; i++) {
				if(foo + i == box) {
					off = 1;
					continue;
				}
				boxes[i-off] = foo[i];
			}
			size--;
			delete [] foo;
		}
	}
#endif
}
