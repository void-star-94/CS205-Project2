//
// Created by void-star on 2022/10/13.
//

#ifndef PROJECT2_DEFINES_H
#define PROJECT2_DEFINES_H

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>

class Number;

//static std::unordered_map<std::string, Number> variables;

using Block = std::vector<std::string>;

enum class Expression {
    Formula, Script, Command
};

struct Package {
    Expression expr;
    Block blk;
};

static std::vector<Block> program;
using u8 = uint8_t;
using u64 = uint64_t;
using i64 = int64_t;
const static std::string RED_BEGIN = "\033[31;1m";
const static std::string RED_END = "\033[0m";
const std::set<char> calculation = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e',
                                    '+', '-', '*', '/', '%', '^'};
namespace Error {
    const static std::string ILLEGAL_NUMBER = "illegal number: ";
    const static std::string ILLEGAL_CHARACTER = "illegal character: ";
    const static std::string ILLEGAL_VARIABLE_NAME = "illegal variable name: ";
    const static std::string MULTIPLE_DOTS = "multiple dots: ";
    const static std::string MULTIPLE_POWERS = "multiple powers: ";
    const static std::string UNSUPPORTED_FUNCTION = "unsupported function: ";
    const static std::string UNSUPPORTED_INPUT = "unsupported input: ";
    const static std::string ILLEGAL_FUNCTION_INPUT = "illegal function input: ";
    const static std::string BRACKETS_MISMATCH = "brackets mismatch: ";
    const static std::string UNDEFINED_VARIABLE = "undefined variable: ";
    const static std::string DIVIDED_BY_ZERO = "divided by zero: ";
    const static std::string ILLEGAL_COMMAND = "illegal command: ";
}
namespace Command {
    const static std::string HELP = "help";
    const static std::string PRECISION = "precision";
    const static std::string QUIT = "quit";
    const static std::string SHOW = "show";
    const static std::string CLEAR = "clear";
    const static std::string HISTORY = "history";
    const static std::set<std::string> commands =
            {HELP, PRECISION, QUIT, SHOW, CLEAR, HISTORY};
}
#endif //PROJECT2_DEFINES_H
