#include "text.h"

#include <fstream>
#include <sstream>

std::string fileIO::readText(const std::string &file) {
  std::ifstream ifs(file);
  if (!ifs) {
    throw std::runtime_error("Could not read " + file);
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
    throw std::runtime_error("Could not read " + file);
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

void fileIO::writeTextToFile(const std::string &file, const std::string &text) {
  std::ofstream ofs(file);
  if (!ofs) {
    throw std::runtime_error("Could not write to " + file);
    return;
  }
  ofs << text;
  if (text.back() != '\n') {
    ofs << '\n';
  }
  ofs.close();
}
