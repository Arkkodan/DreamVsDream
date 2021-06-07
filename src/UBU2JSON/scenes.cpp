#include "scenes.h"

ubu::Scene::Scene(const std::string& file)
	: UBU(file), imageIndex(0)
{}

bool ubu::Scene::parse() {
	if (!parser.exists()) {
		return false;
	}

	while (parser.parseLine()) {
		parseLine();
	}

	return true;
}

void ubu::Scene::parseLine() {
	int argc = parser.getArgC();

	if (parser.is("IMAGE", 3)) {
		j_obj["image"][imageIndex]["image"] = parser.getArg(1);
		j_obj["image"][imageIndex]["pos"]["x"] = parser.getArgFloat(2);
		j_obj["image"][imageIndex]["pos"]["y"] = parser.getArgFloat(3);
		if (argc > 4) {
			j_obj["image"][imageIndex]["renderType"] = parser.getArg(4);
			if (argc > 5) {
				j_obj["image"][imageIndex]["vel"]["x"] = parser.getArgFloat(5);
				if (argc > 6) {
					j_obj["image"][imageIndex]["vel"]["y"] = parser.getArgFloat(6);
					if (argc > 7) {
						j_obj["image"][imageIndex]["wrap"] = parser.getArgBool(7, false);
					}
				}
			}
		}

		imageIndex++;
	}
	else if (parser.is("BGM", 1)) {
		if (argc > 2) {
			j_obj["bgm"]["intro"] = parser.getArg(1);
			j_obj["bgm"]["loop"] = parser.getArg(2);
		}
		else {
			j_obj["bgm"]["loop"] = parser.getArg(1);
		}
	}
	else if (parser.is("SOUND", 4)) {
		j_obj["sound"]["menu"] = parser.getArg(1);
		j_obj["sound"]["select"] = parser.getArg(2);
		j_obj["sound"]["back"] = parser.getArg(3);
		j_obj["sound"]["invalid"] = parser.getArg(4);
	}
	else if (parser.is("VIDEO", 1)) {
		//j_obj["video"] = parser.getArg(1);
	}
}

ubu::Intro::Intro(const std::string& file)
	: Scene(file)
{}

void ubu::Intro::parseLine() {
	if (parser.is("SFX", 1)) {
		j_obj["sfx"] = parser.getArg(1);
	}
	else if (parser.is("INSTRUCTIONS", 1)) {
		j_obj["instructions"] = parser.getArg(1);
	}
	else if (parser.is("DISCLAIMER", 2)) {
		j_obj["disclaimer"]["en"] = parser.getArg(1);
		j_obj["disclaimer"]["ja"] = parser.getArg(2);
	}
	else if (parser.is("SHADER_ERROR", 1)) {
		j_obj["shader_error"] = parser.getArg(1);
	}
	else {
		Scene::parseLine();
	}
}

ubu::Title::Title(const std::string& file)
	: Scene(file), themeIndex(0)
{}

void ubu::Title::parseLine() {
	int argc = parser.getArgC();
	if (parser.is("MENU", 3)) {
		j_obj["menu"]["font"] = parser.getArg(1);
		j_obj["menu"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["menu"]["pos"]["y"] = parser.getArgInt(3);
		if (argc > 4) {
			j_obj["menu"]["dx"] = parser.getArgInt(4);
		}
	}
	else if (parser.is("INACTIVE", 3)) {
		j_obj["inactive"]["r"] = parser.getArgInt(1);
		j_obj["inactive"]["g"] = parser.getArgInt(2);
		j_obj["inactive"]["b"] = parser.getArgInt(3);
	}
	else if (parser.is("ACTIVE", 3)) {
		j_obj["active"]["r"] = parser.getArgInt(1);
		j_obj["active"]["g"] = parser.getArgInt(2);
		j_obj["active"]["b"] = parser.getArgInt(3);
		if (argc >= 4) {
			j_obj["active"]["dx"] = parser.getArgInt(4);
		}
	}
	else if (parser.is("THEMES", 1)) {
		//j_obj["themeCount"] parser.getArgInt(1);
	}
	else if (parser.is("THEME", 1)) {
		j_obj["theme"][themeIndex] = parser.getArg(1);
		themeIndex++;
	}
	else {
		Scene::parseLine();
	}
}

ubu::Select::Select(const std::string& file)
	: Scene(file), gridC(0), selectIndex(0)
{}

void ubu::Select::parseLine() {
	if (parser.is("GRID", 4)) {
		j_obj["grid"]["w"] = parser.getArgInt(1);
		j_obj["grid"]["h"] = parser.getArgInt(2);
		j_obj["grid"]["w_group"] = parser.getArgInt(3);
		j_obj["grid"]["h_group"] = parser.getArgInt(4);
	}
	else if (parser.is("CURSOR", 10)) {
		int group = parser.getArgInt(1) - 1;
		j_obj["cursor"][group]["image"] = parser.getArg(2);
		j_obj["cursor"][group]["offset"]["x"] = parser.getArgInt(3);
		j_obj["cursor"][group]["offset"]["y"] = parser.getArgInt(4);
		j_obj["cursor"][group]["effect"]["image"] = parser.getArg(5);
		j_obj["cursor"][group]["effect"]["frameCount"] = parser.getArgInt(6);
		j_obj["cursor"][group]["effect"]["speed"] = parser.getArgInt(7);
		j_obj["cursor"][group]["effect"]["grow"] = parser.getArgBool(8, false);
		j_obj["cursor"][group]["sound"]["select"] = parser.getArg(9);
		j_obj["cursor"][group]["sound"]["deselect"] = parser.getArg(10);
	}
	else if (parser.is("CHAR", 1)) {
		if (!parser.getArg(1).compare("null")) {
			j_obj["char"][gridC] = nullptr;
			gridC++;
			return;
		}
		j_obj["char"][gridC]["name"] = parser.getArg(1);
		j_obj["char"][gridC]["pos"]["x"] = parser.getArgInt(2);
		j_obj["char"][gridC]["pos"]["y"] = parser.getArgInt(3);
		gridC++;
	}
	else if (parser.is("SELECT", 3)) {
		int argc = parser.getArgC();
		j_obj["select"][selectIndex]["image"] = parser.getArg(1);
		j_obj["select"][selectIndex]["pos"]["x"] = parser.getArgFloat(2);
		j_obj["select"][selectIndex]["pos"]["y"] = parser.getArgFloat(3);
		if (argc > 4) {
			j_obj["select"][selectIndex]["renderType"] = parser.getArg(4);
			if (argc > 5) {
				j_obj["select"][selectIndex]["vel"]["x"] = parser.getArgFloat(5);
				if (argc > 6) {
					j_obj["select"][selectIndex]["vel"]["y"] = parser.getArgFloat(6);
					if (argc > 7) {
						j_obj["select"][selectIndex]["wrap"] = parser.getArgBool(7, false);
					}
				}
			}
		}
		selectIndex++;
	}
	else if (parser.is("STAGES", 1)) {
		j_obj["stages"]["font"] = parser.getArg(1);
	}
	else if (parser.is("PLAYER", 6)) {
		int playerIndex = parser.getArgInt(1) - 1;
		j_obj["player"][playerIndex]["posDefault"]["x"] = parser.getArgInt(2);
		j_obj["player"][playerIndex]["posDefault"]["y"] = parser.getArgInt(3);
		j_obj["player"][playerIndex]["color"]["r"] = parser.getArgInt(4);
		j_obj["player"][playerIndex]["color"]["g"] = parser.getArgInt(5);
		j_obj["player"][playerIndex]["color"]["b"] = parser.getArgInt(6);
	}
	else {
		Scene::parseLine();
	}
}

ubu::Options::Options(const std::string& file)
	: Scene(file), themeIndex(0)
{}

void ubu::Options::parseLine() {
	if (parser.is("FONT", 1)) {
		j_obj["font"] = parser.getArg(1);
	}
	else if (parser.is("MADOTSUKI", 3)) {
		j_obj["madotsuki"]["image"] = parser.getArg(1);
		j_obj["madotsuki"]["sfx"]["step"] = parser.getArg(2);
		j_obj["madotsuki"]["sfx"]["pinch"] = parser.getArg(3);
	}
	else if (parser.is("INACTIVE", 3)) {
		j_obj["inactive"]["r"] = parser.getArgInt(1);
		j_obj["inactive"]["g"] = parser.getArgInt(2);
		j_obj["inactive"]["b"] = parser.getArgInt(3);
	}
	else if (parser.is("ACTIVE", 3)) {
		int argc = parser.getArgC();
		j_obj["active"]["r"] = parser.getArgInt(1);
		j_obj["active"]["g"] = parser.getArgInt(2);
		j_obj["active"]["b"] = parser.getArgInt(3);
		if (argc >= 4) {
			j_obj["active"]["dx"] = parser.getArgInt(4);
		}
	}
	else if (parser.is("THEMES", 1)) {
		//j_obj["themeCount"] parser.getArgInt(1);
	}
	else if (parser.is("THEME", 1)) {
		j_obj["theme"][themeIndex] = parser.getArg(1);
		themeIndex++;
	}
	else if (parser.is("VOICES", 2)) {
		j_obj["voices"]["dame"] = parser.getArg(1);
		j_obj["voices"]["muri"] = parser.getArg(2);
	}
	else {
		Scene::parseLine();
	}
}

ubu::Fight::Fight(const std::string& file)
	: Scene(file)
{}

void ubu::Fight::parseLine() {
	if (parser.is("HUD", 1)) {
		j_obj["hud"]["image"] = parser.getArg(1);
	}
	else if (parser.is("HUD_TAG", 1)) {
		j_obj["hud_tag"]["image"] = parser.getArg(1);
	}
	else if (parser.is("PORTRAITS", 1)) {
		j_obj["portraits"]["image"] = parser.getArg(1);
	}
	else if (parser.is("PORTRAITS_TAG", 1)) {
		j_obj["portraits_tag"]["image"] = parser.getArg(1);
	}
	else if (parser.is("TIMER", 2)) {
		j_obj["timer"]["image"] = parser.getArg(1);
		j_obj["timer"]["font"] = parser.getArg(2);
	}
	else if (parser.is("SHINE", 1)) {
		j_obj["shine"]["image"] = parser.getArg(1);
	}
	else if (parser.is("SHINE_TAG", 1)) {
		j_obj["shine_tag"]["image"] = parser.getArg(1);
	}
	else if (parser.is("HP", 3)) {
		j_obj["hp"]["image"] = parser.getArg(1);
		j_obj["hp"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["hp"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("SUPER", 3)) {
		j_obj["super"]["image"] = parser.getArg(1);
		j_obj["super"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["super"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("TAG", 3)) {
		j_obj["tag"]["image"] = parser.getArg(1);
		j_obj["tag"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["tag"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("STUN", 3)) {
		j_obj["stun"]["image"] = parser.getArg(1);
		j_obj["stun"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["stun"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("GUARD", 3)) {
		j_obj["guard"]["image"] = parser.getArg(1);
		j_obj["guard"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["guard"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("DPM", 3)) {
		j_obj["dpm"]["image"] = parser.getArg(1);
		j_obj["dpm"]["pos"]["x"] = parser.getArgInt(2);
		j_obj["dpm"]["pos"]["y"] = parser.getArgInt(3);
	}
	else if (parser.is("STATIC", 2)) {
		j_obj["static"]["image"] = parser.getArg(1);
		j_obj["static"]["sound"] = parser.getArg(2);
	}
	else if (parser.is("FADE", 2)) {
		j_obj["fade"]["sound"]["in"] = parser.getArg(1);
		j_obj["fade"]["sound"]["out"] = parser.getArg(2);
	}
	else if (parser.is("ROUND_SPLASH", 5)) {
		for (int i = 0; i < 5; i++) {
			j_obj["round_splash"]["image"][i] = parser.getArg(i + 1);
		}
	}
	else if (parser.is("ROUND_HUD", 7)) {
		for (int i = 0; i < 5; i++) {
			j_obj["round_hud"]["image"][i] = parser.getArg(i + 1);
		}
		j_obj["round_hud"]["pos"]["x"] = parser.getArgInt(6);
		j_obj["round_hud"]["pos"]["y"] = parser.getArgInt(7);
	}
	else if (parser.is("KO", 3)) {
		//for (int i = 0; i < 3; i++) {
		//	  j_obj["ko"]["image"][i] = parser.getArg(i + 1);
		//}
		j_obj["ko"]["image"]["ko"] = parser.getArg(1);
		j_obj["ko"]["image"]["timeout"] = parser.getArg(2);
		j_obj["ko"]["image"]["draw"] = parser.getArg(3);
	}
	else if (parser.is("PORTRAIT_POS", 2)) {
		j_obj["portraitPos"]["x"] = parser.getArgInt(1);
		j_obj["portraitPos"]["y"] = parser.getArgInt(2);
	}
	else if (parser.is("COMBO", 3)) {
		j_obj["combo"]["font"] = parser.getArg(1);
		j_obj["combo"]["image"]["left"] = parser.getArg(2);
		j_obj["combo"]["image"]["right"] = parser.getArg(3);
	}
	else if (parser.is("WIN", 3)) {
		j_obj["win"]["image"] = parser.getArg(1);
		j_obj["win"]["font"] = parser.getArg(2);
		j_obj["win"]["bgm"] = parser.getArg(3);
	}
	else if (parser.is("WIN_ORBS", 5)) {
		j_obj["win_orbs"]["image"]["null"] = parser.getArg(1);
		j_obj["win_orbs"]["image"]["win"] = parser.getArg(2);
		j_obj["win_orbs"]["image"]["draw"] = parser.getArg(3);
		j_obj["win_orbs"]["pos"]["x"] = parser.getArgInt(4);
		j_obj["win_orbs"]["pos"]["y"] = parser.getArgInt(5);
	}
	else {
		Scene::parseLine();
	}
}

ubu::Netplay::Netplay(const std::string& file)
	: Scene(file)
{}

void ubu::Netplay::parseLine() {
	if (parser.is("BGM", 2)) { //Override superclass
		j_obj["bgm"]["idle"] = parser.getArg(1);
		j_obj["bgm"]["wait"] = parser.getArg(2);
	}
	else if (parser.is("SFX_CONNECT", 2)) {
		j_obj["sfx_connect"]["start"] = parser.getArg(1);
		j_obj["sfx_connect"]["success"] = parser.getArg(2);
	}
	else if (parser.is("LOGO", 1)) {
		j_obj["logo"] = parser.getArg(1);
	}
	else if (parser.is("SCANLINES", 2)) {
		j_obj["scanlines"]["image"]["scanlines"] = parser.getArg(1);
		j_obj["scanlines"]["image"]["static"] = parser.getArg(2);
	}
	else if (parser.is("MENU", 2)) {
		j_obj["menu"]["font"] = parser.getArg(1);
		j_obj["menu"]["cursorImage"] = parser.getArg(2);
	}
	else if (parser.is("SFX_TV", 2)) {
		j_obj["sfx_tv"]["on"] = parser.getArg(1);
		j_obj["sfx_tv"]["off"] = parser.getArg(2);
	}
	else {
		Scene::parseLine();
	}
}

ubu::Credits::Credits(const std::string& file)
	: Scene(file)
{}

void ubu::Credits::parseLine() {
	if (parser.is("LOGO", 1)) {
		j_obj["logo"] = parser.getArg(1);
	}
	else if (parser.is("CREDITS", 2)) {
		j_obj["credits"]["font"] = parser.getArg(1);
		j_obj["credits"]["file"] = parser.getArg(2);
	}
	else if (parser.is("COLOR", 6)) {
		j_obj["color"]["title"]["r"] = parser.getArgInt(1);
		j_obj["color"]["title"]["g"] = parser.getArgInt(2);
		j_obj["color"]["title"]["b"] = parser.getArgInt(3);
		j_obj["color"]["name"]["r"] = parser.getArgInt(4);
		j_obj["color"]["name"]["g"] = parser.getArgInt(5);
		j_obj["color"]["name"]["b"] = parser.getArgInt(6);
	}
	else {
		Scene::parseLine();
	}
}
