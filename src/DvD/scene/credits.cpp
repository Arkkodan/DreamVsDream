#include "credits.h"

#include "scene.h"

#include "../player.h"
#include "../sys.h"
#include "../resource_manager.h"
#include "../../fileIO/text.h"
#include "../../util/fileIO.h"

scene::Credits::Credits() : Scene("credits") {
	oy = 0;
	timer_start = sys::FPS * 6.35;
	timer_scroll = sys::FPS;
	secret_alpha = 0.0f;
	done = false;

	//Data
	title_r = title_g = title_b = name_r = name_g = name_b = 255;
	c_lines = 0;
	font = nullptr;
}

scene::Credits::~Credits() {}

void scene::Credits::think() {
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

	if (input(game::INPUT_A)) {
		setScene(SCENE_TITLE);
	}
}

void scene::Credits::draw() const {
	Scene::draw();

	if (!timer_start) {
		if (done) {
			static std::string _sz = "Secret character unlocked!";
			font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(_sz) - CREDITS_OFFSET, sys::FLIP(font->img.h * 2), _sz, 255, 255, 255, secret_alpha);
		}
		else {
			int y = (sys::WINDOW_HEIGHT - logo.h) / 4 - oy;
			if (y + (int)logo.h >= 0) {
				logo.draw(sys::WINDOW_WIDTH - logo.w, y);
			}

			int oy_title = 0;

			for (int i = 0; i < c_lines; i++) {
				const char fc = lines[i].front();
				if (fc == ':') {
					oy_title++;
				}

				y = sys::WINDOW_HEIGHT + (font->img.h * (i + oy_title)) - oy;

				if (y + 32 < 0) {
					if (i == c_lines - 1) {
						done = true;
					}
					continue;
				}
				if (y > sys::WINDOW_HEIGHT) {
					break;
				}

				if (fc == ':') {
					std::string out = lines[i].substr(1);
					font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(out) - CREDITS_OFFSET, y, out, title_r, title_g, title_b);
				}
				else {
					font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(lines[i]) - CREDITS_OFFSET, y, lines[i], name_r, name_g, name_b);
				}
			}
		}
	}
}

void scene::Credits::reset() {
	Scene::reset();

	done = false;
	oy = 0;
	timer_start = sys::FPS * 6.35;
	timer_scroll = sys::FPS;
	secret_alpha = 0.0f;
}

void scene::Credits::parseLine(Parser& parser) {
	if (parser.is("LOGO", 1)) {
		logo.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("CREDITS", 2)) {
		font = getResourceT<Font>(parser.getArg(1));
		lines = fileIO::readTextAsLines(util::getPath(getResource(parser.getArg(2), Parser::EXT_TEXT)));
		c_lines = lines.size();
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
