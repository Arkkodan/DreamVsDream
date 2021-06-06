#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include "../util/vec2.h"
#ifdef GAME
#include "atlas.h"
#endif // GAME
#ifdef SPRTOOL
#include "image.h"
#endif // SPRTOOL

#include <vector>
#ifndef GAME
#include <string>
#endif // GAME

namespace sprite {
	/// @brief Unused
	void init();
	/// @brief Unused
	void deinit();

	/// @brief Data structure containing a hitbox with some methods
	/// @details A hitbox is either an attacking hitbox or a defending hurtbox
	class HitBox {
	public:
		util::Vector pos;
		util::Vector size;

		HitBox();
		HitBox(int x, int y, int w, int h);

		bool collideOther(HitBox* other, util::Vector* colpos, bool allowOutOfBounds) const;

		HitBox adjust(int x, int y, bool m, float scale) const;

		bool collidePoint(int pX, int pY) const;
		void draw(int x, int y, bool attack, bool selected) const;
	};

	/// @brief Data structure containing a group of hitboxes
	class HitBoxGroup {
	public:
		int size;
		std::vector<HitBox> boxes;

		HitBoxGroup();
		~HitBoxGroup();

		void init(int size);

#ifdef SPRTOOL
		HitBox* newHitbox();
		void deleteHitbox(HitBox* box);
#endif
	};

	enum {
		HIT_NOT,
		HIT_HIT,
		HIT_ATTACK,
	};

	/// @brief Sprite class containing data from atlases with helper methods
	class Sprite {
	public:
		Sprite();
		~Sprite();

		Sprite(const Sprite& other) = delete;
		Sprite& operator=(const Sprite& other) = delete;

		Sprite(Sprite&& other) noexcept;
		Sprite& operator=(Sprite&& other) noexcept;

#ifndef COMPILER
		void draw(int x, int y, bool mirror, float scale) const;
		void drawShadow(int x, bool mirror, float scale) const;
#endif

		int collide(int x1, int y1, int x2, int y2, bool m1, bool m2, float scale1, float scale2, Sprite* other, util::Vector* colpos, bool allowOutOfBounds) const;

		int x;
		int y;

		//Each program has its own way of representing an image.
#ifdef SPRTOOL
		Image img;
#endif

#ifdef GAME
		Atlas* atlas;
		int atlas_sprite;
#else
		std::string name;
#endif
		//Image img;

		HitBoxGroup hitBoxes;
		HitBoxGroup aHitBoxes;
	};
}

#endif // SPRITE_H_INCLUDED
