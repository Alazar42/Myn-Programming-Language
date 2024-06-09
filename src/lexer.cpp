#include "../include/lexer.h"
#include <iostream>
#include <sstream>
#include <vector>

void lexer(const std::string &data) {
    std::istringstream stream(data);
    std::string token;
    std::vector<std::string> tokens;

    while (stream >> token) {
        tokens.push_back(token);
    }

    std::cout << "Tokens:" << std::endl;
    for (const auto &t : tokens) {
        std::cout << t << std::endl;
    }
}
