#pragma once

#include "../lexer/Lexer.h"
#include "../ast/ASTNode.h"
#include <memory>
#include <stdexcept>

class Parser {
public:
            Parser(const std::string& expr, bool radiansMode = false, bool debugMode = false);
            std::unique_ptr<ASTNode> parseExpression();
private:
            Lexer lexer;
            Token currentToken;
            bool radiansMode;
            bool debugMode;
            void advance();
            void consume(TokenType expected, const std::string& errorMsg);
            std::unique_ptr<ASTNode> parseFunctionCall(TokenType funcToken, const std::string& funcName, bool radiansMode, bool debugMode);
            std::unique_ptr<NumberNode> parseNumber();
            std::unique_ptr<ASTNode> parseAddSub();
            std::unique_ptr<ASTNode> parseMulDiv();
            std::unique_ptr<ASTNode> parsePows();
            std::unique_ptr<ASTNode> parsePrimary(bool radiansMode);
            std::unique_ptr<ASTNode> parseEquality();
};
