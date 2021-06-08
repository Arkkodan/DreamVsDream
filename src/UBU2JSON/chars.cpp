#include "chars.h"

ubu::Character::Character(const std::string& file)
	: UBU(file)
{}

bool ubu::Character::parse() {
	if (!parser.exists()) {
		return false;
	}

	while (parser.parseLine()) {
		if (parser.is("NAME", 1)) {
			j_obj["name"] = parser.getArg(1);
		}
		else if (parser.is("GROUP", 1)) {
			j_obj["group"] = parser.getArgInt(1);
		}
		else if (parser.is("DEFENSE", 1)) {
			j_obj["defense"] = parser.getArgFloat(1);
		}
		else if (parser.is("HEIGHT", 1)) {
			j_obj["height"] = parser.getArgInt(1);
		}
		else if (parser.is("WIDTH", 2)) {
			j_obj["width"] = {
				{"left", parser.getArgInt(1)},
				{"right", parser.getArgInt(2)},
			};
		}
		else if (parser.is("GRAVITY", 1)) {
			j_obj["gravity"] = parser.getArgFloat(1);
		}
		else if (parser.is("PALETTES", 1)) {
			j_obj["palettes"] = parser.getArgInt(1);
		}
	}

	return true;
}

ubu::Sprites::Sprites(const std::string& file)
	: UBU(file)
{}

bool ubu::Sprites::parse() {
	if (!parser.exists()) {
		return false;
	}

	int spriteIndex = -1;
	std::string name = "";
	int hitBoxIndex = 0;
	int aHitBoxIndex = 0;
	int currentHitBoxCount = 0;
	int currentAHitBoxCount = 0;

	while (parser.parseLine()) {
		int argc = parser.getArgC();

		if (parser.isGroup()) {
			spriteIndex++;
			j_obj[spriteIndex]["image"] = parser.getArg(0);

			hitBoxIndex = 0;
			aHitBoxIndex = 0;
			currentHitBoxCount = 0;
			currentAHitBoxCount = 0;

			if (argc == 5) {
				j_obj[spriteIndex]["pos"]["x"] = parser.getArgInt(1);
				j_obj[spriteIndex]["pos"]["y"] = parser.getArgInt(2);

				currentHitBoxCount = parser.getArgInt(3);
				currentAHitBoxCount = parser.getArgInt(4);
			}
			else {
				j_obj[spriteIndex]["pos"]["x"] = 0;
				j_obj[spriteIndex]["pos"]["y"] = 0;
			}
		}
		else {
			if (argc == 4) {
				if (hitBoxIndex < currentHitBoxCount) {
					//j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["pos"]["x"] = parser.getArgInt(0);
					//j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["pos"]["y"] = parser.getArgInt(1);
					//j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["size"]["x"] = parser.getArgInt(2);
					//j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["size"]["y"] = parser.getArgInt(3);
					j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["x"] = parser.getArgInt(0);
					j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["y"] = parser.getArgInt(1);
					j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["w"] = parser.getArgInt(2);
					j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["h"] = parser.getArgInt(3);
					hitBoxIndex++;
				}
				else if (aHitBoxIndex < currentAHitBoxCount) {
					//j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["pos"]["x"] = parser.getArgInt(0);
					//j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["pos"]["y"] = parser.getArgInt(1);
					//j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["size"]["x"] = parser.getArgInt(2);
					//j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["size"]["y"] = parser.getArgInt(3);
					j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["x"] = parser.getArgInt(0);
					j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["y"] = parser.getArgInt(1);
					j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["w"] = parser.getArgInt(2);
					j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["h"] = parser.getArgInt(3);
					aHitBoxIndex++;
				}
			}
		}
	}

	return true;
}

ubu::Sounds::Sounds(const std::string& file)
	: UBU(file)
{}

bool ubu::Sounds::parse() {
	if (!parser.exists()) {
		return false;
	}

	int soundslistIndex = -1;
	std::string name = "";

	while (parser.parseLine()) {
		if (parser.isGroup()) {
			soundslistIndex++;
			j_obj[soundslistIndex]["name"] = parser.getArg(0);
		}
		else {
			j_obj[soundslistIndex]["sounds"].emplace_back(parser.getArg(0));
		}
	}

	return true;
}

ubu::Voices::Voices(const std::string& file)
	: UBU(file)
{}

bool ubu::Voices::parse() {
	if (!parser.exists()) {
		return false;
	}

	int voiceslistIndex = -1;
	std::string name = "";

	while (parser.parseLine()) {
		if (parser.isGroup()) {
			voiceslistIndex++;
			j_obj[voiceslistIndex]["name"] = parser.getArg(0);
			j_obj[voiceslistIndex]["percent"] = parser.getArgInt(1);
		}
		else {
			//j_obj[voiceslistIndex]["voices"].emplace_back(parser.getArg(0));
			j_obj[voiceslistIndex]["sounds"].emplace_back(parser.getArg(0));
		}
	}

	return true;
}
