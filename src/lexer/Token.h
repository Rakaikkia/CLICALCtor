#pragma once
#include <cstddef>

enum class TokenType {
            NUMBER, COMMA,
            PLUS,MINUS,
            EQUAL, NOTEQUAL,
            STAR,SLASH,
            POW, SQRT, ROOT,
            LPAREN,RPAREN,
            LOG2,LOGTEN,LOGE,
            SIN,TAN,COS,CTAN,
            CONST_E, CONST_PI,
            IDENTIFIER,
            END
};

class Token {
public:
            TokenType type;
            double value;
            size_t position;

            Token() : type(TokenType::END), value(0.0), position(0) {}
            Token(double val, size_t pos)
                        :type(TokenType::NUMBER), value(val), position(pos) {}
            Token(TokenType t, size_t pos)
                        :type(t), value(0.0), position(pos) {}
};
