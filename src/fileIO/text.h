#ifndef FILEIO_TEXT_H
#define FILEIO_TEXT_H

#include <string>
#include <vector>

namespace fileIO {
	std::string readText(const std::string& file);

	std::vector<std::string> readTextAsLines(const std::string& file, bool skipEmpty = true);
}

#endif // FILEIO_TEXT_H
