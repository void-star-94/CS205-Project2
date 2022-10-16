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

using u8 = uint8_t;
using u64 = uint64_t;
using i64 = int64_t;

enum class Symbol {
    Number, Variable, Function,
    Left_Bracket, Right_Bracket, Operator, Comma, Equal,//naive
    Undefined
};

enum class Operator {
    Plus, Minus, Multiply, Divide, Mod, Negative, Positive
};

struct Hint {
    //[start, end]
    u64 start;
    u64 end;

    bool operator<(const Hint &other) const { return start < other.start; }
};

struct Status {
    std::string message;
    std::string origin;
    std::vector<Hint> hints;
};


const static std::string RED_BEGIN = "\033[31;1m";
const static std::string RED_END = "\033[0m";

const static std::set<char> formula = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e',
                                       '+', '-', '*', '/', '%', '(', ')'};
const static std::set<char> number = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e'};
const static std::set<char> decimal = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const static std::set<char> not_command = {'+', '-', '*', '/', '%', '=', '(', ')', ','};
const static std::set<char> operation = {'+', '-', '*', '/', '%'};
const static std::set<char> number_start = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.',
                                            '+', '-'};
const static std::set<char> variable_start = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                              'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                              'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                              'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                              '_'};
const static std::set<char> variable = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                        '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

const static std::set<char> legal = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                     'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                     'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                     '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                     '+', '-', '*', '/', '%', '=', '(', ')', ',', ' ', '.'};

namespace Error {
    const static std::string ILLEGAL_NUMBER = "illegal number: ";
    const static std::string ILLEGAL_CHARACTER = "illegal character: ";
    const static std::string ILLEGAL_FUNCTION_NAME = "illegal function name: ";
    const static std::string ILLEGAL_VARIABLE_NAME = "variable name conflicts with command: ";
    const static std::string ILLEGAL_ASSIGN = "illegal assign statement: ";
    const static std::string MULTIPLE_DOTS = "multiple dots: ";
    const static std::string MULTIPLE_POWERS = "multiple powers: ";
    const static std::string MULTIPLE_EQUALS = "one line can only have one assign statement: ";
    const static std::string ASSIGN_VARIABLE_EMPTY = "you need a variable before assign expression: ";
    const static std::string ASSIGN_EXPR_EMPTY = "you need an expression: ";
    const static std::string ASSIGN_NOT_VARIABLE = "you need to assign a variable: ";
    const static std::string ILLEGAL_FUNCTION_INPUT = "illegal function input: ";
    const static std::string BRACKETS_MISMATCH = "brackets mismatch: ";
    const static std::string ILLEGAL_BRACKETS_PAIR = "there must be function, number or variable between brackets: ";
    const static std::string UNDEFINED_VARIABLE = "undefined variable: ";
    const static std::string DIVIDED_BY_ZERO = "divided by zero: ";
    const static std::string FLOAT_MOD = "mod operation does not support floating point number: ";
    const static std::string ILLEGAL_COMMAND = "illegal command: ";
    const static std::string SYNTAX_ERROR = "syntax error: ";
//    const static std::string ILLEGAL_VARIABLE_NAME = "illegal variable name: ";
}

namespace Command {
    const static std::string HELP = "help";
    const static std::string QUIT = "quit";
    const static std::string CLEAR = "clear";
    const static std::string CLC = "clc";
    const static std::string HISTORY = "history";
    const static std::string VARIABLES = "variables";
    const static std::set<std::string> commands =
            {HELP, QUIT, CLEAR, CLC, HISTORY, VARIABLES};
}

const static std::string help = "this cal";
const static std::string history_empty = "no history";
const static std::string variable_empty = "no variable";

const static std::vector<std::string> function_names =
        {
                "floor",
                "ceil",
                "round",
                "sqrt",
                "exp",
                "log",
                "log10",
                "sin",
                "cos",
                "tan",
                "abs",
                "power",
                "max",
                "min"
        };
#endif //PROJECT2_DEFINES_H
