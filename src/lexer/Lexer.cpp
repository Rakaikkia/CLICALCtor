#include "Lexer.h"
#include "Token.h"
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>

Lexer::Lexer(const std::string& str, bool debug): input(str), index(0), debugMode(debug) {}

void Lexer::skipWhitespace() {while (index < input.size() && std::isspace(static_cast<unsigned char>(input[index]))) {++index;}}

Token Lexer::readNumber() {
            size_t start = index;
            while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index]))) ++index;
            if (index < input.size() && input[index] == '.') {
                        ++index;
                        while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index]))) ++index;
            }
            if (index < input.size() && (input[index] == 'e')) {
                        ++index;
                        if (index < input.size() && (input[index] == '+' || input[index] == '-')) ++index;
                        bool hasDigits = false;
                        while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index]))) {
                                    hasDigits = true;
                                    ++index;
                        }
                        if (!hasDigits) throw std::runtime_error("Invalid number exponent at position " + std::to_string(start));
            }

            std::string numStr = input.substr(start, index - start);
            if (debugMode) {
                        std::cout << "[Lexer] Number: " << numStr << " at position " << start << std::endl;
            }
            try {
                        double value = std::stod(numStr);
                        return Token(value, start);
            } catch (const std::exception&) {
                        throw std::runtime_error("Invalid number '" + numStr + "' at position " + std::to_string(start));
            }
}


Token Lexer::readIdentifier() {
            size_t start = index;
            while (index < input.size() && std::isalnum(static_cast<unsigned char>(input[index]))) {++index;}
            std::string name = input.substr(start, index - start);

            if (debugMode) {std::cout << "[Lexer] Identifier: " << name << " at position " << start << std::endl;}
            static const std::unordered_map<std::string, TokenType> opMap = {
                        {"ln", TokenType::LOGTEN},
                        {"logtwo", TokenType::LOG2},
                        {"loge", TokenType::LOGE},
                        {"sin", TokenType::SIN},
                        {"cos", TokenType::COS},
                        {"tan", TokenType::TAN},
                        {"tg", TokenType::TAN},
                        {"cot", TokenType::CTAN},
                        {"ctg", TokenType::CTAN},
                        {"sqrt", TokenType::SQRT},
                        {"root", TokenType::ROOT},
                        {"PI", TokenType::CONST_PI},
                        {"E", TokenType::CONST_E},
            };
            auto it = opMap.find(name);
            if (debugMode) {
                        std::cout << "[Lexer] Identifier: " << name << " at position " << start << std::endl;
            }
            if (it != opMap.end()) {
                        return Token(it->second, start);
            }
            throw std::runtime_error("Unexpected identifier " + name + " at position " + std::to_string(index));
}

Token Lexer::getNextToken() {
            skipWhitespace();
            if (index >= input.size()) return Token(TokenType::END, index);

            if (index + 1 < input.size() && input[index] == '!' && input[index+1] == '=') {
                        index += 2;
                        return Token(TokenType::NOTEQUAL, index - 2);
            }


            char ch = input[index];
            if (std::isalpha(static_cast<unsigned char>(ch))) {return readIdentifier();}
            if (std::isdigit(static_cast<unsigned char>(ch))) {return readNumber();}

            static const std::unordered_map<char, TokenType> opMap = {
                        {'+', TokenType::PLUS},
                        {'-', TokenType::MINUS},
                        {'*', TokenType::STAR},
                        {'^', TokenType::POW},
                        {'/', TokenType::SLASH},
                        {'(', TokenType::LPAREN},
                        {')', TokenType::RPAREN},
                        {',', TokenType::COMMA},
                        {'=', TokenType::EQUAL},
                        {'#', TokenType::NOTEQUAL},
            };
            auto it = opMap.find(ch);
            if (it != opMap.end()) {
                        ++index;
                        if (debugMode) {
                                    std::cout << "[Lexer] Operator: " << ch << " at position " << (index - 1) << std::endl;
                        }
                        return Token(it->second, index - 1);
            }
            throw std::runtime_error("Unexpected character at position " + std::to_string(index));
}
