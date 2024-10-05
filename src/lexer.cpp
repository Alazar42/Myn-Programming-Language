#include "lexer.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cctype>
#include <set>

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

std::unordered_set<std::string> defaultReservedKeywords = {
    "fun", "start", "for", "while", "switch", "class", "break", "case", "true", "false", 
    "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch", 
    "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return", 
    "input", "output", "int", "float", "string", "bool", "start"
};

std::unordered_set<std::string> reservedKeywords = {
    "myn", "for", "while", "switch", "fun", "class", "break", "case", "True", "False", 
    "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch", 
    "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return", 
    "input", "output", "int", "float", "string", "bool", "start"
};


void updateReservedKeywords(const std::unordered_map<std::string, std::string>& config) {
    reservedKeywords.clear(); // Clear the existing reserved keywords
    for (const auto& entry : config) {
        reservedKeywords.insert(entry.second); // Add the new keywords to the reserved keywords set
    }
    // Add the default reserved keywords to the set, but only if they are not overridden
    for (const auto& keyword : defaultReservedKeywords) {
        if (config.count(keyword) == 0) { // Check if the keyword is not overridden
            reservedKeywords.insert(keyword);
        }
    }
}
std::set<std::pair<std::string, std::string>> classChildRelations;

void addClassChildRelation(const std::string& parent, const std::string& child) {
    classChildRelations.insert({parent, child});
}

std::string shift(std::vector<std::string> &src)
{
    std::string current = src.front();
    src.erase(src.begin());
    return current;
}

bool isNumber(const std::string &str)
{
    bool hasDecimal = false;
    for (char ch : str)
    {
        if (ch == '.')
        {
            if (hasDecimal) return false;
            hasDecimal = true;
        }
        else if (!isdigit(ch))
        {
            return false;
        }
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
            else if (ch == '#')
            {
                // Handle single-line comment
                while (i < sourceCode.size() && sourceCode[i] != '\n')
                {
                    ++i;
                }
            }
            else if (ch == '/' && i + 1 < sourceCode.size() && sourceCode[i + 1] == '*')
            {
                // Handle multi-line comment
                i += 2;
                while (i + 1 < sourceCode.size() && !(sourceCode[i] == '*' && sourceCode[i + 1] == '/'))
                {
                    ++i;
                }
                i += 1; // Skip the closing '*/'
            }
            else if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || 
                     ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ';' || ch == ',')
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
                    case ',': type = TokenType::Comma; break;
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

std::vector<Token> tokenize_with_config(std::string &sourcecode, std::unordered_map<std::string, std::string> &config) {
    updateReservedKeywords(config); // Update the reservedKeywords set with the new keywords from the config map
    std::vector<Token> tokens;
    std::string buffer;
    bool inQuote = false;
    char quoteChar = '\0';

    for (size_t i = 0; i < sourcecode.size(); ++i) {
        char ch = sourcecode[i];
        if (ch == '\'' || ch == '"') {
            inQuote = true;
            quoteChar = ch;
            if (!buffer.empty()) {
                if (reservedKeywords.count(buffer)) {
                    tokens.push_back(token(buffer, TokenType::ReservedKeyword));
                } else {
                    tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
                }
                buffer.clear();
            }
            buffer += ch;
            } else if (isSkippable(ch)) {
                if (!buffer.empty()) {
                    if (reservedKeywords.count(buffer)) {
                        tokens.push_back(token(buffer, TokenType::ReservedKeyword));
                    } else {
                        tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
                    }
                    buffer.clear();
                }
            } else if (ch == '#') {
                // Handle single-line comment
                while (i < sourcecode.size() && sourcecode[i] != '\n') {
                    ++i;
                }
            } else if (ch == '/' && i + 1 < sourcecode.size() && sourcecode[i + 1] == '*') {
                // Handle multi-line comment
                i += 2;
                while (i + 1 < sourcecode.size() && !(sourcecode[i] == '*' && sourcecode[i + 1] == '/')) {
                    ++i;
                }
                i += 1; // Skip the closing '*/'
            } else if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
                       ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ';' || ch == ',') {
                if (!buffer.empty()) {
                    if (reservedKeywords.count(buffer)) {
                        tokens.push_back(token(buffer, TokenType::ReservedKeyword));
                    } else {
                        tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
                    }
                    buffer.clear();
                }

                // Handle single-character symbols
                TokenType type;
                switch (ch) {
                    case '(': type = TokenType::OpenParen; break;
                    case ')': type = TokenType::CloseParen; break;
                    case '[': type = TokenType::OpenSBracket; break;
                    case ']': type = TokenType::CloseSBracket; break;
                    case '{': type = TokenType::OpenBrace; break;
                    case '}': type = TokenType::CloseBrace; break;
                    case '+': case '-': case '*': case '/': type = TokenType::ArithmeticOperator; break;
                    case '=': type = TokenType::AssignmentOperator; break;
                    case ';': type = TokenType::Semicolon; break;
                    case ',': type = TokenType::Comma; break;
                    default: type = TokenType::Unknown; break;
                }
                tokens.push_back(token(std::string(1, ch), type));
            } else {
                buffer += ch;
            }
        }
    if (!buffer.empty()) {
        if (reservedKeywords.count(buffer)) {
            tokens.push_back(token(buffer, TokenType::ReservedKeyword));
        } else {
            tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
        }
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
        return token.find('.') != std::string::npos ? TokenType::FloatNumber : TokenType::IntNumber;
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