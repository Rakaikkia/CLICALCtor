#pragma once
#include "Token.h"
#include <cstddef>
#include <string>

class Lexer {
private:
            std::string input;
            size_t index;
            bool debugMode;

            void skipWhitespace();
            Token readNumber();
            Token readIdentifier();
            char peek() const;
public:
            Lexer(const std::string& str, bool debug = false);
            Token getNextToken();
};
