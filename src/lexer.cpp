#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <cctype>
#include "../include/lexer.h"


typedef std::map<std::string, TokenType> ReservedIdentMap;

// Reserved identifiers map
ReservedIdentMap reservedIdent;

void INIT_RESERVED_IDENTIFIER() {
    reservedIdent["myn"] = TokenType::Myn;
    reservedIdent["for"] = TokenType::ForKeyword;
    reservedIdent["while"] = TokenType::WhileKeyword;
    reservedIdent["switch"] = TokenType::SwitchKeyword;
    reservedIdent["class"] = TokenType::ClassKeyword;
    reservedIdent["fun"] = TokenType::FunctionKeyword;
    reservedIdent["in"] = TokenType::In;
    reservedIdent["out"] = TokenType::Out;
}

bool isWhiteSpace(char c) {
    return (c == ' ') || (c == '\t') || (c == '\f') || (c == '\v');
}

bool isEndOfLine(char c) {
    return (c == '\n') || (c == '\r') || (c == ';');
}

bool isLetter(char c) {
    return std::isalpha(c);
}

bool isNumeric(char c) {
    return std::isdigit(c);
}

bool isSkippable(char c) {
    return std::isspace(c);
}

std::vector<std::string> splitString(const std::string &sourceCode) {
    std::vector<std::string> words;
    std::string word;
    bool inQuote = false;

    for (char ch : sourceCode) {
        if (ch == '\'' || ch == '"') {
            inQuote = !inQuote;
            if (!inQuote && !word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else if (!inQuote && ch == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += ch;
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

std::string shift(std::vector<std::string> &src) {
    std::string current = src.front();
    src.erase(src.begin());
    return current;
}

Token createToken(const std::string &value, TokenType tokentype) {
    return {value, tokentype};
}

std::vector<Token> tokenize(const std::string &sourceCode) {
    std::vector<Token> tokens;
    std::vector<std::string> src = splitString(sourceCode);
    int currentPosition = 0;

    while (!src.empty()) {
        if (src.front() == "(") {
            tokens.push_back(createToken(shift(src), TokenType::OpenParen));
        } else if (src.front() == ")") {
            tokens.push_back(createToken(shift(src), TokenType::CloseParen));
        } else if (src.front() == "[") {
            tokens.push_back(createToken(shift(src), TokenType::OpenSBracket));
        } else if (src.front() == "]") {
            tokens.push_back(createToken(shift(src), TokenType::CloseSBracket));
        } else if (src.front() == "{") {
            tokens.push_back(createToken(shift(src), TokenType::OpenBrace));
        } else if (src.front() == "}") {
            tokens.push_back(createToken(shift(src), TokenType::CloseBrace));
        } else if (src.front() == "+" || src.front() == "-" || src.front() == "*" || src.front() == "/") {
            tokens.push_back(createToken(shift(src), TokenType::BinaryOperator));
        } else if (src.front() == "=") {
            if (currentPosition == 0) {
                std::cerr << "Error: '=' cannot appear at the beginning of the expression." << std::endl;
                return tokens;
            }

            if (src.size() > 1) {
                const std::string &nextToken = src[1];
                if (isNumeric(nextToken[0]) || isLetter(nextToken[0]) || nextToken == "(") {
                    tokens.push_back(createToken(shift(src), TokenType::Equals));
                } else {
                    std::cerr << "Error: Invalid expression after '=' at position " << currentPosition << "." << std::endl;
                    return tokens;
                }
            } else {
                std::cerr << "Error: '=' must be followed by an expression at position " << currentPosition << "." << std::endl;
                return tokens;
            }
        } else if (src.front() == "and" || src.front() == "or") {
            tokens.push_back(createToken(shift(src), TokenType::LogicalOperator));
        } else if (src.front() == "\"" || src.front() == "'") {
            std::string quote = shift(src);
            std::string str;
            str += quote;
            while (!src.empty() && src.front() != quote) {
                str += shift(src);
            }
            if (!src.empty()) {
                str += shift(src);
            }
            tokens.push_back(createToken(str, TokenType::String));
        } else {
            if (isNumeric(src.front()[0])) {
                std::string number;
                while (!src.empty() && isNumeric(src.front()[0])) {
                    number += shift(src);
                }
                tokens.push_back(createToken(number, TokenType::Number));
            } else if (isLetter(src.front()[0])) {
                std::string ident = shift(src);
                ReservedIdentMap::iterator it = reservedIdent.find(ident);
                if (it != reservedIdent.end()) {
                    tokens.push_back(createToken(ident, it->second));
                } else {
                    tokens.push_back(createToken(ident, TokenType::Identifier));
                }
            } else if (isSkippable(src.front()[0])) {
                shift(src);
            } else {
                std::cerr << "Error: Unrecognized character '" << src.front() << "' found at position " << currentPosition << "." << std::endl;
                return tokens;
            }
        }

        currentPosition++;
    }

    return tokens;
}

void debugPrintTokens(const std::vector<Token> &tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": " << tokens[i].value << " - " << static_cast<int>(tokens[i].type) << std::endl;
    }
}

int main() {
    INIT_RESERVED_IDENTIFIER();
    
    std::string code = R"(myn while (x < 10) { x = x + 1; })";
    std::vector<Token> tokens = tokenize(code);

    debugPrintTokens(tokens);

    return 0;
}