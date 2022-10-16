//
// Created by void-star on 2022/10/13.
//
#include "Executor.h"
#include <iostream>
#include <stack>
#include <algorithm>
#include <string_view>
#include <sstream>
#include "Number.h"
#include "Functions.h"
#include "unordered_map"

namespace void_star {
#define print(x) (std::cout << (x) << "\n")
    std::unordered_map<std::string, Number> variables;
    std::vector<std::string> history;

    struct Token {
        std::string_view content;
        Symbol symbol{};
        Operator op{};
        Number value;
        int function_id{};
    };

    inline Operator get_operator(char c) {
        switch (c) {
            case '+':
                return Operator::Plus;
            case '-':
                return Operator::Minus;
            case '*':
                return Operator::Multiply;
            case '/':
                return Operator::Divide;
            default:
                return Operator::Mod;
        }
    }

    inline int get_priority(const Token &token) {
        if (token.symbol == Symbol::Operator) {
            if (token.op == Operator::Plus || token.op == Operator::Minus)return 1;
            if (token.op == Operator::Positive || token.op == Operator::Negative)return 4;
            return 2;
        }
        if (token.symbol == Symbol::Function) {
            return 4;
        }
        return 0;
    }

    inline int get_function_id(const std::string_view &str) {
        for (int i = 0; i < function_names.size(); ++i) {
            if (str.compare(function_names[i]) == 0)return i;
        }
        return -1;
    }

    void print_error(Status &status) {
        std::sort(status.hints.begin(), status.hints.end());
        u64 start = 0;
        std::string message;
        auto &str = status.origin;
        auto &hints = status.hints;
        message.reserve(str.size() * 2);

        print(status.message);
        for (auto &hint: hints) {
            message += str.substr(start, hint.start - start);
            message += RED_BEGIN;
            message += str.substr(hint.start, hint.end - hint.start + 1);
            message += RED_END;
            start = hint.end + 1;
        }
        if (start < str.size())message.append(str.substr(start));
        print(message);
    }

    Status get_input() {
        std::string str;
        std::getline(std::cin, str);
        return {{}, std::move(str)};
    }

    inline u64 get_loc(const std::string &origin, const std::string_view &str, u64 d = 0) {
        return d + (&str[0] - &origin[0]);
    }

    inline std::vector<Token> preprocess(Status &status) {
        std::vector<Token> re;
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
                else {
                    if (c == '+' || c == '-') {
                        bool t = it == str.begin();
                        if (!t) {
                            auto sym = re.rbegin()->symbol;
                            t = sym == Symbol::Operator || sym == Symbol::Left_Bracket || sym == Symbol::Comma ||
                                sym == Symbol::Equal;
                        }
                        if (t) {
                            re.push_back({{it, it + 1}, Symbol::Operator,
                                          c == '+' ? Operator::Positive : Operator::Negative});
                        } else {
                            re.push_back({{it, it + 1}, Symbol::Operator,
                                          c == '+' ? Operator::Plus : Operator::Minus});
                        }
                    } else {
                        re.push_back({{it, it + 1}, Symbol::Operator, get_operator(c)});
                    }
                }
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

    inline std::vector<Token>::iterator equal_check(Status &status, std::vector<Token> &list) {
        std::vector<std::vector<Token>::iterator> eq_pos;
        eq_pos.reserve(4);
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->symbol == Symbol::Equal)
                eq_pos.push_back(it);
        }
        if (eq_pos.size() > 1) {
            for (auto p: eq_pos) {
                status.hints.push_back({get_loc(status.origin, p->content),
                                        get_loc(status.origin, p->content)});
            }
            status.message = Error::MULTIPLE_EQUALS;
            return {};
        }
        return eq_pos.empty() ? list.end() : eq_pos[0];
    }

    inline bool check_brackets(Status &status, std::vector<Token> &list, std::vector<Token>::iterator start,
                               std::vector<Token>::iterator end) {
        std::stack<u64> left_brackets_mis;
        std::stack<bool> has_content;
        for (; start < end; ++start) {
            u64 loc = get_loc(status.origin, start->content);
            if (start->symbol == Symbol::Left_Bracket) {
                left_brackets_mis.push(loc);
                has_content.push(false);
            }
            if ((!has_content.empty()) && (start->symbol == Symbol::Function || start->symbol == Symbol::Variable ||
                                           start->symbol == Symbol::Number)) {
                has_content.top() = true;
            }
            if (start->symbol == Symbol::Right_Bracket) {
                if (left_brackets_mis.empty()) {
                    status.hints = {{loc, loc}};
                    status.message = Error::BRACKETS_MISMATCH;
                    return false;
                }
                if (!has_content.top()) {
                    status.hints = {{left_brackets_mis.top(), loc}};
                    status.message = Error::ILLEGAL_BRACKETS_PAIR;
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

    inline bool
    check_assign(Status &status, const std::vector<Token> &list, const std::vector<Token>::iterator &eq_pos) {
        if (list.front().symbol == Symbol::Equal) {
            status.message = Error::ASSIGN_VARIABLE_EMPTY;
            status.hints = {{get_loc(status.origin, list.front().content),
                             get_loc(status.origin, list.front().content)}};
            return false;
        }
        if (list.back().symbol == Symbol::Equal) {
            status.message = Error::ASSIGN_EXPR_EMPTY;
            status.hints = {{get_loc(status.origin, list.back().content),
                             get_loc(status.origin, list.back().content)}};
            return false;
        }
        auto it = ++list.begin();
        if (it->symbol != Symbol::Equal) {
            status.message = Error::ILLEGAL_ASSIGN;
            status.hints = {{get_loc(status.origin, eq_pos->content),
                             get_loc(status.origin, eq_pos->content)}};
            return false;
        }
        if (list.front().symbol != Symbol::Variable) {
            status.message = Error::ASSIGN_NOT_VARIABLE;
            status.hints = {{get_loc(status.origin, list.front().content),
                             get_loc(status.origin, list.front().content, list.front().content.size())}};
            return false;
        }
        if (variables.find(std::string(list.front().content)) == variables.end())
            variables[std::string(list.front().content)] = Number();
        return true;
    }

    inline bool check_function_name(Status &status, std::vector<Token> &list) {
        for (auto &it: list) {
            if (it.symbol == Symbol::Function) {
                it.function_id = get_function_id(it.content);
                if (it.function_id == -1) {
                    status.message = Error::ILLEGAL_FUNCTION_NAME;
                    status.hints = {{get_loc(status.origin, it.content),
                                     get_loc(status.origin, it.content, it.content.size() - 1)}};
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
                status.hints.push_back({get_loc(status.origin, str, d),
                                        get_loc(status.origin, str, d)});
            }
            return false;
        }
        if (power.size() > 1) {
            status.message = Error::MULTIPLE_POWERS;
            for (auto d: power) {
                status.hints.push_back({get_loc(status.origin, str, d),
                                        get_loc(status.origin, str, d)});
            }
            return false;
        }
        if (str.size() == (power.size() + dot.size())) {
            status.message = Error::ILLEGAL_NUMBER;
            status.hints = {
                    {
                            get_loc(status.origin, str),
                            get_loc(status.origin, str, str.size() - 1),
                    }
            };
            return false;
        }
        if ((!power.empty()) && (!dot.empty())) {
            if (dot[0] > power[0]) {
                status.message = Error::ILLEGAL_NUMBER;
                status.hints = {
                        {
                                get_loc(status.origin, str, power[0]),
                                get_loc(status.origin, str, power[0]),
                        },
                        {
                                get_loc(status.origin, str, dot[0]),
                                get_loc(status.origin, str, dot[0]),
                        },
                };
            }
        }
        num = Number(str);
        return true;
    }

    inline bool check_number(Status &status, std::vector<Token> &list) {
        for (auto &tk: list) {
            if (tk.symbol == Symbol::Number) {
                if (!check_number(tk.content, status, tk.value))return false;
            }
        }
        return true;
    }

    inline bool check_variable_name(Status &status, std::vector<Token> &list) {
        for (auto &tk: list) {
            if (tk.symbol == Symbol::Variable) {
                if (variables.find(std::string(tk.content)) == variables.end()) {
                    status.message = Error::UNDEFINED_VARIABLE;
                    status.hints = {{get_loc(status.origin, tk.content),
                                     get_loc(status.origin, tk.content, tk.content.size() - 1)}};
                    return false;
                }
                if (Command::commands.find(std::string(tk.content)) != Command::commands.end()) {
                    status.message = Error::ILLEGAL_VARIABLE_NAME;
                    status.hints = {{get_loc(status.origin, tk.content),
                                     get_loc(status.origin, tk.content, tk.content.size() - 1)}};
                    return false;
                }
            }
        }
        return true;
    }

    std::vector<Token>::iterator get_matched_bracket(std::vector<Token> &list, std::vector<Token>::iterator bra) {
        u64 matched = 0;
        for (; bra < list.end(); ++bra) {
            if (bra->symbol == Symbol::Left_Bracket)matched++;
            if (bra->symbol == Symbol::Right_Bracket)matched--;
            if (matched == 0)return bra;
        }
        return {};
    }

    bool check_expr(Status &status, std::vector<Token> &list, std::vector<Token>::iterator start,/*NOLINT*/
                    std::vector<Token>::iterator end) {
        bool re = true;
        re &= check_brackets(status, list, start, end);
        if (!re)return false;
        auto border_left = start;
        for (; start != end; ++start) {
            if (start->symbol == Symbol::Left_Bracket) {
                if (start != list.begin() && start->symbol != Symbol::Equal) {
                    auto tmp = start - 1;
                    if ((tmp->symbol != Symbol::Left_Bracket) &&
                        (tmp->symbol != Symbol::Operator) &&
                        (tmp->symbol != Symbol::Comma) &&
                        (tmp->symbol != Symbol::Function)) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, tmp->content),
                                         get_loc(status.origin, start->content)}};
                        return false;
                    }
                }
                auto new_start = get_matched_bracket(list, start);
                if (!check_expr(status, list, start + 1, get_matched_bracket(list, start)))
                    return false;
                start = new_start;
                continue;
            }
            if (start->symbol == Symbol::Number || start->symbol == Symbol::Variable) {
                //check left
                if (start != border_left) {
                    auto tmp = start - 1;
                    if (tmp->symbol == Symbol::Number || tmp->symbol == Symbol::Variable ||
                        tmp->symbol == Symbol::Function || tmp->symbol == Symbol::Right_Bracket) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, tmp->content),
                                         get_loc(status.origin, start->content, start->content.size() - 1)}};
                        return false;
                    }
                }
                //check right
                if (end != (start + 1)) {
                    auto tmp = (start + 1);
                    if (tmp->symbol == Symbol::Number || tmp->symbol == Symbol::Variable ||
                        tmp->symbol == Symbol::Function || tmp->symbol == Symbol::Left_Bracket) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, start->content),
                                         get_loc(status.origin, tmp->content, tmp->content.size() - 1)}};
                        return false;
                    }
                }
            }
            if (start->symbol == Symbol::Function) {
                if (start != border_left) {
                    auto tmp = start - 1;
                    if (tmp->symbol == Symbol::Number || tmp->symbol == Symbol::Variable ||
                        tmp->symbol == Symbol::Function || tmp->symbol == Symbol::Right_Bracket) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, tmp->content),
                                         get_loc(status.origin, start->content, start->content.size() - 1)}};
                        return false;
                    }
                }
                if (end == (start + 1)) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, start->content, start->content.size() - 1)}};
                    return false;
                }
                if ((start + 1)->symbol != Symbol::Left_Bracket) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, (start + 1)->content, (start + 1)->content.size() - 1)}};
                    return false;
                }
            }
            if (start->symbol == Symbol::Operator) {
                if (start + 1 == end) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, start->content)}};
                    return false;
                }
                if ((start + 1)->symbol != Symbol::Number && (start + 1)->symbol != Symbol::Variable &&
                    (start + 1)->symbol != Symbol::Function && (start + 1)->symbol != Symbol::Operator &&
                    (start + 1)->symbol != Symbol::Left_Bracket) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, (start + 1)->content, (start + 1)->content.size() - 1)}};
                    return false;
                }
                if ((start + 1)->symbol == Symbol::Operator) {
                    bool b = (start + 1)->op != Operator::Mod &&
                             (start + 1)->op != Operator::Multiply &&
                             (start + 1)->op != Operator::Divide;
                    if (!b) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, start->content),
                                         get_loc(status.origin, (start + 1)->content,
                                                 (start + 1)->content.size() - 1)}};
                        return false;
                    }
                }
                if (start == border_left) {
                    if (start == list.begin() || (start - 1)->symbol == Symbol::Equal) {
                        if (start->op != Operator::Positive && start->op != Operator::Negative) {
                            status.message = Error::SYNTAX_ERROR;
                            status.hints = {{get_loc(status.origin, (start - 1)->content),
                                             get_loc(status.origin, start->content)}};
                            return false;
                        }
                    }
                } else {
                    auto tmp = start - 1;
                    if (tmp->symbol == Symbol::Function) {
                        status.message = Error::SYNTAX_ERROR;
                        status.hints = {{get_loc(status.origin, tmp->content),
                                         get_loc(status.origin, start->content, start->content.size() - 1)}};
                        return false;
                    }
                    if (tmp->symbol == Symbol::Comma) {
                        if (start->op != Operator::Positive && start->op != Operator::Negative) {
                            status.message = Error::SYNTAX_ERROR;
                            status.hints = {{get_loc(status.origin, tmp->content),
                                             get_loc(status.origin, start->content)}};
                            return false;
                        }
                    }
                }
            }
            if (start->symbol == Symbol::Comma) {
                if (start == list.begin() || (start - 1)->symbol == Symbol::Equal || start == end - 1) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, start->content)}};
                    return false;
                }
                auto tmp = start - 1;
                if ((tmp->symbol != Symbol::Number) &&
                    (tmp->symbol != Symbol::Variable) &&
                    (tmp->symbol != Symbol::Right_Bracket)) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, tmp->content),
                                     get_loc(status.origin, start->content, start->content.size() - 1)}};
                    return false;
                }
                tmp = start + 1;
                if ((tmp->symbol == Symbol::Right_Bracket) || (tmp->symbol == Symbol::Comma) ||
                    ((tmp->symbol == Symbol::Operator) && (tmp->op != Operator::Positive) &&
                     (tmp->op != Operator::Negative))) {
                    status.message = Error::SYNTAX_ERROR;
                    status.hints = {{get_loc(status.origin, start->content),
                                     get_loc(status.origin, tmp->content, tmp->content.size() - 1)}};
                    return false;
                }
            }
        }
        return true;
    }

    inline bool check_function_call(Status &status, std::vector<Token> &list) {
        std::stack<std::vector<Token>::iterator> funcs;
        std::stack<std::vector<Token>::iterator> right_bras;
        std::stack<bool> has_comma;
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->symbol == Symbol::Function) {
                funcs.push(it);
                right_bras.push(++it);
                has_comma.push(false);
            }
            while ((!right_bras.empty()) && it != right_bras.top()) {
                if (it->symbol == Symbol::Comma) {
                    auto i = funcs.top();
                    if (i->function_id < 11 || has_comma.top()) {
                        status.message = Error::ILLEGAL_FUNCTION_INPUT;
                        status.hints = {{get_loc(status.origin, i->content),
                                                get_loc(status.origin, i->content, i->content.size() - 1)},
                                        {get_loc(status.origin, it->content),
                                                get_loc(status.origin, it->content)}};
                        return false;
                    }
                    has_comma.top() = true;
                }
            }
            if (!funcs.empty())
                funcs.pop();
            if (!right_bras.empty())
                right_bras.pop();
            if (!has_comma.empty())
                has_comma.pop();
        }
        return true;
    }

    inline bool check(Status &status, std::vector<Token> &list) {
        auto eq_pos = equal_check(status, list);
        if (eq_pos == decltype(eq_pos)())return false;
        bool has_eq = eq_pos != list.end();
        auto start = has_eq ? list.begin() + 2 : list.begin();
        if (!check_brackets(status, list, start, list.end()))return false;
        if (has_eq && (!check_assign(status, list, eq_pos)))return false;
        if (!(check_function_name(status, list) && check_number(status, list)))return false;
        if (!check_variable_name(status, list))return false;
        if (!check_expr(status, list, start, list.end()))return false;
        if (!check_function_call(status, list))return false;
        return true;
    }

    inline Number evaluate(Status &status, std::vector<Token> &expr) {
        std::stack<Token> nums;
        for (auto &tk: expr) {
            if (tk.symbol == Symbol::Number || tk.symbol == Symbol::Variable)
                nums.push(tk);
            else if (tk.symbol == Symbol::Function) {
                auto func = functions[tk.function_id];
                if (tk.function_id < 11) {
                    if (tk.function_id == 3 || tk.function_id == 5 || tk.function_id == 6) {
                        if (!nums.top().value.sign) {
                            status.message = Error::ILLEGAL_FUNCTION_INPUT;
                            status.hints = {{get_loc(status.origin, nums.top().content),
                                             get_loc(status.origin, nums.top().content)}};
                            return {};
                        }
                    }
                    nums.top().value = reinterpret_cast<Number(*)(const Number &)>(func)(nums.top().value);
                } else {
                    auto tmp_r = nums.top();
                    nums.pop();
                    auto tmp_l = nums.top();
                    if (tk.function_id == 11) {
                        if ((!tmp_l.value.sign) && (tmp_r.value.power < 0)) {
                            status.message = Error::ILLEGAL_FUNCTION_INPUT;
                            status.hints = {{get_loc(status.origin, tmp_l.content),
                                                    get_loc(status.origin, tmp_l.content)},
                                            {get_loc(status.origin, tmp_r.content),
                                                    get_loc(status.origin, tmp_r.content)}};
                            return {};
                        }
                    }
                    nums.top().value = reinterpret_cast<Number(*)(const Number &, const Number &)>(func)(tmp_l.value,
                                                                                                         tmp_r.value);
                }
            } else {
                Token tmp_r, tmp_l;
                switch (tk.op) {
                    case Operator::Plus:
                        tmp_r = nums.top();
                        nums.pop();
                        tmp_l = nums.top();
                        nums.top().value = tmp_l.value + tmp_r.value;
                        break;
                    case Operator::Minus:
                        tmp_r = nums.top();
                        nums.pop();
                        tmp_l = nums.top();
                        nums.top().value = tmp_l.value - tmp_r.value;
                        break;
                    case Operator::Multiply:
                        tmp_r = nums.top();
                        nums.pop();
                        tmp_l = nums.top();
                        nums.top().value = tmp_l.value * tmp_r.value;
                        break;
                    case Operator::Divide:
                        tmp_r = nums.top();
                        nums.pop();
                        tmp_l = nums.top();
                        if (tmp_r.value.isZero()) {
                            status.message = Error::DIVIDED_BY_ZERO;
                            status.hints = {{get_loc(status.origin, tmp_r.content),
                                             get_loc(status.origin, tmp_r.content)}};
                            return {};
                        }
                        nums.top().value = tmp_l.value / tmp_r.value;
                        break;
                    case Operator::Mod:
                        tmp_r = nums.top();
                        nums.pop();
                        tmp_l = nums.top();
                        if (tmp_r.value.isZero()) {
                            status.message = Error::DIVIDED_BY_ZERO;
                            status.hints = {{get_loc(status.origin, tmp_r.content),
                                             get_loc(status.origin, tmp_r.content)}};
                            return {};
                        }
                        if (tmp_l.value.power < 0 || tmp_r.value.power < 0) {
                            status.message = Error::FLOAT_MOD;
                            status.hints.clear();
                            if (tmp_l.value.power < 0) {
                                status.hints.push_back({get_loc(status.origin, tmp_l.content),
                                                        get_loc(status.origin, tmp_l.content)});
                            }
                            if (tmp_r.value.power < 0) {
                                status.hints.push_back({get_loc(status.origin, tmp_r.content),
                                                        get_loc(status.origin, tmp_r.content)});
                            }
                            return {};
                        }
                        nums.top().value = (tmp_l.value % tmp_r.value).second;
                        break;
                    case Operator::Negative:
                        nums.top().value.sign = !nums.top().value.sign;
                        break;
                    case Operator::Positive:
                        break;
                }
            }
        }
        return nums.top().value;
    }

    inline bool process(Status &status) {
        auto list = preprocess(status);
        if (!check(status, list))return false;
        std::stack<Token> tmp;
        std::vector<Token> expr;
        expr.reserve(list.size());
        std::vector<Token>::iterator func2, it;
        bool ass = list[1].symbol == Symbol::Equal;
        it = ass ? list.begin() + 2 : list.begin();
        for (; it != list.end(); ++it) {
            switch (it->symbol) {
                case Symbol::Number:
                    expr.push_back(*it);
                    break;
                case Symbol::Variable:
                    it->value = variables[std::string(it->content)];
                    expr.push_back(*it);
                    break;
                case Symbol::Left_Bracket:
                    tmp.push(*it);
                    break;
                case Symbol::Right_Bracket:
                    while (tmp.top().symbol != Symbol::Left_Bracket) {
                        expr.push_back(tmp.top());
                        tmp.pop();
                    }
                    tmp.pop();
                    break;
                case Symbol::Operator:
                    if (it->op == Operator::Positive)continue;
                    if (tmp.empty() || tmp.top().symbol == Symbol::Left_Bracket ||
                        get_priority(tmp.top()) <= get_priority(*it))
                        tmp.push(*it);
                    else {
                        while (((!tmp.empty()) && get_priority(tmp.top()) > get_priority(*it))) {
                            if (tmp.top().symbol == Symbol::Left_Bracket)break;
                            expr.push_back(tmp.top());
                            tmp.pop();
                        }
                        tmp.push(*it);
                    }
                    break;
                case Symbol::Function:
                    if (it->function_id < 11) {
                        tmp.push(*it);
                    } else {
                        func2 = it;
                    }
                    break;
                case Symbol::Comma:
                    while (tmp.top().symbol != Symbol::Left_Bracket) {
                        expr.push_back(tmp.top());
                        tmp.pop();
                    }
                    tmp.pop();


                    tmp.push(*func2);


                    tmp.push({{}, Symbol::Left_Bracket});
                    break;

                default:
                    break;
            }

        }
        while (!tmp.empty()) {
            expr.push_back(tmp.top());
            tmp.pop();
        }
        status.message.clear();
        auto num = evaluate(status, expr);
        if (!status.message.empty()) {
            return false;
        }
        if (ass) {
            variables[std::string(list[0].content)] = num;
            history.push_back(status.origin);
        } else {
            history.push_back(status.origin + ": ");
            std::stringstream ss;
            std::cout << num << "\n";
            ss << num;
            history.push_back(ss.str());
        }
        return true;
    }


    bool process_input(Status &status) {
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