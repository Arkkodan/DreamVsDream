#include "json.h"

nlohmann::ordered_json fileIO::readJSON(const std::string& file) {
	auto text = readText(file);
	if (text.empty()) {
		return nullptr;
	}
	return nlohmann::ordered_json::parse(text);
}
