#include "credits.h"

#include "scene.h"

#define CREDITS_OFFSET 20

SceneCredits::SceneCredits() : Scene("credits") {
	oy = 0;
	timer_start = FPS * 6.35;
	timer_scroll = FPS;
	secret_alpha = 0.0f;
	done = false;

	//Data
	title_r = title_g = title_b = name_r = name_g = name_b = 255;
	c_lines = 0;
	sz_lines = nullptr;
}

SceneCredits::~SceneCredits() {
	util::freeLines(sz_lines);
}

void SceneCredits::think() {
	Scene::think();

	if (timer_start) {
		timer_start--;
	}
	else if (timer_scroll) {
		timer_scroll--;
	}
	else if (!done) {
		oy += 0.5;
	}
	else if (secret_alpha < 1.0f) {
		secret_alpha += 0.05f;
		if (secret_alpha > 1.0f) {
			secret_alpha = 1.0f;
		}
	}

	if (input(INPUT_A)) {
		setScene(SCENE_TITLE);
	}
}

void SceneCredits::draw() {
	Scene::draw();

	if (!timer_start) {
		if (done) {
			const static char* _sz = "Secret character unlocked!";
			font.drawText(WINDOW_WIDTH - font.getTextWidth(_sz) - CREDITS_OFFSET, FLIP(font.img.h * 2), _sz, 255, 255, 255, secret_alpha);
		}
		else {
			int y = (WINDOW_HEIGHT - logo.h) / 4 - oy;
			if (y + (int)logo.h >= 0) {
				logo.draw(WINDOW_WIDTH - logo.w, y);
			}

			int oy_title = 0;

			for (int i = 0; i < c_lines; i++) {
				if (*sz_lines[i] == ':') {
					oy_title++;
				}

				y = WINDOW_HEIGHT + (font.img.h * (i + oy_title)) - oy;

				if (y + 32 < 0) {
					if (i == c_lines - 1) {
						done = true;
					}
					continue;
				}
				if (y > WINDOW_HEIGHT) {
					break;
				}

				if (*sz_lines[i] == ':') {
					font.drawText(WINDOW_WIDTH - font.getTextWidth(sz_lines[i] + 1) - CREDITS_OFFSET, y, sz_lines[i] + 1, title_r, title_g, title_b);
				}
				else {
					font.drawText(WINDOW_WIDTH - font.getTextWidth(sz_lines[i]) - CREDITS_OFFSET, y, sz_lines[i], name_r, name_g, name_b);
				}
			}
		}
	}
}

void SceneCredits::reset() {
	Scene::reset();

	done = false;
	oy = 0;
	timer_start = FPS * 6.35;
	timer_scroll = FPS;
	secret_alpha = 0.0f;
}

void SceneCredits::parseLine(Parser& parser) {
	if (parser.is("LOGO", 1)) {
		logo.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	}
	else if (parser.is("CREDITS", 2)) {
		font.createFromFile(getResource(parser.getArg(1), EXT_FONT));
		sz_lines = util::getLinesFromFile(&c_lines, getResource(parser.getArg(2), EXT_TEXT));
	}
	else if (parser.is("COLOR", 6)) {
		title_r = parser.getArgInt(1);
		title_g = parser.getArgInt(2);
		title_b = parser.getArgInt(3);
		name_r = parser.getArgInt(4);
		name_g = parser.getArgInt(5);
		name_b = parser.getArgInt(6);
	}
	else {
		Scene::parseLine(parser);
	}
}
