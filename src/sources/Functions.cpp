//
// Created by 45098 on 2022/10/12.
//
#include "Functions.h"
#include "Number.h"

Number floor(const Number &num) {
    if (num.isZero())return {};
    if (num.power >= 0)return num;
    int64_t tmp = num.power + int64_t(num.nums.size());
    if (tmp <= 0)
        return num.sign ? Number(0) : Number(1, NEGATIVE);
    Number re = num;
    re.nums.erase(re.nums.begin(), re.nums.begin() + tmp);
    re.power = 0;
    re.squeeze();
    return re.sign ? re : --re;
}