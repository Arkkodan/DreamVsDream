#include "json.h"

nlohmann::ordered_json fileIO::readJSON(const std::string& file) {
	auto text = readText(file);
	if (!nlohmann::ordered_json::accept(text)) {
		return nullptr;
	}
	return nlohmann::ordered_json::parse(text);
}
