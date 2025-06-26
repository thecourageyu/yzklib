#include <iostream>
#include "FileParser.h"

int main() {
    FileParser parser("data.txt");

    if (!parser.loadFile()) {
        return 1;
    }

    for (const auto& line : parser.getLines()) {
        auto tokens = parser.split(line, ',');
        for (const auto& token : tokens) {
            std::cout << "[" << token << "] ";
        }
        std::cout << std::endl;
    }

    return 0;
}
