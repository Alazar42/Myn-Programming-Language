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
    
    if (token.type == Fun) {
            parseFunctionDefinition();
    } else if (token.type == While) {
            parseWhileLoop();
    } else if (token.type == For) {
            parseForLoop();
    } else if (token.type == Output) {
            parseOutputStatement();
    } else if (token.type == IntType || token.type == StringType || token.type == FloatType || token.type == BooleanType) {
            parseVariableDeclaration();
    } else if (token.type == TokenType::Identifier) {
        parseFunctionCall();  // Assume an identifier followed by `(` is a function call
    } else {
        LOG_ERROR("Unexpected token: " + token.type);
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
    if (currentToken().type != OpenParen) {
        LOG_ERROR("Expected '(' after function name");
        exit(0);
    }

    // Skip '(' and parameters parsing (simple for now)
    while (currentToken().type != CloseParen) {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().type != OpenBrace) {
        LOG_ERROR("Expected '{' after function declaration");
        exit(0);
    }

    advance();  // Enter function body
    while (currentToken().type != CloseBrace) {
        parseStatement();
    }
    advance();  // Skip `}`
}

void Parser::parseFunctionCall() {
    Token functionName = currentToken();
    advance();  // Skip function name

    if (currentToken().type != OpenParen) {
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
        exit(0);
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
    if (currentToken().type != OpenBrace) {
        LOG_ERROR("Expected '(' after 'for'");
        exit(0);
    }

    // Skip '(' and loop header parsing (simple for now)
    while (currentToken().type != OpenParen) {
        advance();
    }
    advance();  // Skip `)`

    if (currentToken().type != OpenBrace) {
        LOG_ERROR("Expected '{' after for loop header");
        exit(0);
    }

    advance();  // Enter for loop body
    while (currentToken().type != CloseBrace) {
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
    // Check if the current token is a reserved type
    if (currentToken().type == TokenType::IntType ||
        currentToken().type == TokenType::FloatType ||
        currentToken().type == TokenType::StringType ||
        currentToken().type == TokenType::BooleanType) {
        
        std::string typeName = currentToken().value; // Assuming 'value' holds the string representation of the type
        advance(); // Move to the next token

        if (currentToken().type != TokenType::Identifier) {
            LOG_ERROR("Expected variable name after type declaration");
            exit(0);
        }

        std::string variableName = currentToken().value; // Assuming 'value' holds the identifier name
        advance(); // Move to the next token

        // Handle the rest of the variable declaration (e.g., assignment)
    } else {
        LOG_ERROR("Expected a type declaration (int, float, string, bool)");
        exit(0);
    }
}

void Parser::parseOutputStatement() {
    // Ensure the current token is the 'output' keyword
    if (currentToken().type == TokenType::Output) {
        advance();  // Move to '('
        
        if (currentToken().type == TokenType::OpenParen) {
            advance();  // Move to the string or variable
            
            if (currentToken().type == TokenType::String) {
                std::string outputString = currentToken().value; // Assuming 'value' holds the string content
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
        LOG_ERROR("Unexpected token: " + currentToken().value); // Assuming 'value' holds the string representation of the token
        exit(0);
    }
}

// Function to check if there are more tokens to process
bool Parser::hasMoreTokens() {
    return currentIndex < tokens.size();
}
