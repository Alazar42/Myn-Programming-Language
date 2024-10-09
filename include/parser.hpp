#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "lexer.hpp"
#include <cassert>  

#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#define LOG_WARNING(msg) std::cerr << "[WARNING] " << msg << std::endl
#define LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl

class Parser
{
public:
    Parser(const std::vector<Token> &tokens);
    void parse();

private:
    const std::vector<Token> &tokens;
    size_t currentIndex;

    void parseStatement();
    Token currentToken() const;
    void advance();
    void handleKeyword(const Token &token);

public:
    void parseVariableDeclaration();
    void parseFunctionDefinition();
    void parseWhileLoop();
    void parseForLoop();
    void parseFunctionCall();
    void parseOutputStatement();
    void ParsingError(std::string error);
    bool hasMoreTokens();
};

#endif // PARSER_H