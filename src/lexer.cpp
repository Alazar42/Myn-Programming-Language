#include "../include/lexer.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>

typedef std::map<std::string, TokenType> ReservedIdentMap;

// Reserved identifiers map
ReservedIdentMap reservedIdent;

void INIT_RESERVED_IDENTIFIER()
{
    reservedIdent["myn"] = TokenType::Myn;
}

std::vector<std::string> splitString(const std::string &sourceCode)
{
    std::vector<std::string> words;
    std::string word;
    bool inQuote = false; // Flag to track if we are inside a quoted string

    for (char ch : sourceCode)
    {
        if (ch == '\'' || ch == '"')
        {
            // Toggle the inQuote flag when encountering a quote
            inQuote = !inQuote;

            // If we're not inside a quoted string anymore, add the word to the vector
            if (!inQuote && !word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else if (!inQuote && ch == ' ')
        {
            // If we're not inside a quoted string and encounter a space,
            // add the word to the vector
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else
        {
            // If we're inside a quoted string or the character is not a space,
            // add the character to the current word
            word += ch;
        }
    }

    // Add the last word if it's not empty
    if (!word.empty())
    {
        words.push_back(word);
    }

    return words;
}

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
    std::vector<std::string> src = splitString(sourceCode);

    int currentPosition = 0;

    while (!src.empty())
    {
        if (src.front() == "(")
        {
            tokens.push_back(token(shift(src), TokenType::OpenParen));
        }
        else if (src.front() == ")")
        {
            tokens.push_back(token(shift(src), TokenType::CloseParen));
        }
        else if (src.front() == "[")
        {
            tokens.push_back(token(shift(src), TokenType::OpenSBracket));
        }
        else if (src.front() == "]")
        {
            tokens.push_back(token(shift(src), TokenType::CloseSBracket));
        }
        else if (src.front() == "{")
        {
            tokens.push_back(token(shift(src), TokenType::OpenBrace));
        }
        else if (src.front() == "}")
        {
            tokens.push_back(token(shift(src), TokenType::CloseBrace));
        }
        else if (src.front() == "+" || src.front() == "-" || src.front() == "*" || src.front() == "/")
        {
            tokens.push_back(token(shift(src), TokenType::BinaryOperator));
        }
        else if (src.front() == "=")
        {
            if (currentPosition == 0)
            {
                std::cerr << "Error: '=' cannot appear at the beginning of the expression." << std::endl;
                return tokens;
            }

            // Check if "=" is followed by a valid expression
            if (src.size() > 1)
            {
                const std::string &nextToken = src[1];
                if (isNumber(nextToken) || isAlpha(nextToken) || nextToken == "(")
                {
                    tokens.push_back(token(shift(src), TokenType::Equals));
                }
                else
                {
                    std::cerr << "Error: Invalid expression after '=' at position " << currentPosition << "." << std::endl;
                    return tokens;
                }
            }
            else
            {
                std::cerr << "Error: '=' must be followed by an expression at position " << currentPosition << "." << std::endl;
                return tokens;
            }
        }
        else if (src.front() == "myn")
        {
            tokens.push_back(token(shift(src), TokenType::Myn));
        }
        else if (src.front() == "and" || src.front() == "or")
        {
            tokens.push_back(token(shift(src), TokenType::LogicalOperator));
        }
        else if (src.front() == "for")
        {
            tokens.push_back(token(shift(src), TokenType::ForKeyword));
        }
        else if (src.front() == "while")
        {
            tokens.push_back(token(shift(src), TokenType::WhileKeyword));
        }
        else if (src.front() == "switch")
        {
            tokens.push_back(token(shift(src), TokenType::SwitchKeyword));
        }
        else if (src.front() == "class")
        {
            tokens.push_back(token(shift(src), TokenType::ClassKeyword));
        }
        else if (src.front() == "fun")
        {
            tokens.push_back(token(shift(src), TokenType::FunctionKeyword));
        }
        else if (src.front() == "in")
        {
            tokens.push_back(token(shift(src), TokenType::In));
        }
        else if (src.front() == "out")
        {
            tokens.push_back(token(shift(src), TokenType::Out));
        }
        else if (src.front() == "\"" || src.front() == "'")
        {
            // Handle strings enclosed in double or single quotes
            std::string quote = shift(src);
            std::string str;
            str += quote; // Include the opening quote
            while (!src.empty() && src.front() != quote)
            {
                str += shift(src);
            }
            if (!src.empty())
            {
                str += shift(src); // Add the closing quote
            }
            tokens.push_back(token(str, TokenType::String));
        }
        // else if(src.front() == "")
        else
        {
            if (isNumber(src.front()))
            {
                std::string number;
                while (!src.empty() && isNumber(src.front()))
                {
                    number += shift(src);
                }

                tokens.push_back(token(number, TokenType::Number));
            }
            else if (isAlpha(src.front()))
            {
                std::string ident = shift(src);
                ReservedIdentMap::iterator it = reservedIdent.find(ident);
                if (it != reservedIdent.end())
                {
                    tokens.push_back(token(ident, it->second));
                }
                else
                {
                    tokens.push_back(token(ident, TokenType::Identifier));
                }
            }
            else if (isSkippable(src.front()[0]))
            {
                shift(src);
            }
            else
            {
                std::cerr << "Error: Unrecognized character '" << src.front() << "' found at position " << currentPosition << "." << std::endl;
                return tokens;
            }
        }

        currentPosition++;
    }

    return tokens;
}
