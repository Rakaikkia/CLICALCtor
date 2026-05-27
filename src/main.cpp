#include "parser/Parser.h"
#include <exception>
#include <iostream>
#include <string>

struct Args {
            std::string expression;
            bool radians = false;
            bool help = false;
            bool debug = false;
};


Args ParseArgs(int argc, char* argv[]) {
            Args args;
            for (int i = 1; i < argc; ++i) {
                        std::string arg = argv[i];
                        if (arg == "--expr" && i + 1 < argc)            args.expression = argv[++i];
                        if (arg == "--radians")                         args.radians = true;
                        if (arg == "--help")                            args.help = true;
                        if (arg == "-h")                                args.help = true;
                        if (arg == "-help")                             args.help = true;
                        if (arg == "--debug")                           args.debug = true;
            }
            return args;
}

int main(int argc, char *argv[]) {
            Args args = ParseArgs(argc, argv);
            if (args.help) {
                        std::cout << "Usage: " << argv[0] << " [flags]\n" << std::endl;

                        std::cout << "Flags:\n" <<
                                    "\t--expr \"expression\".Main expression..................Ex: --expr \"1+1\"\n" <<
                                    "\t--radians...........Use radians......................Default: false\n" <<
                                    "\t--help..............Print this message and exit......Default: false\n" <<
                                    "\t--debug.............Show debug info..................Default: false" << std::endl;

                        std::cout << "Expression:\n" <<
                                    "One-char functions:\n" <<
                                    "\ta + b...............Sum. of a and b\n" <<
                                    "\ta - b...............Dif. between a and b\n" <<
                                    "\ta * b...............Mul. a by b\n" <<
                                    "\ta / b...............Div. a by b\n" <<
                                    "\ta % b...............Rem. of div. of a by b\n" <<
                                    "\ta ^ b...............Raise a to power of b\n" <<
                                    "\ta = b...............Returns true if a is equal to b, false if a is not equal to b\n" <<
                                    "\ta != b..............Returns true if a is not equal to b, false if a is equal to b (can be written as a # b)\n" <<
                                    "\ta!..................Factorial" << std::endl <<
                                    "Multi-char functions:\n" <<
                                    "\tsqrt(a).............Square root of a\n" <<
                                    "\troot(a, b)..........b root of a\n" <<
                                    "\tsin(a)..............sin(a)\n" <<
                                    "\tcos(a)..............cos(a)\n" <<
                                    "\ttan(a)..............tan(a)\n" <<
                                    "\tctg(a)..............ctg(a) (can bewritten as cot(a))\n" <<
                                    "\tln(a)...............log_10(a)\n" <<
                                    "\tlogE(a).............log_e(a)\n" <<
                                    "\tlogTwo(a)...........log_2(a)\n" <<
                                    "\tsum(a, b, c)........sum of all numbers from a to b with step c\n" <<
                                    "\tabs(a)..............Returns a  if a > 0, else -a" << std::endl <<
                                    "Constants:\n" <<
                                    "\tPI..................3,141592653589793\n" <<
                                    "\teu..................2,718281828459045........(Euler number)\n" <<
                                    "\tc...................299792458................(Speed of light in vac.)\n" <<
                                    "\te...................1,602176634e−19..........(Elementary charge)\n" <<
                                    "\tk...................1,380649e-23.............(Boltzmann constant)\n" <<
                                    "\tNa..................6,02214076e23............(Avogadro constant)\n" <<
                                    "\th...................6,62607015e-34...........(Planck constant)\n" <<
                                    "\thd..................1,054571817e−34..........(Reduced Planck constant)\n" <<
                                    "\tG...................6,67408e-11..............(Gravitational constant)" << std::endl;
                        return 0;
            }

            if (args.debug) std::cout << "DEBUG MODE\n[EXPRESSION] " << args.expression << std::endl;
            try {
                        Parser parser(args.expression, args.radians, args.debug);
                        auto ast = parser.parseExpression();
                        double result = ast->evaluate();
                        if (result == 1 && args.expression.find('=') != std::string::npos)      std::cout << "true" << std::endl;
                        else if (result == 1 && args.expression.find('#') != std::string::npos) std::cout << "true" << std::endl;
                        else if (result == 0 && args.expression.find('=') != std::string::npos) std::cout << "false" << std::endl;
                        else if (result == 0 && args.expression.find('#') != std::string::npos) std::cout << "false" << std::endl;
                        else                                                                    std::cout << result << std::endl;
            } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                        return 1;
            }
            return 0;
}
