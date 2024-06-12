#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>

enum TokenType
{
    Number,
    Identifier,
    AssignmentOperator,
    OpenParen,
    CloseParen,
    ArithmeticOperator,
    LogicalOperator,
    OpenBrace,
    CloseBrace,
    OpenSBracket,
    CloseSBracket,
    String, 
    Semicolon, 
    Skip,      
    Unknown,
    Comma,
    ReservedKeyword
};


struct Token
{
    std::string value;
    TokenType type;
};

void INIT_RESERVED_IDENTIFIER();
std::vector<Token> tokenize(std::string &sourceCode);
void check_spacing(const std::vector<Token> &tokens); // Function to check spacing issues
TokenType checkTokenType(const std::string &token);
bool isReservedKeyword(const std::string& word);
#endif // LEXER_H
