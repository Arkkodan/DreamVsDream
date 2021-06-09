#include "resource_manager.h"

#include "../fileIO/json.h"
#include "../util/fileIO.h"
#include "error.h"

#include "fighter.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace resource_manager {
	static std::vector<std::string> fighterNames;
	static std::unordered_map<std::string, std::unique_ptr<game::Fighter>> fightersMap;
}

template<>
void resource_manager::load<game::Fighter>(const std::string& resource, bool reloadOK) {
	if (!reloadOK) {
		if (fightersMap.find(resource) != fightersMap.end()) {
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
	if (fightersMap.find(resource) == fightersMap.end()) {
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
