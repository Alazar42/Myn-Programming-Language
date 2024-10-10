#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>

enum TokenType
{
    IntNumber,
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
    FloatNumber,
    Myn,
    For,
    While,
    Switch,
    Fun,
    Class,
    Break,
    Case,
    True,
    False,
    Public,
    Enum,
    Private,
    Protected,
    Void,
    This,
    Throw,
    Try,
    Catch,
    Import,
    Continue,
    Pass,
    Null,
    Elif,
    Else,
    If,
    Static,
    Return,
    Input,
    Output,
    IntType,
    FloatType,
    BooleanType,
    StringType,
    Invalid
};


struct Token
{
    std::string value;
    TokenType type;
};

void INIT_RESERVED_IDENTIFIER();
std::vector<Token> tokenize(std::string &sourceCode);
std::vector<Token> tokenize_with_config(std::string &sourcecode, std::unordered_map<std::string, std::string> &config);
void check_spacing(const std::vector<Token> &tokens);
TokenType checkTokenType(const std::string &token);
bool isReservedKeyword(const std::string& word);
bool isSymbol(char ch);
TokenType getSymbolTokenType(const std::string& symbol);
#endif // LEXER_H
