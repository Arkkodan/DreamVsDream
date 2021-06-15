#include "text.h"

#include <fstream>
#include <sstream>

std::string fileIO::readText(const std::string &file) {
  std::ifstream ifs(file);
  if (!ifs) {
    // Maybe add error-handling?
    return std::string{};
  }
  std::stringstream ss;
  ss << ifs.rdbuf();
  ifs.close();
  return ss.str();
}

std::vector<std::string> fileIO::readTextAsLines(const std::string &file,
                                                 bool skipEmpty) {
  std::ifstream ifs(file);
  if (!ifs) {
    return std::vector<std::string>{};
  }
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(ifs, line)) {
    if (skipEmpty && line.empty()) {
      continue;
    }
    lines.emplace_back(line);
  }
  ifs.close();
  return lines;
}
