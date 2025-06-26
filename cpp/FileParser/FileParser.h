#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>
#include <vector>

class FileParser {
public:
    FileParser(const std::string& filePath);
    bool loadFile();
    std::vector<std::string> getLines() const;
    std::vector<std::string> split(const std::string& line, char delimiter = ',') const;

private:
    std::string filePath_;
    std::vector<std::string> lines_;
};

#endif // FILE_PARSER_H
