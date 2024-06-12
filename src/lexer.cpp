#include "../include/lexer.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <cctype>

typedef std::map<std::string, TokenType> ReservedIdentMap;

// Reserved identifiers map
ReservedIdentMap reservedIdent;

void INIT_RESERVED_IDENTIFIER()
{
    reservedIdent["myn"] = TokenType::ReservedKeyword;
    reservedIdent["for"] = TokenType::ReservedKeyword;
    reservedIdent["while"] = TokenType::ReservedKeyword;
    reservedIdent["switch"] = TokenType::ReservedKeyword;
    reservedIdent["fun"] = TokenType::ReservedKeyword;
    reservedIdent["class"] = TokenType::ReservedKeyword;
    reservedIdent["break"] = TokenType::ReservedKeyword;
    reservedIdent["case"] = TokenType::ReservedKeyword;
    reservedIdent["True"] = TokenType::ReservedKeyword;
    reservedIdent["False"] = TokenType::ReservedKeyword;
    reservedIdent["public"] = TokenType::ReservedKeyword;
    reservedIdent["enum"] = TokenType::ReservedKeyword;
    reservedIdent["private"] = TokenType::ReservedKeyword;
    reservedIdent["protected"] = TokenType::ReservedKeyword;
    reservedIdent["void"] = TokenType::ReservedKeyword;
    reservedIdent["this"] = TokenType::ReservedKeyword;
    reservedIdent["throw"] = TokenType::ReservedKeyword;
    reservedIdent["try"] = TokenType::ReservedKeyword;
    reservedIdent["catch"] = TokenType::ReservedKeyword;
    reservedIdent["import"] = TokenType::ReservedKeyword;
    reservedIdent["continue"] = TokenType::ReservedKeyword;
    reservedIdent["pass"] = TokenType::ReservedKeyword;
    reservedIdent["NULL"] = TokenType::ReservedKeyword;
    reservedIdent["elif"] = TokenType::ReservedKeyword;
    reservedIdent["else"] = TokenType::ReservedKeyword;
    reservedIdent["if"] = TokenType::ReservedKeyword;
    reservedIdent["static"] = TokenType::ReservedKeyword;
    reservedIdent["return"] = TokenType::ReservedKeyword;
    reservedIdent["input"] = TokenType::ReservedKeyword;
    reservedIdent["output"] = TokenType::ReservedKeyword;

    // Add data types
    reservedIdent["int"] = TokenType::ReservedKeyword;
    reservedIdent["float"] = TokenType::ReservedKeyword;
    reservedIdent["string"] = TokenType::ReservedKeyword;
    reservedIdent["bool"] = TokenType::ReservedKeyword;
}

std::unordered_set<std::string> reservedKeywords = {
    "myn", "for", "while", "switch", "fun", "class", "break", "case", "True", "False", 
    "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch", 
    "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return", 
    "input", "output", "int", "float", "string", "bool", "start"
};

std::string shift(std::vector<std::string> &src)
{
    std::string current = src.front();
    src.erase(src.begin());
    return current;
}

bool isNumber(const std::string &str)
{
    for (char ch : str)
    {
        if (!isdigit(ch))
            return false;
    }
    return true;
}

bool isAlpha(const std::string &str)
{
    for (char ch : str)
    {
        if (!isalpha(ch))
            return false;
    }
    return true;
}

bool isSkippable(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

Token token(std::string value, TokenType tokentype)
{
    return {value, tokentype};
}

std::vector<Token> tokenize(std::string &sourceCode)
{
    std::vector<Token> tokens;
    std::string buffer;
    bool inQuote = false;
    char quoteChar = '\0';

    for (size_t i = 0; i < sourceCode.size(); ++i)
    {
        char ch = sourceCode[i];

        if (inQuote)
        {
            buffer += ch;
            if (ch == quoteChar)
            {
                tokens.push_back(token(buffer, TokenType::String));
                buffer.clear();
                inQuote = false;
                quoteChar = '\0';
            }
        }
        else
        {
            if (ch == '\'' || ch == '"')
            {
                inQuote = true;
                quoteChar = ch;
                if (!buffer.empty())
                {
                    TokenType type = checkTokenType(buffer);
                    tokens.push_back(token(buffer, type));
                    buffer.clear();
                }
                buffer += ch;
            }
            else if (isSkippable(ch))
            {
                if (!buffer.empty())
                {
                    TokenType type = checkTokenType(buffer);
                    tokens.push_back(token(buffer, type));
                    buffer.clear();
                }
            }
            else if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || 
                     ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ';')
            {
                if (!buffer.empty())
                {
                    TokenType type = checkTokenType(buffer);
                    tokens.push_back(token(buffer, type));
                    buffer.clear();
                }
                
                TokenType type;
                switch (ch)
                {
                    case '(': type = TokenType::OpenParen; break;
                    case ')': type = TokenType::CloseParen; break;
                    case '[': type = TokenType::OpenSBracket; break;
                    case ']': type = TokenType::CloseSBracket; break;
                    case '{': type = TokenType::OpenBrace; break;
                    case '}': type = TokenType::CloseBrace; break;
                    case '+': case '-': case '*': case '/': type = TokenType::ArithmeticOperator; break;
                    case '=': type = TokenType::AssignmentOperator; break;
                    case ';': type = TokenType::Semicolon; break;
                }
                tokens.push_back(token(std::string(1, ch), type));
            }
            else
            {
                buffer += ch;
            }
        }
    }

    if (!buffer.empty())
    {
        TokenType type = checkTokenType(buffer);
        tokens.push_back(token(buffer, type));
    }

    return tokens;
}


TokenType checkTokenType(const std::string &token)
{
    if (isReservedKeyword(token))
    {
        // If the token is a reserved keyword, return its corresponding token type
        return TokenType::ReservedKeyword;
    }

    if (isAlpha(token))
    {
        // If the token consists of only alphabetic characters, it's an identifier
        return TokenType::Identifier;
    }
    
    if (isNumber(token))
    {
        // If the token consists only of numeric characters, it's a number
        return TokenType::Number;
    }

    // Handle other token types using switch statement
    switch (token[0]) {
        case '=':
            return TokenType::AssignmentOperator;
        case '+':
            return TokenType::ArithmeticOperator;
        case '-':
            return TokenType::ArithmeticOperator;
        case '*':
            return TokenType::ArithmeticOperator;
        case '/':
            return TokenType::ArithmeticOperator;
        case '(':
            return TokenType::OpenParen;
        case ')':
            return TokenType::CloseParen;
        case '{':
            return TokenType::OpenBrace;
        case '}':
            return TokenType::CloseBrace;
        case '[':
            return TokenType::OpenSBracket;
        case ']':
            return TokenType::CloseSBracket;
        case ';':
            return TokenType::Semicolon;
        case ',':
            return TokenType::Comma;
        // Add more cases for other token types as needed
        default:
            // If none of the above conditions match, it's an unknown token
            return TokenType::Unknown;
    }
}


void check_spacing(const std::vector<Token> &tokens)
{
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        // Check if the current token and the previous token are both non-skippable
        if (tokens[i - 1].type != TokenType::Skip && tokens[i].type != TokenType::Skip)
        {
            // Check if there is no space between the tokens
            if (tokens[i - 1].value.size() > 0 && tokens[i].value.size() > 0 &&
                !isspace(tokens[i - 1].value.back()) && !isspace(tokens[i].value.front()))
            {
                std::cerr << "Warning: No space between tokens '" << tokens[i - 1].value << "' and '" << tokens[i].value << "'." << std::endl;
            }
        }
    }
}

bool isReservedKeyword(const std::string& word) {
    return reservedKeywords.count(word) > 0;
}

