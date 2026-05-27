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
                        else if (op == '%') {
                                    if (r == 0.0) throw std::runtime_error("Modulo by zero");
                                    return std::fmod(l, r);
                        }
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

class FactorialNode: public ASTNode {
public:
            std::unique_ptr<ASTNode> operand;
            explicit FactorialNode(std::unique_ptr<ASTNode> op): operand(std::move(op)) {};

            double evaluate() const override {
                        double val = operand->evaluate();
                        if (val < 0 || std::fabs(val - std::round(val)) > 1e-12) {
                                    throw std::runtime_error("Factorial of non-integer or negative number");
                        }
                        int n = static_cast<int>(std::round(val));
                        double result = 1.0;
                        for (int i = 2; i < n + 1; ++i) {
                                    result *=i;
                        }
                        return result;
            }
};

class ConstantNode: public ASTNode {
public:
            std::string name;
            ConstantNode(const std::string& n): name(n) {}
            double evaluate() const override {
                        if (name == "PI") return 3.141592653589793;
                        if (name == "eu") return 2.718281828459045;
                        if (name == "e") return 1.602176634e-19;
                        if (name == "c") return 299792458.0;
                        if (name == "k") return 1.380649e-23;
                        if (name == "h") return 6.62607015e-34;
                        if (name == "G") return 6.67408e-11;
                        if (name == "hd") return 1.054571817e-34;
                        if (name == "Na") return 6.02214076e23;
                        throw std::runtime_error("Unknown constant");
            }
};

class SumNode : public ASTNode {
public:
            std::unique_ptr<ASTNode> start;
            std::unique_ptr<ASTNode> end;
            std::unique_ptr<ASTNode> step;
            bool debugMode;

            SumNode(std::unique_ptr<ASTNode> s, std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> st, bool debug)
                        : start(std::move(s)), end(std::move(e)), step(std::move(st)), debugMode(debug) {}

            double evaluate() const override {
                        double a = start->evaluate();
                        double b = end->evaluate();
                        double h = step->evaluate();

                        if (h == 0.0) throw std::runtime_error("sum step cannot be zero");

                        const double eps = 1e-12;

                        double raw_n = (b - a) / h;
                        long long n = static_cast<long long>(std::floor(raw_n + eps)) + 1;

                        double last = a + (n - 1) * h;

                        if ((h > 0 && last > b + eps) || (h < 0 && last < b - eps)) {
                                    n = (h > 0) ? std::max(1LL, static_cast<long long>((b - a) / h + eps)) + 1 : 1LL;
                                    last = a + (n - 1) * h;
                        }

                        if (debugMode) {
                                    std::cout << "[SUM] count = " << n << ", first = " << a << ", last = " << last << std::endl;
                        }

                        double sum = n * (a + last) / 2.0;
                        return sum;
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
                        if (funcName == "logE") return std::log(arg);
                        if (funcName == "logTwo") return std::log2(arg);
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
                        if (funcName == "abs") return (arg > 0) ? arg : -arg;

                        throw std::runtime_error("Cant find function " + funcName);
            }
};
