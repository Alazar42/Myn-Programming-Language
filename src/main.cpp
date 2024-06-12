#include <iostream>
#include <string>
#include "../include/lexer.h"
#include <filesystem>
#include <fstream>

// Function to check if a string ends with a given suffix
bool hasValidExtension(const std::string &filename) {
    std::filesystem::path filePath(filename);
    std::string extension = filePath.extension().string();
    return extension == ".myn" || extension == ".MYN";
}

void handleFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return;
    }

    // Read the contents of the file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    if (file.bad()) {
        std::cerr << "Error: An I/O error occurred while reading the file " << filename << std::endl;
        file.close();
        return;
    }

    file.close();

    // Tokenize the file content
    std::vector<Token> tokens = tokenize(fileContent);

    // Now you can do something with the tokens, like parsing or interpreting them
    // For now, let's just print them
    for (const Token& token : tokens) {
        std::cout << "Token: " << token.value << " => Type: " << static_cast<int>(token.type) << std::endl;
    }
}

int main(int argc, char const *argv[]) {
    // Check if there is at least one command-line argument
    if (argc < 2) {
        std::cerr << "No arguments provided." << std::endl;
        return 1;
    }

    int fileCount = 0;

    // Using a for loop to iterate through command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];
        
        if (hasValidExtension(filename)) {
            if (fileCount == 0) {
                handleFile(filename);
                fileCount++;
            } else {
                std::cout << "Info: " << filename << " ignored." << std::endl;
            }
        } else {
            std::cout << "Error: " << filename << " has an unrecognized file extension." << std::endl << "Expected: .myn or .MYN" << std::endl;
        }
    }

    return 0;
}
