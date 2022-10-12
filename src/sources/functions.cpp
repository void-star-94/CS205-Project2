//
// Created by 45098 on 2022/10/12.
//
#include "functions.h"
#include "zh_number.h"

zh_number floor(const zh_number &num) {
    if (num.isZero())return {};
    if (num.power >= 0)return num;
    int64_t tmp = num.power + int64_t(num.nums.size());
    if (tmp <= 0)
        return num.sign ? zh_number(0) : zh_number(1, NEGATIVE);
    zh_number re = num;
    re.nums.erase(re.nums.begin(), re.nums.begin() + tmp);
    re.power = 0;
    re.squeeze();
    return re.sign ? re : --re;
}