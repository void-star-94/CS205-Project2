#include "Executor.h"


int main() {

    while (true) {
        auto status = void_star::get_input();
        if (!void_star::process_input(status))void_star::print_error(status);
    }

}
