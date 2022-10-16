//
// Created by void-star on 2022/10/12.
//
#include "Functions.h"
#include "Number.h"
#include <cmath>
#include <sstream>

namespace void_star {

    long double to_float(const Number &num) {
        long double re = 0;
        for (u64 i = 0; i < num.nums.size(); ++i) {
            re += num.nums[i] * powl(10, i);
        }
        re *= powl(10, num.power);
        if (!num.sign)re *= -1;
        return re;
    }

    Number to_number(long double num) {
        bool neg = num < 0;
        if (neg)num = -num;
        std::stringstream ss;
        ss << num;
        Number re(std::string_view(ss.str()));
        if (neg)re.sign = NEGATIVE;
        return re;
    }

    Number floor(const Number &num) {
        if (num.isZero())return {};
        if (num.power >= 0)return num;
        i64 tmp = num.power + i64(num.nums.size());
        if (tmp <= 0)
            return num.sign ? Number(0) : Number(1, NEGATIVE);
        Number re = num;
        re.nums.erase(re.nums.begin(), re.nums.begin() - num.power);
        re.power = 0;
        re.squeeze();
        return re.sign ? re : --re;
    }

    Number ceil(const Number &num) {
        if (num.isZero())return {};
        if (num.power >= 0)return num;
        i64 tmp = num.power + i64(num.nums.size());
        if (tmp <= 0)
            return num.sign ? Number(1) : Number(0);
        Number re = num;
        re.nums.erase(re.nums.begin(), re.nums.begin() + tmp);
        re.power = 0;
        re.squeeze();
        return re.sign ? ++re : re;
    }

    Number round(const Number &num) {
        return (num - floor(num)) < Number(5, POSITIVE, -1) ? floor(num) : ceil(num);
    }

    Number sqrt(const Number &num) {
        return to_number(sqrtl(to_float(num)));
    }

    Number exp(const Number &num) {
        return to_number(expl(to_float(num)));
    }

    Number log(const Number &num) {
        return to_number(logl(to_float(num)));
    }

    Number log10(const Number &num) {
        return to_number(log10l(to_float(num)));
    }

    Number sin(const Number &num) {
        return to_number(sinl(to_float(num)));
    }

    Number cos(const Number &num) {
        return to_number(cosl(to_float(num)));
    }

    Number tan(const Number &num) {
        return to_number(tanl(to_float(num)));
    }

    Number abs(const Number &num) {
        auto tmp = num;
        tmp.sign = POSITIVE;
        return tmp;
    }

    Number power(const Number &num1, const Number &num2) {
        return to_number(powl(to_float(num1), to_float(num2)));
    }

    Number max(const Number &num1, const Number &num2) {
        return num1 > num2 ? num1 : num2;
    }

    Number min(const Number &num1, const Number &num2) {
        return num1 > num2 ? num2 : num1;
    }

}