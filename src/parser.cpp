#include "parser.hpp"
#include <stdexcept>
#include <iostream>

class ParsingException : public std::runtime_error {
public:
    explicit ParsingException(const std::string& message)
        : std::runtime_error(message) {}
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentIndex(0) {}

void Parser::parse() {
    while (currentIndex < tokens.size()) {
        parseStatement();
    }
}

void Parser::parseStatement() {
    Token token = currentToken();
    
    if (token.type == TokenType::ReservedKeyword) {
        if (token.value == "fun") {
            parseFunctionDefinition();
        } else if (token.value == "while") {
            parseWhileLoop();
        } else if (token.value == "for") {
            parseForLoop();
        } else if (token.value == "output") {
            parseOutputStatement();
        } else if (token.value == "int" || token.value == "string" || token.value == "float" || token.value == "bool") {
            parseVariableDeclaration();
        }
        else {
            LOG_ERROR("Unexpected keyword: " + token.value);
            exit(0);
        }
    } else if (token.type == TokenType::Identifier) {
        parseFunctionCall();  // Assume an identifier followed by `(` is a function call
    } else {
        LOG_ERROR("Unexpected token: " + token.value);
        exit(0);
    }
}

void Parser::parseFunctionDefinition() {
    advance();  // Skip `fun`
    Token functionName = currentToken();

    if (functionName.type != TokenType::Identifier) {
        LOG_ERROR("Expected function name after 'fun'");
    }

    advance();  // Skip function name
    if (currentToken().value != "(") {
        LOG_ERROR("Expected '(' after function name");
        exit(0);
    }

    // Skip '(' and parameters parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().value != "{") {
        LOG_ERROR("Expected '{' after function declaration");
        exit(0);
    }

    advance();  // Enter function body
    while (currentToken().value != "}") {
        parseStatement();
    }
    advance();  // Skip `}`
}

void Parser::parseFunctionCall() {
    Token functionName = currentToken();
    advance();  // Skip function name

    if (currentToken().value != "(") {
        LOG_ERROR("Expected '(' for function call");
        exit(0);
    }

    // Skip '(' and arguments parsing (simple for now)
    while (currentToken().type != TokenType::Semicolon) {
        advance();
    }
    advance();  // Skip `)`
}

void Parser::parseWhileLoop() {
    advance();  // Skip `while`
    if (currentToken().type != TokenType::OpenParen) {
        LOG_ERROR("Expected '(' after 'while'");
    }

    // Skip '(' and condition parsing (simple for now)
    while (currentToken().type != TokenType::CloseParen) {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().type != TokenType::OpenBrace) {
        LOG_ERROR("Expected '{' after while condition");
        exit(0);
    }

    advance();  // Enter while loop body
    while (currentToken().type != TokenType::CloseBrace) {
        parseStatement();
    }
    advance();  // Skip `}`
}

void Parser::parseForLoop() {
    advance();  // Skip `for`
    if (currentToken().value != "(") {
        LOG_ERROR("Expected '(' after 'for'");
        exit(0);
    }

    // Skip '(' and loop header parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().value != "{") {
        LOG_ERROR("Expected '{' after for loop header");
        exit(0);
    }

    advance();  // Enter for loop body
    while (currentToken().value != "}") {
        parseStatement();
    }
    advance();  // Skip `}`
}

Token Parser::currentToken() const {
    return tokens[currentIndex];
}

void Parser::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

void Parser::parseVariableDeclaration() {
    if (currentToken().type == TokenType::Type) { // TYPE could be 'int', 'float', 'string', etc.
        std::string typeName = currentToken().value;
        advance(); // Move to the next token

        if (currentToken().type != TokenType::Identifier) {
            LOG_ERROR("Expected variable name after type declaration");
            exit(0);
        }

        std::string variableName = currentToken().value;
        advance(); // Move to the next token

        // Handle the rest of the variable declaration (e.g., assignment)
    }
}

void Parser::parseOutputStatement() {
    // Ensure the current token is a reserved keyword before proceeding
    assert(currentToken().type == TokenType::ReservedKeyword && "Expected reserved keyword 'output'");
    
    if (currentToken().type == TokenType::ReservedKeyword) {
        advance();  // Move to '('
        
        if (currentToken().type == TokenType::OpenParen) {
            advance();  // Move to the string or variable
            
            if (currentToken().type == TokenType::String) {
                std::string outputString = currentToken().value;
                advance();  // Move to ')'
                
                if (currentToken().type == TokenType::CloseParen) {
                    advance();  // Safely advance to the next token
                    
                    // Handle optional semicolon
                    if (!hasMoreTokens()) {
                        LOG_ERROR("Semicolon missing after output statement");
                        exit(0);
                    } else if (currentToken().type == TokenType::Semicolon) {
                        advance();  // Consume the semicolon if present
                    } else {
                        LOG_WARNING("Semicolon missing after output statement");
                        exit(0);
                    }
                    return;
                } else {
                    LOG_ERROR("Expected ')' after string in output statement.");
                    exit(0);
                }
            } else {
                LOG_ERROR("Expected a string in output statement.");
                exit(0);
            }
        } else {
            LOG_ERROR("Expected '(' after output keyword.");
            exit(0);
        }
    } else {
        LOG_ERROR("Unexpected token: " + currentToken().value);
        exit(0);
    }
}

// Function to check if there are more tokens to process
bool Parser::hasMoreTokens() {
    return currentIndex < tokens.size();
}
