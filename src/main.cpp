#include "Executor.h"
#include <sstream>
#include <iostream>

int main() {
//    while (true) {
//        auto status = get_input();
//        if (!process_input(status))print_error(status);
//    }
    for (int i = 'A'; i < 'Z' + 1; ++i) {
        std::cout << '\'' << char(i) << "', ";
    }

}
