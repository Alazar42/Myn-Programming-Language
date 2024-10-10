#include <iostream>
#include <string>
#include "lexer.hpp"
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <parser.hpp>

namespace fs = std::filesystem;

std::unordered_set<std::string> reservedKeywordsForConfig = {
  "myn", "for", "while", "switch", "fun", "class", "break", "case", "true", "false", 
  "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch", 
  "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return", 
  "input", "output", "int", "float", "string", "bool", "start"
};


// Function to read and validate myn.config file
std::unordered_map<std::string, std::string> readConfigFile(const std::string& configFilePath) {
    std::unordered_map<std::string, std::string> config;

    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open the config file " << configFilePath << std::endl;
        std::exit(0);
        return config;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Skip empty lines or lines starting with a comment
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream lineStream(line);
        std::string key, value;
        if (std::getline(lineStream, key, '=')) {
            // Trim leading and trailing whitespace from key and value
            key = key.substr(key.find_first_not_of(" \t")); // Trim leading whitespace
            key = key.substr(0, key.find_last_not_of(" \t") + 1); // Trim trailing whitespace

            if (std::getline(lineStream, value)) {
                // Trim leading and trailing whitespace from value
                value = value.substr(value.find_first_not_of(" \t")); // Trim leading whitespace
                value = value.substr(0, value.find_last_not_of(" \t") + 1); // Trim trailing whitespace

                // Check if the key is in the set of reserved keywords
                if (reservedKeywordsForConfig.find(key) != reservedKeywordsForConfig.end()) {
                    config[key] = value;
                } else {
                    std::cerr << "Error: Invalid key found in config file: " << key << std::endl;
                    configFile.close();
                    std::exit(0);
                    return {};
                }
            } else {
                std::cerr << "Error: Invalid value line in config file: " << line << std::endl;
                configFile.close();
                std::exit(0);
                return {};
            }
        } else {
            std::cerr << "Error: Invalid key line in config file: " << line << std::endl;
            configFile.close();
            std::exit(0);
            return {};
        }
    }

    configFile.close();
    return config;
}


// Function to check if a string ends with a given suffix
bool hasValidExtension(const std::string &filename) {
    std::filesystem::path filePath(filename);
    std::string extension = filePath.extension().string();
    return extension == ".myn" || extension == ".MYN";
}

std::string getAbsolutePath(const std::string &filename) {
    if (!fs::exists(filename)) {
        std::cout << "Error: File Not Found \"" << filename << "\"" << std::endl;
        exit(0);
    }

    try {
        return fs::canonical(filename).string();
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

bool searchForConfigFile(const std::string &filename) {
    // Convert the given filename to an absolute path
    std::string absPath = getAbsolutePath(filename);
    fs::path filePath(absPath);
    fs::path directory = filePath.parent_path();

    // Check if the directory exists
    if (!fs::exists(directory)) {
        std::cerr << "Directory does not exist: " << directory << std::endl;
        return false;
    }

    // Define the target config file name
    fs::path configFile("myn.config");

    try {
        // Iterate through the directory to find the config file
        for (const auto &entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename() == configFile) {
                return true;
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
    return false;
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

     // Now create a parser instance
    Parser parser(tokens);
    try {
        parser.parse(); // Start parsing the tokens
    } catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }
}

void handleFileWithConfig(const std::string &filename, std::unordered_map<std::string, std::string> config) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    file.close();

    std::vector<Token> tokens = tokenize_with_config(fileContent, config);

    for (const Token& token : tokens) {
        std::cout << "Token: " << token.value << " => Type: " << static_cast<int>(token.type) << std::endl;
    }

     // Now create a parser instance
    Parser parser(tokens);
    try {
        parser.parse(); // Start parsing the tokens
    } catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
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
                // Search for the config file in the directory of the current file
                if (searchForConfigFile(filename)) {
                    std::cout << "Using configurations from myn.config file." << std::endl;
                    std::unordered_map<std::string, std::string> config = readConfigFile("myn.config");
                    
                    handleFileWithConfig(filename, config);
                    
                // Printing the contents of 'config' map
                for (const auto& pair : config) {
                    std::cout << pair.first << " = " << pair.second << std::endl;
                }
                } else {
                    std::cout << "No myn.config file found. Proceeding with default configurations." << std::endl;
                    handleFile(filename);
                }
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
