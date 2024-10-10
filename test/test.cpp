#include <string>
#include <cctype>
#include <unordered_map>
#include <iostream>

// Enum for Token Types
enum class TokenType {
    Identifier,
    IntNumber,
    FloatNumber,
    AssignmentOperator,
    ArithmeticOperator,
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    OpenSBracket,
    CloseSBracket,
    Semicolon,
    Comma,
    Invalid,
    Unknown
};

// Reserved Identifiers
std::unordered_map<std::string, TokenType> reservedIdent = {
    {"int", TokenType::IntNumber},
    {"float", TokenType::FloatNumber},
    {"string", TokenType::Identifier},  // Assuming strings are treated as identifiers here
    {"bool", TokenType::Identifier}
};

// Function to classify a token and return its TokenType
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
    return hasDecimalPoint ? TokenType::FloatNumber : TokenType::IntNumber; // Determine type based on decimal point
}

// Updated isAlpha function
bool isAlpha(const std::string &str, TokenType &type) {
    // Classify the token and assign its TokenType
    type = classifyToken(str);

    // Check if the token is an identifier
    return type == TokenType::Identifier;
}

// Updated checkTokenType function
TokenType checkTokenType(const std::string &token) {
    TokenType type;

    if (isAlpha(token, type)) {
        // If the token consists of only alphabetic characters, it's an identifier
        return TokenType::Identifier;
    }

    // if (isNumber(token)) { // You will need to define isNumber similarly
    //     // If the token consists only of numeric characters, it's a number
    //     return token.find('.') != std::string::npos ? TokenType::FloatNumber : TokenType::IntNumber;
    // }

    // Handle other token types using switch statement
    switch (token[0]) {
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

// Define the isNumber function (based on your existing logic)
bool isNumber(const std::string &token) {
    // Check if the token consists only of numeric characters
    bool hasDecimalPoint = false;
    for (char ch : token) {
        if (ch == '.') {
            if (hasDecimalPoint) return false; // Invalid if there's more than one decimal point
            hasDecimalPoint = true; // Found a decimal point
        } else if (!isdigit(ch)) {
            return false; // Return false if any character is not a digit
        }
    }
    return true; // It's a valid number
}

// Example usage
int main() {
    std::string tokens[] = {"fun", "42", "3.14", "=", "+", "start", "var1", "hello"};

    for (const std::string &token : tokens) {
        TokenType type = checkTokenType(token);
        std::cout << "Token: \"" << token << "\" => Type: " << static_cast<int>(type) << "\n";
    }

    return 0;
}
