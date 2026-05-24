#include "Parser.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

Parser::Parser(const std::string& expr, bool radians, bool debug): lexer(expr, debug), currentToken(), radiansMode(radians), debugMode(debug) {advance();}

void Parser::advance() {
            if (debugMode) {
                        std::cout << "[Parser] Advance: current token type=" << static_cast<int>(currentToken.type)
                                    << " value=" << currentToken.value << " pos=" << currentToken.position << std::endl;
            }
            currentToken = lexer.getNextToken();
            if (debugMode) {
                        std::cout << "[Parser] New token: type=" << static_cast<int>(currentToken.type)
                                    << " value=" << currentToken.value << " pos=" << currentToken.position << std::endl;
            }
}

void Parser::consume(TokenType expected, const std::string& errorMsg) {
            if (debugMode) {
                        std::cout << "[Parser] Consume: expecting " << static_cast<int>(expected)
                                    << ", got " << static_cast<int>(currentToken.type) << std::endl;
            }
            if (currentToken.type != expected) {throw std::runtime_error(errorMsg + " at position " + std::to_string(currentToken.position));}
            advance();
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall(TokenType /*funcToken*/, const std::string& funcName, bool radiansMode, bool debugMode) {
            if (debugMode) std::cout << "[Parser] Function call: " << funcName << std::endl;
            consume(TokenType::LPAREN, "Expected '(' after function name");
            if (funcName == "root") {
                        auto left = parseAddSub();
                        if (currentToken.type != TokenType::COMMA) {
                                    throw std::runtime_error("Expected ',' in root() at position " + std::to_string(currentToken.position));
                        }
                        advance();
                        auto right = parseAddSub();
                        consume(TokenType::RPAREN, "Expected ')' after function name");
                        auto inv = std::make_unique<BinaryOpNode>(std::make_unique<NumberNode>(1.0), std::move(right), '/');
                        return std::make_unique<BinaryOpNode>(std::move(left), std::move(inv), '^');
            } else {
                        auto arg = parseAddSub();
                        consume(TokenType::RPAREN, "Expected ')' after function name");
                        return std::make_unique<FunctionCallNode>(funcName, std::move(arg), radiansMode, debugMode);
            }
}


std::unique_ptr<NumberNode> Parser::parseNumber() {
            if (currentToken.type != TokenType::NUMBER) {throw std::runtime_error("Expected NUMBER at position " + std::to_string(currentToken.position));}
            double val = currentToken.value;
            advance();
            return std::make_unique<NumberNode>(val);
}

std::unique_ptr<ASTNode> Parser::parsePrimary(bool radiansMode) {
            if (debugMode) std::cout << "[Parser] parsePrimary" << std::endl;
            if (currentToken.type == TokenType::PLUS) {
                        advance();
                        return parsePrimary(radiansMode);
            }
            if (currentToken.type == TokenType::MINUS) {
                        advance();
                        auto operand = parsePrimary(radiansMode);
                        return std::make_unique<UnaryMinusNode>(std::move(operand));
            }


            std::unique_ptr<ASTNode> node;
            auto CTT = currentToken.type;
            if (CTT == TokenType::NUMBER) {
                        node = parseNumber();
            } else if (CTT == TokenType::CONST_PI || CTT == TokenType::CONST_E) {
                        std::string name = (currentToken.type == TokenType::CONST_PI) ? "PI" : "E";
                        advance();
                        node = std::make_unique<ConstantNode>(name);
            } else if (CTT == TokenType::LOGTEN ||
                                    CTT == TokenType::LOG2 ||
                                    CTT == TokenType::LOGE ||
                                    CTT == TokenType::SIN ||
                                    CTT == TokenType::COS ||
                                    CTT == TokenType::TAN ||
                                    CTT == TokenType::ROOT ||
                                    CTT == TokenType::SQRT ||
                                    CTT == TokenType::CTAN) {
                        std::string funcName;
                        switch (CTT) {
                                    case TokenType::LOGTEN: funcName = "ln"; break;
                                    case TokenType::LOG2: funcName = "logtwo"; break;
                                    case TokenType::LOGE: funcName = "loge"; break;
                                    case TokenType::SIN: funcName = "sin"; break;
                                    case TokenType::COS: funcName = "cos"; break;
                                    case TokenType::TAN: funcName = "tan"; break;
                                    case TokenType::CTAN: funcName = "ctg"; break;
                                    case TokenType::SQRT: funcName = "sqrt"; break;
                                    case TokenType::ROOT: funcName = "root"; break;
                                    default: break;
                        }
                        advance();
                        node = parseFunctionCall(currentToken.type, funcName, radiansMode, debugMode);
            } else if (CTT == TokenType::LPAREN) {
                        advance();
                        node = parseAddSub();
                        consume(TokenType::RPAREN, "Expected )");
            } else {
                        throw std::runtime_error("Expected number or ( at position " + std::to_string(currentToken.position));
            }

            while (currentToken.type == TokenType::NUMBER || currentToken.type == TokenType::LPAREN) {
                        auto right = (currentToken.type == TokenType::NUMBER) ? parseNumber() : [&]() {
                                    advance();
                                    auto n = parseAddSub();
                                    consume(TokenType::RPAREN, "Expected )");
                                    return n;
                        }();
                        node = std::make_unique<BinaryOpNode>(std::move(node), std::move(right), '*');
            }

            return node;
}



std::unique_ptr<ASTNode> Parser::parseAddSub() {
            if (debugMode) std::cout << "[Parser] parseAddSub" << std::endl;
            auto left = parseMulDiv();
            while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
                        char op = (currentToken.type == TokenType::PLUS) ? '+' : '-';
                        advance();
                        auto right = parseMulDiv();
                        if (debugMode) std::cout << "[Parser] AddSub op: " << op << std::endl;
                        left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
            }
            return left;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
            if (debugMode) std::cout << "[Parser] parseEquality" << std::endl;
            auto left = parseAddSub();
            while (currentToken.type == TokenType::EQUAL || currentToken.type == TokenType::NOTEQUAL) {
                        TokenType t = currentToken.type;
                        advance();
                        auto right = parseAddSub();
                        char opChar = (t == TokenType::EQUAL) ? '=' : '#';
                        if (debugMode) std::cout << "[Parser] Equality op: " << opChar << std::endl;
                        left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), opChar);
            }
            return left;
}



std::unique_ptr<ASTNode> Parser::parsePows() {
            if (debugMode) std::cout << "[Parser] parsePows" << std::endl;
            auto left = parsePrimary(radiansMode);
            if (currentToken.type == TokenType::POW) {
                        advance();
                        auto right = parsePows();
                        left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), '^');
            }
            return left;
}

std::unique_ptr<ASTNode> Parser::parseMulDiv() {
            if (debugMode) std::cout << "[Parser] parseMulDiv" << std::endl;
            std::unique_ptr<ASTNode> left = parsePows();
            while (currentToken.type == TokenType::STAR || currentToken.type == TokenType::SLASH) {
                        char op = (currentToken.type == TokenType::STAR) ? '*' : '/';
                        advance();
                        auto right = parsePows();
                        if (debugMode) std::cout << "[Parser] MulDiv op: " << op << std::endl;
                        left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
            }
            return left;
}



std::unique_ptr<ASTNode> Parser::parseExpression() {
            if (debugMode) std::cout << "[Parser] parseExpression" << std::endl;
            auto node = parseEquality();
            if (currentToken.type != TokenType::END) {
                        throw std::runtime_error("Unexpected token after expression at position " + std::to_string(currentToken.position));
            }
            if (debugMode) std::cout << "[Parser] parseExpression done" << std::endl;
            return node;
}
