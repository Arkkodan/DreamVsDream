#include "resource_manager.h"

#include "../fileIO/json.h"
#include "../util/fileIO.h"
#include "error.h"
#include "parser.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "effect.h"

namespace resource_manager {
	static std::vector<std::string> effectsManifest;
	static std::unordered_map<std::string, std::unique_ptr<effect::EffectAnimation>> effectsMap;
}

template<>
bool resource_manager::load<effect::EffectAnimation>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (effectsMap.find(resource) != effectsMap.cend()) {
			return false;
		}
	}
	// Unsafe: What if stage does not exist?
	effectsMap[resource] = std::make_unique<effect::EffectAnimation>(resource);
	return true;
}

template<>
void resource_manager::unload<effect::EffectAnimation>(const std::string& resource) {
	effectsMap.erase(resource);
}

template<>
effect::EffectAnimation* resource_manager::getResource<effect::EffectAnimation>(const std::string& resource) {
	if (effectsMap.find(resource) == effectsMap.cend()) {
		if (!load<effect::EffectAnimation>(resource, true)) {
			return nullptr;
		}
	}
	return effectsMap[resource].get();
}

template<>
void resource_manager::loadFromManifest<effect::EffectAnimation>() {
	auto j_effects = fileIO::readJSON(util::getPath("effects/effects.json"));
	if (!j_effects.is_array()) {
		error::die("Cannot parse effects.json");
	}

	effectsManifest.reserve(j_effects.size());
	effectsMap.reserve(j_effects.size());

	for (const auto& j_effect : j_effects) {
		if (j_effect.is_string()) {
			if (std::find(effectsManifest.cbegin(), effectsManifest.cend(), j_effect) == effectsManifest.cend()) {
				effectsManifest.emplace_back(j_effect);
			}
			load<effect::EffectAnimation>(j_effect);
		}
	}
}

template<>
std::vector<effect::EffectAnimation*> resource_manager::getFromManifest<effect::EffectAnimation>() {
	std::vector<effect::EffectAnimation*> effects;
	effects.reserve(effectsManifest.size());

	for (const auto& effectName : effectsManifest) {
		effects.push_back(effectsMap[effectName].get());
	}

	return effects;
}


#include "fighter.h"

namespace resource_manager {
	static std::vector<std::string> fightersManifest;
	static std::unordered_map<std::string, std::unique_ptr<game::Fighter>> fightersMap;
}

template<>
bool resource_manager::load<game::Fighter>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (fightersMap.find(resource) != fightersMap.cend()) {
			return false;
		}
	}
	std::string path = util::getPath("chars/" + resource + ".char");
	if (!util::fileExists(path)) {
		return false;
	}
	fightersMap[resource] = std::make_unique<game::Fighter>();
	fightersMap[resource]->create(resource);
	return true;
}

template<>
void resource_manager::unload<game::Fighter>(const std::string& resource) {
	fightersMap.erase(resource);
}

template<>
game::Fighter* resource_manager::getResource<game::Fighter>(const std::string& resource) {
	if (fightersMap.find(resource) == fightersMap.cend()) {
		if (!load<game::Fighter>(resource, true)) {
			return nullptr;
		}
	}
	return fightersMap[resource].get();
}

template<>
void resource_manager::loadFromManifest<game::Fighter>() {
	auto j_fighters = fileIO::readJSON(util::getPath("chars/chars.json"));
	if (!j_fighters.is_array()) {
		error::die("Cannot parse chars.json");
	}

	fightersManifest.reserve(j_fighters.size());
	fightersMap.reserve(j_fighters.size());

	for (const auto& j_fighter : j_fighters) {
		if (j_fighter.is_string()) {
			if (std::find(fightersManifest.cbegin(), fightersManifest.cend(), j_fighter) == fightersManifest.cend()) {
				fightersManifest.emplace_back(j_fighter);
			}
			load<game::Fighter>(j_fighter);
		}
	}
}

template<>
std::vector<game::Fighter*> resource_manager::getFromManifest<game::Fighter>() {
	std::vector<game::Fighter*> fighters;
	fighters.reserve(fightersManifest.size());

	for (const auto& fighterName : fightersManifest) {
		fighters.push_back(fightersMap[fighterName].get());
	}

	return fighters;
}


#include "font.h"

namespace resource_manager {
	// static std::vector<std::string> fontsManifest;
	static std::unordered_map<std::string, std::unique_ptr<Font>> fontsMap;
}

template<>
bool resource_manager::load<Font>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (fontsMap.find(resource) != fontsMap.cend()) {
			return false;
		}
	}
	std::string path = util::getPath("fonts/" + resource + '.' + Parser::EXT_FONT);
	if (!util::fileExists(path)) {
		return false;
	}
	fontsMap[resource] = std::make_unique<Font>();
	fontsMap[resource]->createFromFile(path);
	return true;
}

template<>
void resource_manager::unload<Font>(const std::string& resource) {
	fontsMap.erase(resource);
}

template<>
Font* resource_manager::getResource<Font>(const std::string& resource) {
	if (fontsMap.find(resource) == fontsMap.cend()) {
		if (!load<Font>(resource, true)) {
			return nullptr;
		}
	}
	return fontsMap[resource].get();
}


#include "sound.h"

namespace resource_manager {
	// static std::vector<std::string> soundsManifest;
	static std::unordered_map<std::string, std::unique_ptr<audio::Sound>> soundsMap;
}

template<>
bool resource_manager::load<audio::Sound>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (soundsMap.find(resource) != soundsMap.cend()) {
			return false;
		}
	}
	std::string path = util::getPath("sounds/" + resource + '.' + Parser::EXT_SOUND);
	if (!util::fileExists(path)) {
		return false;
	}
	soundsMap[resource] = std::make_unique<audio::Sound>();
	soundsMap[resource]->createFromFile(path);
	return true;
}

template<>
void resource_manager::unload<audio::Sound>(const std::string& resource) {
	soundsMap.erase(resource);
}

template<>
audio::Sound* resource_manager::getResource<audio::Sound>(const std::string& resource) {
	if (soundsMap.find(resource) == soundsMap.cend()) {
		if (!load<audio::Sound>(resource, true)) {
			return nullptr;
		}
	}
	return soundsMap[resource].get();
}


#include "stage.h"

namespace resource_manager {
	static std::vector<std::string> stagesManifest;
	static std::unordered_map<std::string, std::unique_ptr<Stage>> stagesMap;
}

template<>
bool resource_manager::load<Stage>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (stagesMap.find(resource) != stagesMap.cend()) {
			return false;
		}
	}
	// Unsafe: What if stage does not exist?
	stagesMap[resource] = std::make_unique<Stage>();
	stagesMap[resource]->create(resource);
	return true;
}

template<>
void resource_manager::unload<Stage>(const std::string& resource) {
	stagesMap.erase(resource);
}

template<>
Stage* resource_manager::getResource<Stage>(const std::string& resource) {
	if (stagesMap.find(resource) == stagesMap.cend()) {
		if (!load<Stage>(resource, true)) {
			return nullptr;
		}
	}
	return stagesMap[resource].get();
}

template<>
void resource_manager::loadFromManifest<Stage>() {
	auto j_stages = fileIO::readJSON(util::getPath("stages/stages.json"));
	if (!j_stages.is_array()) {
		error::die("Cannot parse stages.json");
	}

	int size = j_stages.size();
	stagesManifest.resize(size);
	stagesMap.reserve(size);

	for (int i = 0; i < size; i++) {
		const auto& j_stage = j_stages[i];
		if (j_stage.is_string()) {
			if (std::find(stagesManifest.cbegin(), stagesManifest.cend(), j_stage) == stagesManifest.cend()) {
				stagesManifest[i] = j_stage;
			}
			load<Stage>(j_stage);
		}
	}
}

template<>
std::vector<Stage*> resource_manager::getFromManifest<Stage>() {
	std::vector<Stage*> stages;
	stages.reserve(stagesManifest.size());

	for (const auto& stageName : stagesManifest) {
		stages.push_back(stagesMap[stageName].get());
	}

	return stages;
}
