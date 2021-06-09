#include "resource_manager.h"

#include "../fileIO/json.h"
#include "../util/fileIO.h"
#include "error.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "effect.h"

namespace resource_manager {
	static std::vector<std::string> effectNames;
	static std::unordered_map<std::string, std::unique_ptr<effect::EffectAnimation>> effectsMap;
}

template<>
void resource_manager::load<effect::EffectAnimation>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (effectsMap.find(resource) != effectsMap.cend()) {
			return;
		}
	}
	effectsMap[resource] = std::make_unique<effect::EffectAnimation>(resource);
}

template<>
void resource_manager::unload<effect::EffectAnimation>(const std::string& resource) {
	effectsMap.erase(resource);
}

template<>
effect::EffectAnimation* resource_manager::getResource<effect::EffectAnimation>(const std::string& resource) {
	if (effectsMap.find(resource) == effectsMap.cend()) {
		load<effect::EffectAnimation>(resource, true);
	}
	return effectsMap[resource].get();
}

template<>
void resource_manager::loadAll<effect::EffectAnimation>() {
	auto j_effects = fileIO::readJSON(util::getPath("effects/effects.json"));
	if (!j_effects.is_array()) {
		error::die("Cannot parse effects.json");
	}

	effectNames.reserve(j_effects.size());
	effectsMap.reserve(j_effects.size());

	for (const auto& j_effect : j_effects) {
		if (j_effect.is_string()) {
			if (std::find(effectNames.cbegin(), effectNames.cend(), j_effect) == effectNames.cend()) {
				effectNames.emplace_back(j_effect);
			}
			load<effect::EffectAnimation>(j_effect);
		}
	}
}

template<>
std::vector<effect::EffectAnimation*> resource_manager::getAll<effect::EffectAnimation>() {
	std::vector<effect::EffectAnimation*> effects;
	effects.reserve(effectNames.size());

	for (const auto& effectName : effectNames) {
		effects.push_back(effectsMap[effectName].get());
	}

	return effects;
}


#include "fighter.h"

namespace resource_manager {
	static std::vector<std::string> fighterNames;
	static std::unordered_map<std::string, std::unique_ptr<game::Fighter>> fightersMap;
}

template<>
void resource_manager::load<game::Fighter>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (fightersMap.find(resource) != fightersMap.cend()) {
			return;
		}
	}
	fightersMap[resource] = std::make_unique<game::Fighter>();
	fightersMap[resource]->create(resource);
}

template<>
void resource_manager::unload<game::Fighter>(const std::string& resource) {
	fightersMap.erase(resource);
}

template<>
game::Fighter* resource_manager::getResource<game::Fighter>(const std::string& resource) {
	if (fightersMap.find(resource) == fightersMap.cend()) {
		load<game::Fighter>(resource, true);
	}
	return fightersMap[resource].get();
}

template<>
void resource_manager::loadAll<game::Fighter>() {
	auto j_fighters = fileIO::readJSON(util::getPath("chars/chars.json"));
	if (!j_fighters.is_array()) {
		error::die("Cannot parse chars.json");
	}

	fighterNames.reserve(j_fighters.size());
	fightersMap.reserve(j_fighters.size());

	for (const auto& j_fighter : j_fighters) {
		if (j_fighter.is_string()) {
			if (std::find(fighterNames.cbegin(), fighterNames.cend(), j_fighter) == fighterNames.cend()) {
				fighterNames.emplace_back(j_fighter);
			}
			load<game::Fighter>(j_fighter);
		}
	}
}

template<>
std::vector<game::Fighter*> resource_manager::getAll<game::Fighter>() {
	std::vector<game::Fighter*> fighters;
	fighters.reserve(fighterNames.size());

	for (const auto& fighterName : fighterNames) {
		fighters.push_back(fightersMap[fighterName].get());
	}

	return fighters;
}


#include "stage.h"

namespace resource_manager {
	static std::vector<std::string> stageNames;
	static std::unordered_map<std::string, std::unique_ptr<Stage>> stagesMap;
}

template<>
void resource_manager::load<Stage>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (stagesMap.find(resource) != stagesMap.cend()) {
			return;
		}
	}
	stagesMap[resource] = std::make_unique<Stage>();
	stagesMap[resource]->create(resource);
}

template<>
void resource_manager::unload<Stage>(const std::string& resource) {
	stagesMap.erase(resource);
}

template<>
Stage* resource_manager::getResource<Stage>(const std::string& resource) {
	if (stagesMap.find(resource) == stagesMap.cend()) {
		load<Stage>(resource, true);
	}
	return stagesMap[resource].get();
}

template<>
void resource_manager::loadAll<Stage>() {
	auto j_stages = fileIO::readJSON(util::getPath("stages/stages.json"));
	if (!j_stages.is_array()) {
		error::die("Cannot parse stages.json");
	}

	int size = j_stages.size();
	stageNames.resize(size);
	stagesMap.reserve(size);

	for (int i = 0; i < size; i++) {
		const auto& j_stage = j_stages[i];
		if (j_stage.is_string()) {
			if (std::find(stageNames.cbegin(), stageNames.cend(), j_stage) == stageNames.cend()) {
				stageNames[i] = j_stage;
			}
			load<Stage>(j_stage);
		}
	}
}

template<>
std::vector<Stage*> resource_manager::getAll<Stage>() {
	std::vector<Stage*> stages;
	stages.reserve(stageNames.size());

	for (const auto& stageName : stageNames) {
		stages.push_back(stagesMap[stageName].get());
	}

	return stages;
}
