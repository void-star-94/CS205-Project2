#include "zh_number.h"

uint64_t zh_number::precision = 128;
print_tag zh_number::tag = print_tag::Scientific;


int main() {
    zh_number a = {{POSITIVE, {2, 5, 6}}, 15};
    zh_number b = {{POSITIVE, {7, 3, 9}}, 15};
    zh_number re = a - b;
    zh_number::tag = print_tag::Decimal;
    std::cout << re << "\n";
    std::cout << "------------------------\n";


    return 0;
}
