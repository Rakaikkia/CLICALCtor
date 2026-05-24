#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class ASTNode {
public:
            virtual ~ASTNode() = default;
            virtual double evaluate() const = 0;
};

class NumberNode: public ASTNode {
public:
            double value;
            explicit NumberNode(double val): value(val) {}
            double evaluate() const override {return value;}
};

class BinaryOpNode: public ASTNode {
public:
            std::unique_ptr<ASTNode> left;
            std::unique_ptr<ASTNode> right;
            char op;

            BinaryOpNode(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs, char oper)
                        :left(std::move(lhs)), right(std::move(rhs)), op(oper) {};

            double evaluate() const override {
                        double l = left->evaluate();
                        double r = right->evaluate();

                        if (op == '+') return l + r;
                        else if (op == '-') return l - r;
                        else if (op == '*') return l * r;
                        else if (op == '^') return std::pow(l, r);
                        else if (op == '/') {
                                    if (r == 0.0) throw std::runtime_error ("Division by zero");
                                    return l / r;
                        }
                        else if (op == '=') {
                                    double eps = 1e-9;
                                    return (std::fabs(l - r) <= eps) ? 1.0 : 0.0;
                        }
                        else if (op == '#') {
                                    double eps = 1e-9;
                                    return (std::fabs(l - r) > eps) ? 1.0 : 0.0;
                        }


                        else throw std::runtime_error("Unknown operand in BinaryOpNode");
            }
};

class UnaryMinusNode: public ASTNode {
public:
            std::unique_ptr<ASTNode> operand;
            explicit UnaryMinusNode(std::unique_ptr<ASTNode> op): operand(std::move(op)) {};
            double evaluate() const override {return - (operand->evaluate());}
};

class ConstantNode: public ASTNode {
public:
            std::string name;
            ConstantNode(const std::string& n): name(n) {}
            double evaluate() const override {
                        if (name == "PI") return 3.141592653589793;
                        if (name == "E") return 2.718281828459045;
                        throw std::runtime_error("Unknown constant");
            }
};

class FunctionCallNode: public ASTNode {
public:
            std::string funcName;
            std::unique_ptr<ASTNode> argument;
            bool radiansMode;
            bool debugMode;
            FunctionCallNode(const std::string& name, std::unique_ptr<ASTNode> arg, bool rad, bool debug)
                        :funcName(name), argument(std::move(arg)), radiansMode(rad), debugMode(debug) {}
            double evaluate() const override {
                        double arg = argument->evaluate();
                        if (debugMode) {
                                    std::cout << "[AST] Function " << funcName << " argument = " << arg << std::endl;
                        }
                        if (!radiansMode && (funcName == "sin" ||
                                                            funcName == "cos" ||
                                                            funcName == "tan" ||
                                                            funcName == "ctg")) {
                                    arg = arg * 3.141592653589793 / 180.0;
                        }

                        if (funcName == "ln") return std::log10(arg);
                        if (funcName == "loge") return std::log(arg);
                        if (funcName == "logtwo") return std::log2(arg);
                        if (funcName == "sin") return std::sin(arg);
                        if (funcName == "cos") return std::cos(arg);
                        if (funcName == "tan") return std::tan(arg);
                        if (funcName == "ctg") {
                                    double s = std::sin(arg);
                                    if (s == 0.0) throw std::runtime_error("Division by zero in cotangent");
                                    return std::cos(arg) / s;}
                        if (funcName == "sqrt") {
                                    if (arg < 0.0) throw std::runtime_error("sqrt of negative number");
                                    return std::sqrt(arg);
                        }
                        if (funcName == "root") return std::sqrt(arg);

                        throw std::runtime_error("Cant find function " + funcName);
            }
};
