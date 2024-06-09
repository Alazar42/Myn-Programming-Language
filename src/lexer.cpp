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
    for (char ch : sourceCode)
    {
        if (ch != ' ')
        {
            word += ch;
        }
        else if (!word.empty())
        {
            words.push_back(word);
            word.clear();
        }
    }
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
    return ch == ' ' || ch == '\t' || ch == '\n';
}

Token token(std::string value, TokenType tokentype)
{
    return {value, tokentype};
}

std::vector<Token> tokenize(std::string &sourceCode)
{
    std::vector<Token> tokens;
    std::vector<std::string> src = splitString(sourceCode);

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
        else if (src.front() == "+" || src.front() == "-" || src.front() == "*" || src.front() == "/")
        {
            tokens.push_back(token(shift(src), TokenType::BinaryOperator));
        }
        else if (src.front() == "=")
        {
            tokens.push_back(token(shift(src), TokenType::Equals));
        }
        else if (src.front() == "myn")
        {
            tokens.push_back(token(shift(src), TokenType::Myn));
        }
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
                std::cout << "Unrecognized character found! " << std::endl;
                exit(1);
            }
        }
    }

    return tokens;
}


