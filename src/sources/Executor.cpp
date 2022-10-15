//
// Created by void-star on 2022/10/13.
//
#include "Executor.h"
#include <iostream>

Package get_input() {
    std::string first_line;
    std::getline(std::cin, first_line);
    bool b = true;
    for (char c: first_line) {
        b &= calculation.find(c) != calculation.end();
        if (!b)break;
    }
    if (b) {
        return {Expression::Formula, {std::move(first_line)}};
    }
    Package re;
    auto &blk = re.blk;
    blk.reserve(16);
    blk.push_back(std::move(first_line));
    for (u64 i = 1;; ++i) {
        std::string line;
        std::getline(std::cin, line);
        blk.push_back(std::move(line));
        if (Command::commands.find(blk[i]) == Command::commands.end()) {
            return re;
        }
    }
}

void check_block(Block &blk) {

}

void process_block(Block &blk) {

}