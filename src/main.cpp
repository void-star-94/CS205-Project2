#include "zh_number.h"
#include <cstdlib>
#include <algorithm>

uint64_t zh_number::precision = 32;
Print_Tag zh_number::tag = Print_Tag::Scientific;


int main() {
    uint64_t tmp = 114514;
//    zh_number::tag = Print_Tag::Decimal;
    zh_number a(000);
    zh_number b(1919810);
    zh_number re = a / b;
    re.squeeze();
//    a -= b;
    std::cout << (b % a).first;
    std::cout << "\n------------------------\n";
    std::cout << (b / a);


    return 0;
}
