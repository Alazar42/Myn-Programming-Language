#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

enum class TokenType {
    Unknown,
    Identifier,
    Number,
    String,
    BinaryOperator,
    Equals,
    Myn,
    LogicalOperator,
    ForKeyword,
    WhileKeyword,
    SwitchKeyword,
    ClassKeyword,
    FunctionKeyword,
    In,
    Out,
    OpenParen,
    CloseParen,
    OpenSBracket,
    CloseSBracket,
    OpenBrace,
    CloseBrace,
    EndOfStream
};

struct Token {
    std::string value;
    TokenType type;
};

std::vector<Token> tokenize(std::string &sourceCode);

#endif // LEXER_Hz
