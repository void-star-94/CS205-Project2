//
// Created by void-star on 2022/10/13.
//

#ifndef PROJECT2_MESSAGES_H
#define PROJECT2_MESSAGES_H

#include <string>

const static std::string RED_BEGIN = "\033[31;1m";
const static std::string RED_END = "\033[0m";
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
    const static std::string PRINT = "print";
    const static std::string EVAL = "eval";
    const static std::string QUIT = "quit";
    const static std::string SHOW = "show";
    const static std::string CLEAR = "clear";
    const static std::string HISTORY = "history";
}
#endif //PROJECT2_MESSAGES_H
