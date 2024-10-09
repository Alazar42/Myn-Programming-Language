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
        } 
        else {
            ParsingException("Unexpected keyword: " + token.value);
        }
    } else if (token.type == TokenType::Identifier) {
        parseFunctionCall();  // Assume an identifier followed by `(` is a function call
    } else {
        ParsingException("Unexpected token: " + token.value);
    }
}

void Parser::parseFunctionDefinition() {
    advance();  // Skip `fun`
    Token functionName = currentToken();

    if (functionName.type != TokenType::Identifier) {
        ParsingException("Expected function name after 'fun'");
    }

    advance();  // Skip function name
    if (currentToken().value != "(") {
        ParsingException("Expected '(' after function name");
    }

    // Skip '(' and parameters parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().value != "{") {
        ParsingException("Expected '{' after function declaration");
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
        ParsingException("Expected '(' for function call");
    }

    // Skip '(' and arguments parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`
}

void Parser::parseWhileLoop() {
    advance();  // Skip `while`
    if (currentToken().value != "(") {
        ParsingException("Expected '(' after 'while'");
    }

    // Skip '(' and condition parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().value != "{") {
        ParsingException("Expected '{' after while condition");
    }

    advance();  // Enter while loop body
    while (currentToken().value != "}") {
        parseStatement();
    }
    advance();  // Skip `}`
}

void Parser::parseForLoop() {
    advance();  // Skip `for`
    if (currentToken().value != "(") {
        ParsingException("Expected '(' after 'for'");
    }

    // Skip '(' and loop header parsing (simple for now)
    while (currentToken().value != ")") {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().value != "{") {
        ParsingException("Expected '{' after for loop header");
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
            ParsingException("Expected variable name after type declaration");
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
                    
                    // Output the string
                    std::cout << outputString << std::endl;
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
