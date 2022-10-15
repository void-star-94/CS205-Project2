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

enum class Color {
    Red, Green, Blue
};

struct Hint {
    //[start, end]
    u64 start;
    u64 end;
    Color color;

    bool operator<(const Hint &other) const { return start < other.start; }
};

struct Status {
    std::string message;
    const std::string origin;
    std::vector<Hint> hints;

    explicit Status(std::string str) : origin(std::move(str)) {}
};

const static std::string RED_BEGIN = "\033[31;1m";
const static std::string GREEN_BEGIN = "\033[32;1m";
const static std::string BLUE_BEGIN = "\033[34;1m";
const static std::string COLOR_END = "\033[0m";

inline const std::string &color_to_string(Color color) {
    return color == Color::Red ? RED_BEGIN : (color == Color::Blue ? BLUE_BEGIN : GREEN_BEGIN);
}

const std::set<char> formula = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e',
                                '+', '-', '*', '/', '%', '(', ')'};
const std::set<char> number = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e',
                               '+', '-'};
const std::set<char> operation = {'+', '-', '*', '/', '%', '=', '(', ')', ','};

const std::set<char> number_start = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.',
                                     '+', '-'};

const std::set<char> variable_start = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                       'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                       'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                       'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                       '_'};

const std::set<char> variable = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                 '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

namespace Error {
    const static std::string ILLEGAL_NUMBER = "illegal number: ";
    const static std::string ILLEGAL_INPUT = "please input formula, command or script: ";
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
    const static std::string QUIT = "quit";
    const static std::string SHOW = "show";
    const static std::string CLEAR = "clear";
    const static std::string CLC = "clc";
    const static std::string HISTORY = "history";
    const static std::string VARIABLES = "variables";
    const static std::set<std::string> commands =
            {HELP, QUIT, SHOW, CLEAR, CLC, HISTORY, VARIABLES};
}

const static std::string help = "this cal";
const static std::string history_empty = "no history";
const static std::string variable_empty = "no variable";
#endif //PROJECT2_DEFINES_H
