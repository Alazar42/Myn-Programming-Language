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
    reservedIdent["myn"] = TokenType::Myn;
    reservedIdent["for"] = TokenType::For;
    reservedIdent["while"] = TokenType::While;
    reservedIdent["switch"] = TokenType::Switch;
    reservedIdent["fun"] = TokenType::Fun;
    reservedIdent["class"] = TokenType::Class;
    reservedIdent["break"] = TokenType::Break;
    reservedIdent["case"] = TokenType::Case;
    reservedIdent["True"] = TokenType::True;
    reservedIdent["False"] = TokenType::False;
    reservedIdent["public"] = TokenType::Public;
    reservedIdent["enum"] = TokenType::Enum;
    reservedIdent["private"] = TokenType::Private;
    reservedIdent["protected"] = TokenType::Protected;
    reservedIdent["void"] = TokenType::Void;
    reservedIdent["this"] = TokenType::This;
    reservedIdent["throw"] = TokenType::Throw;
    reservedIdent["try"] = TokenType::Try;
    reservedIdent["catch"] = TokenType::Catch;
    reservedIdent["import"] = TokenType::Import;
    reservedIdent["continue"] = TokenType::Continue;
    reservedIdent["pass"] = TokenType::Pass;
    reservedIdent["NULL"] = TokenType::Null;
    reservedIdent["elif"] = TokenType::Elif;
    reservedIdent["else"] = TokenType::Else;
    reservedIdent["if"] = TokenType::If;
    reservedIdent["static"] = TokenType::Static;
    reservedIdent["return"] = TokenType::Return;
    reservedIdent["input"] = TokenType::Input;
    reservedIdent["output"] = TokenType::Output;

    // Add data types
    reservedIdent["int"] = TokenType::IntType;
    reservedIdent["float"] = TokenType::FloatType;
    reservedIdent["string"] = TokenType::StringType;
    reservedIdent["bool"] = TokenType::BooleanType;
}

std::unordered_set<std::string> defaultReservedKeywords = {
    "fun", "for", "while", "switch", "class", "break", "case", "true", "false",
    "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch",
    "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return",
    "input", "output", "int", "float", "string", "bool"};

std::unordered_set<std::string> reservedKeywords = {
    "myn", "for", "while", "switch", "fun", "class", "break", "case", "True", "False",
    "public", "enum", "private", "protected", "void", "this", "throw", "try", "catch",
    "import", "continue", "pass", "NULL", "elif", "else", "if", "static", "return",
    "input", "output", "int", "float", "string", "bool"};

void updateReservedKeywords(const std::unordered_map<std::string, std::string> &config)
{
    reservedKeywords.clear(); // Clear the existing reserved keywords
    for (const auto &entry : config)
    {
        reservedKeywords.insert(entry.second); // Add the new keywords to the reserved keywords set
    }
    // Add the default reserved keywords to the set, but only if they are not overridden
    for (const auto &keyword : defaultReservedKeywords)
    {
        if (config.count(keyword) == 0)
        { // Check if the keyword is not overridden
            reservedKeywords.insert(keyword);
        }
    }
}
std::set<std::pair<std::string, std::string>> classChildRelations;

void addClassChildRelation(const std::string &parent, const std::string &child)
{
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
            if (hasDecimal)
                return false;
            hasDecimal = true;
        }
        else if (!isdigit(ch))
        {
            return false;
        }
    }
    return true;
}

// Function to classify the token and return its TokenType
TokenType classifyToken(const std::string &token) {
    // Check if the token is a reserved identifier
    if (reservedIdent.find(token) != reservedIdent.end()) {
        return reservedIdent[token];
    }

    // Check if the token is an identifier (alphabetic or underscore)
    if (isalpha(token[0]) || token[0] == '_') {
        for (char ch : token) {
            if (!isalnum(ch) && ch != '_') {
                return TokenType::Invalid; // Invalid identifier
            }
        }
        return TokenType::Identifier;
    }

    // Check if the token is a number (integer or float)
    bool hasDecimalPoint = false;
    for (char ch : token) {
        if (ch == '.') {
            if (hasDecimalPoint) return TokenType::Invalid; // Multiple decimal points
            hasDecimalPoint = true;
        } else if (!isdigit(ch)) {
            return TokenType::Invalid; // Not a valid number
        }
    }
    return hasDecimalPoint ? TokenType::FloatType : TokenType::IntNumber; // Determine type based on decimal point
}

// Check if the token is an identifier
bool isAlpha(const std::string &str, TokenType &type) {
    // Classify the token and assign its TokenType
    type = classifyToken(str);

    // Check if the token is an identifier
    return type == TokenType::Identifier;
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
                case '(':
                    type = TokenType::OpenParen;
                    break;
                case ')':
                    type = TokenType::CloseParen;
                    break;
                case '[':
                    type = TokenType::OpenSBracket;
                    break;
                case ']':
                    type = TokenType::CloseSBracket;
                    break;
                case '{':
                    type = TokenType::OpenBrace;
                    break;
                case '}':
                    type = TokenType::CloseBrace;
                    break;
                case '+':
                case '-':
                case '*':
                case '/':
                    type = TokenType::ArithmeticOperator;
                    break;
                case '=':
                    type = TokenType::AssignmentOperator;
                    break;
                case ';':
                    type = TokenType::Semicolon;
                    break;
                case ',':
                    type = TokenType::Comma;
                    break;
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

std::vector<Token> tokenize_with_config(std::string &sourcecode, std::unordered_map<std::string, std::string> &config)
{
    updateReservedKeywords(config); // Update the reservedKeywords set with the new keywords from the config map
    std::vector<Token> tokens;
    std::string buffer;
    bool inQuote = false;
    char quoteChar = '0';

    for (size_t i = 0; i < sourcecode.size(); ++i)
    {
        char ch = sourcecode[i];
        if (ch == '\'' || ch == '"')
        {
            inQuote = !inQuote; // Toggle inQuote
            if (inQuote)
            {
                quoteChar = ch; // Store quote character
            }
            else
            {
                buffer += ch;                                       // Close the string
                tokens.push_back(token(buffer, TokenType::String)); // Assuming you have a String type
                buffer.clear();
            }
        }
        else if (isSkippable(ch))
        {
            if (!buffer.empty())
            {
                // Handle the buffer before skipping
                tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
                buffer.clear();
            }
        }
        else if (ch == '#')
        {
            // Handle single-line comment
            while (i < sourcecode.size() && sourcecode[i] != 'n') // Fixed 'n' character
            {
                ++i;
            }
        }
        else if (ch == '/' && i + 1 < sourcecode.size() && sourcecode[i + 1] == '*')
        {
            // Handle multi-line comment
            i += 2;
            while (i + 1 < sourcecode.size() && !(sourcecode[i] == '*' && sourcecode[i + 1] == '/'))
            {
                ++i;
            }
            i += 1; // Skip the closing '*/'
        }
        else if (isSymbol(ch))
        { 
            if (!buffer.empty())
            {
                tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
                buffer.clear();
            }

            // Handle single-character symbols
            TokenType type = getSymbolTokenType(std::string(1, ch)); // Convert char to std::string
            tokens.push_back(token(std::string(1, ch), type));
        }
        else
        {
            buffer += ch;
        }
    }

    if (!buffer.empty())
    {
        tokens.push_back(token(buffer, TokenType::Identifier)); // Default to identifier
    }

    return tokens;
}

// Helper function to determine if a character is a symbol
bool isSymbol(char ch)
{
    return ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
           ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' ||
           ch == ';' || ch == ',';
}

// Helper function to get the TokenType for symbols
TokenType getSymbolTokenType(const std::string& symbol) {
    static const std::unordered_map<std::string, TokenType> tokenMap = {
        {"(", TokenType::OpenParen},
        {")", TokenType::CloseParen},
        {"[", TokenType::OpenSBracket},
        {"]", TokenType::CloseSBracket},
        {"{", TokenType::OpenBrace},
        {"}", TokenType::CloseBrace},
        {"+", TokenType::ArithmeticOperator},
        {"-", TokenType::ArithmeticOperator},
        {"*", TokenType::ArithmeticOperator},
        {"/", TokenType::ArithmeticOperator},
        {"=", TokenType::AssignmentOperator},
        {";", TokenType::Semicolon},
        {",", TokenType::Comma},
        // Add more symbols as needed
    };

    auto it = tokenMap.find(symbol);
    if (it != tokenMap.end()) {
        return it->second;
    }

    return TokenType::Unknown; // Handle unexpected symbols
}

TokenType checkTokenType(const std::string &token)
{
    TokenType type;

    if (isAlpha(token, type))
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
    switch (token[0])
    {
    case '=':
        return TokenType::AssignmentOperator;
    case '+':
    case '-':
    case '*':
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
