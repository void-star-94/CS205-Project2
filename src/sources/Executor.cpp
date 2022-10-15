//
// Created by void-star on 2022/10/13.
//
#include "Executor.h"
#include <iostream>
#include <stack>
#include <algorithm>
#include <string_view>
#include <list>
#include "Number.h"
#include "Functions.h"
#include "unordered_map"

namespace void_star {
#define print(x) (std::cout << (x) << "\n")
    std::unordered_map<std::string, Number> variables;
    std::vector<std::string> history;

    struct Token {
        std::string_view content;
        Symbol symbol;
        Operator op;
        Number value;
        int function_id;
    };

    inline int get_function_id(const std::string_view &str) {
        for (int i = 0; i < function_names.size(); ++i) {
            if (str.compare(function_names[i]) == 0)return i;
        }
        return -1;
    }

    inline void print_error(Status &status) {
        std::sort(status.hints.begin(), status.hints.end());
        u64 start = 0;
        std::string message;
        auto &str = status.origin;
        auto &hints = status.hints;
        message.reserve(str.size() * 2);
        print(status.message);
        for (auto &hint: hints) {
            message.append(str.substr(start, hint.start - start));
            message.append(RED_BEGIN);
            message.append(str.substr(hint.start, hint.start - hint.end + 1));
            message.append(RED_END);
            start = hint.end + 1;
        }
        if (start != str.size())message.append(str.substr(start));
        message.append("\n");
        print(message);
    }

    inline Status get_input() {
        std::string str;
        std::getline(std::cin, str);
        return {{}, std::move(str)};
    }

    inline std::list<Token> preprocess(Status &status) {
        std::list<Token> re;
        auto &str = status.origin;
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        bool checking = false;
        decltype(str.begin()) start;
        Symbol symbol = Symbol::Undefined;
        for (auto it = str.begin(); it != str.end(); ++it) {
            char c = *it;
            if (not_command.find(c) != not_command.end()) {
                if (checking) {
                    checking = false;
                    if (symbol == Symbol::Undefined)
                        symbol = c == '(' ? Symbol::Function : Symbol::Variable;
                    re.push_back({{start, it}, symbol});
                    symbol = Symbol::Undefined;
                }
                if (c == '(')
                    re.push_back({{it, it + 1}, Symbol::Left_Bracket});
                else if (c == ')')
                    re.push_back({{it, it + 1}, Symbol::Right_Bracket});
                else if (c == ',')
                    re.push_back({{it, it + 1}, Symbol::Comma});
                else if (c == '=')
                    re.push_back({{it, it + 1}, Symbol::Equal});
                else if (operation.find(c) != operation.end())
                    re.push_back({{it, it + 1}, Symbol::Operator});
            } else {
                if (!checking) {
                    if (it == str.end() - 1) {
                        if (number.find(c) != number.end() && c != 'e')
                            re.push_back({{it, it + 1}, Symbol::Number});
                        else
                            re.push_back({{it, it + 1}, Symbol::Variable});
                        return re;
                    }
                    checking = true;
                    start = it;
                    if (number.find(c) != number.end() && c != 'e')
                        symbol = Symbol::Number;
                } else {
                    if (it == str.end() - 1) {
                        re.push_back({{start, it + 1},
                                      symbol == Symbol::Undefined ? Symbol::Variable : Symbol::Number});
                        return re;
                    }
                }
            }
        }
        return re;
    }

    inline bool process_command(Status &status) {
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
        status.hints = {{0, status.origin.size() - 1}};
        return false;
    }

    inline std::list<Token>::iterator equal_check(Status &status, std::list<Token> &list) {
        std::vector<std::list<Token>::iterator> eq_pos;
        eq_pos.reserve(4);
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->symbol == Symbol::Equal)
                eq_pos.push_back(it);
        }
        if (eq_pos.size() > 1) {
            for (auto p: eq_pos) {
                status.hints.push_back({u64(&p->content[0] - &status.origin[0]),
                                        u64(&p->content[0] - &status.origin[0])});
            }
            status.message = Error::MULTIPLE_EQUALS;
            return {};
        }
        return eq_pos.empty() ? list.end() : eq_pos[0];
    }

    inline bool check_brackets(Status &status, u64 start) {
        std::stack<u64> left_brackets_mis;
        for (u64 i = start; i < status.origin.size(); ++i) {
            if (status.origin[i] == '(') {
                left_brackets_mis.push(i);
            }
            if (status.origin[i] == ')') {
                if (left_brackets_mis.empty()) {
                    status.hints = {{i, i}};
                    status.message = Error::BRACKETS_MISMATCH;
                    return false;
                }
                if (left_brackets_mis.top() == i - 1) {
                    status.hints = {{i - 1, i}};
                    status.message = Error::BRACKETS_EMPTY;
                    return false;
                }
                left_brackets_mis.pop();
            }
        }
        if (left_brackets_mis.empty()) {
            return true;
        }
        while (!left_brackets_mis.empty()) {
            status.hints.push_back({left_brackets_mis.top(), left_brackets_mis.top()});
            left_brackets_mis.pop();
        }
        status.message = Error::BRACKETS_MISMATCH;
        return false;
    }

    inline bool check_assign(Status &status, const std::list<Token> &list, const std::list<Token>::iterator &eq_pos) {
        if (list.front().symbol == Symbol::Equal) {
            status.message = Error::ASSIGN_VARIABLE_EMPTY;
            status.hints = {{u64(&list.front().content[0] - &status.origin[0]),
                             u64(&list.front().content[0] - &status.origin[0])}};
            return false;
        }
        if (list.back().symbol == Symbol::Equal) {
            status.message = Error::ASSIGN_EXPR_EMPTY;
            status.hints = {{u64(&list.back().content[0] - &status.origin[0]),
                             u64(&list.back().content[0] - &status.origin[0])}};
            return false;
        }
        auto it = ++list.begin();
        if (it->symbol != Symbol::Equal) {
            status.message = Error::ILLEGAL_ASSIGN;
            status.hints = {{u64(&eq_pos->content[0] - &status.origin[0]),
                             u64(&eq_pos->content[0] - &status.origin[0])}};
            return false;
        }
        if (list.front().symbol != Symbol::Variable) {
            status.message = Error::ASSIGN_NOT_VARIABLE;
            status.hints = {{u64(&list.front().content[0] - &status.origin[0]),
                             u64(&list.front().content[0] - &status.origin[0] + list.front().content.size())}};
            return false;
        }
        if (variables.find(std::string(list.front().content)) == variables.end())
            variables[std::string(list.front().content)] = Number();
        return true;
    }

    inline bool check_function_name(Status &status, std::list<Token> &list) {
        for (auto &it: list) {
            if (it.symbol == Symbol::Function) {
                it.function_id = get_function_id(it.content);
                if (it.function_id == -1) {
                    status.message = Error::ILLEGAL_FUNCTION_NAME;
                    status.hints = {{u64(&it.content[0] - &status.origin[0]),
                                     u64(&it.content[0] - &status.origin[0])}};
                    return false;
                }
            }
        }
        return true;
    }

    inline bool check_number(const std::string_view &str, Status &status, Number &num) {
        //no sign!
        std::vector<u64> dot, power;
        dot.reserve(str.size());
        power.reserve(str.size());
        for (u64 i = 0; i < str.size(); ++i) {
            if (str[i] == '.')dot.push_back(i);
            if (str[i] == 'e')power.push_back(i);
        }
        if (dot.size() > 1) {
            status.message = Error::MULTIPLE_DOTS;
            for (auto d: dot) {
                status.hints.push_back({d + u64(&str[0] - &status.origin[0]),
                                        d + u64(&str[0] - &status.origin[0])});
            }
            return false;
        }
        if (power.size() > 1) {
            status.message = Error::MULTIPLE_POWERS;
            for (auto d: power) {
                status.hints.push_back({d + u64(&str[0] - &status.origin[0]),
                                        d + u64(&str[0] - &status.origin[0])});
            }
            return false;
        }
        if (str.size() == (power.size() + dot.size())) {
            status.message = Error::ILLEGAL_NUMBER;
            status.hints = {
                    {
                            u64(&str[0] - &status.origin[0]),
                            str.size() - 1 + u64(&str[0] - &status.origin[0]),
                    }
            };
            return false;
        }
        num = Number(str);
        return true;
    }

    inline bool check_number(Status &status, std::list<Token> &list) {
        for (auto &tk: list) {
            if (tk.symbol == Symbol::Number) {
                if (!check_number(tk.content, status, tk.value))return false;
            }
        }
        return true;
    }

    inline bool check_variable_name(Status &status, std::list<Token> &list) {
        for (auto &tk: list) {
            if (tk.symbol == Symbol::Variable) {
                if (variables.find(std::string(tk.content)) == variables.end()) {
                    status.message = Error::UNDEFINED_VARIABLE;
                    status.hints = {{u64(&tk.content[0] - &status.origin[0]),
                                     tk.content.size() + u64(&tk.content[0] - &status.origin[0])}};
                    return false;
                }
            }
        }
        return true;
    }

    inline bool function_convert(Status &status, std::list<Token> &list) {

    }

    inline bool check(Status &status, std::list<Token> &list) {
        auto eq_pos = equal_check(status, list);
        if (eq_pos == decltype(eq_pos)())return false;
        bool has_eq = eq_pos != list.end();
        u64 start = has_eq ? (&(eq_pos->content[0]) - &status.origin[0]) : 0;
        if (!check_brackets(status, start))return false;
        if (has_eq && (!check_assign(status, list, eq_pos)))return false;
        if (!(check_function_name(status, list) && check_number(status, list)))return false;
        if (!check_variable_name(status, list))return false;
        return true;
    }

    inline bool process(Status &status) {
        auto list = preprocess(status);
        if (!check(status, list))return false;
        std::stack<Token> tmp;
        std::vector<Token> expr;
        expr.reserve(list.size());

        return true;
    }


    inline bool process_input(Status &status) {
        bool is_command = true;
        u64 i = 0;
        for (auto c: status.origin) {
            is_command &= not_command.find(c) == not_command.end();
            if (legal.find(c) == legal.end()) {
                status.message = Error::ILLEGAL_CHARACTER;
                status.hints = {{i, i}};
                return false;
            }
            if (!is_command)break;
            i++;
        }
        return is_command ? process_command(status) : process(status);
    }
}