#ifndef FILEIO_JSON_H
#define FILEIO_JSON_H

#include "text.h"

#include <nlohmann/json.hpp>

namespace fileIO {
	nlohmann::ordered_json readJSON(const std::string& file);
}

#endif // FILEIO_JSON_H
