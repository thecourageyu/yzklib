#include "FileParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

FileParser::FileParser(const std::string& filePath)
    : filePath_(filePath) {}

bool FileParser::loadFile() {
    std::ifstream file(filePath_);
    if (!file.is_open()) {
        std::cerr << "無法打開檔案: " << filePath_ << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines_.push_back(line);
    }

    file.close();
    return true;
}

std::vector<std::string> FileParser::getLines() const {
    return lines_;
}

std::vector<std::string> FileParser::split(const std::string& line, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
