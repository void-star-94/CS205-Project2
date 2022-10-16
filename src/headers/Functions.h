//
// Created by void-star on 2022/10/12.
//

#ifndef PROJECT2_FUNCTIONS_H
#define PROJECT2_FUNCTIONS_H

#include <initializer_list>

namespace void_star {
    class Number;

    long double to_float(const Number &num);

    Number to_number(long double num);

    Number floor(const Number &);

    Number ceil(const Number &);

    Number round(const Number &);

    Number sqrt(const Number &);

    Number exp(const Number &);

    Number log(const Number &);

    Number log10(const Number &);

    Number sin(const Number &);

    Number cos(const Number &);

    Number tan(const Number &);

    Number abs(const Number &);

    Number power(const Number &, const Number &);

    Number max(const Number &, const Number &);

    Number min(const Number &, const Number &);

    static void (*functions[])() ={
            reinterpret_cast<void (*)()>(floor),
            reinterpret_cast<void (*)()>(ceil),
            reinterpret_cast<void (*)()>(round),
            reinterpret_cast<void (*)()>(sqrt),
            reinterpret_cast<void (*)()>(exp),
            reinterpret_cast<void (*)()>(log),
            reinterpret_cast<void (*)()>(log10),
            reinterpret_cast<void (*)()>(sin),
            reinterpret_cast<void (*)()>(cos),
            reinterpret_cast<void (*)()>(tan),
            reinterpret_cast<void (*)()>(abs),
            reinterpret_cast<void (*)()>(power),
            reinterpret_cast<void (*)()>(max),
            reinterpret_cast<void (*)()>(min),
    };
}
#endif //PROJECT2_FUNCTIONS_H
