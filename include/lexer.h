#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

enum TokenType
{
    Number,
    Identifier,
    Equals,
    OpenParen,
    CloseParen,
    BinaryOperator,
    Myn,
};

struct Token
{
    std::string value;
    TokenType type;
};

void INIT_RESERVED_IDENTIFIER();
std::vector<Token> tokenize(std::string &sourceCode);

#endif // LEXER_H
