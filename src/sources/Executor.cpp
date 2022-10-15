//
// Created by void-star on 2022/10/13.
//
#include "Executor.h"
#include <iostream>
#include <stack>
#include <algorithm>
#include "Number.h"
#include "unordered_map"

#define print(x) (std::cout << (x) << "\n")
std::unordered_map<std::string, Number> variables;
std::vector<std::string> history;

void print_error(Status &status) {
    std::sort(status.hints.begin(), status.hints.end());
    u64 start = 0;
    std::string message;
    auto &str = status.origin;
    auto &hints = status.hints;
    message.reserve(str.size() * 2);
    print(status.message);
    for (auto &hint: hints) {
        message.append(str.substr(start, hint.start - start));
        message.append(color_to_string(hint.color));
        message.append(str.substr(hint.start, hint.start - hint.end + 1));
        message.append(COLOR_END);
        start = hint.end + 1;
    }
    if (start != str.size())message.append(str.substr(start));
    print(message);
}

Status get_input() {
    std::string str;
    std::getline(std::cin, str);
    return Status(str);
}

bool check_brackets(Status &status) {
    std::stack<u64> left_brackets_mis;
    std::vector<Hint> bracket_hints;
    bracket_hints.reserve(16);
    for (u64 i = 0; i < status.origin.size(); ++i) {
        if (status.origin[i] == '(') {
            left_brackets_mis.push(i);
        }
        if (status.origin[i] == ')') {
            if (left_brackets_mis.empty()) {
                bracket_hints.push_back({i, i, Color::Red});
                status.hints = std::move(bracket_hints);
                status.message = Error::BRACKETS_MISMATCH;
                return false;
            }
            u64 pos = left_brackets_mis.top();
            left_brackets_mis.pop();
            bracket_hints.push_back({pos, pos, Color::Green});
            bracket_hints.push_back({i, i, Color::Blue});
        }
    }
    if (left_brackets_mis.empty()) {
        return true;
    }

    while (!left_brackets_mis.empty()) {
        bracket_hints.push_back({left_brackets_mis.top(), left_brackets_mis.top(), Color::Red});
        left_brackets_mis.pop();
    }
    status.hints = std::move(bracket_hints);
    status.message = Error::BRACKETS_MISMATCH;
    return false;
}

bool check_number(const std::string &str) {

}

auto parse_formula(Status &status) {
    auto &str = status.origin;
    auto length = str.size();

    for (u64 i = 0; i < length; ++i) {

    }

}

bool process_command(Status &status) {
    auto &cmd = status.origin;
    if (cmd == Command::HELP) {
        print(help);
        return true;
    }
    if (cmd == Command::QUIT) {
        exit(EXIT_SUCCESS);
    }
    if (cmd == Command::CLEAR) {
        std::system(Command::CLEAR.c_str());
        variables.clear();
        history.clear();
        return true;
    }
    if (cmd == Command::CLC) {
        std::system(Command::CLEAR.c_str());
        return true;
    }
    if (cmd == Command::HISTORY) {
        if (history.empty()) {
            print(history_empty);
            return true;
        }
        for (auto &str: history) {
            print(str);
        }
        return true;
    }
    if (cmd == Command::VARIABLES) {
        if (variables.empty()) {
            print(variable_empty);
            return true;
        }
        for (const auto &[k, v]: variables) {
            std::cout << k << "\t" << v << "\n";
        }
        return true;
    }
    status.message = Error::ILLEGAL_COMMAND;
    return false;
}

bool process_formula(Status &status) {}

bool process_script(Status &status) {}

bool process_input(Status &status) {
    bool is_formula = true, is_command = true;
    for (auto c: status.origin) {
        is_formula &= formula.find(c) != formula.end();
        is_command &= operation.find(c) == operation.end();
        if (!(is_formula || is_command))break;
    }
    if (is_formula && is_command) {
        status.message = Error::ILLEGAL_INPUT;
        status.hints = {{0, status.origin.size() - 1, Color::Red}};
        return false;
    }
    if (is_formula)return process_formula(status);
    return is_command ? process_command(status) : process_script(status);
}
