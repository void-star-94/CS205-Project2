//
// Created by void-star on 2022/10/4.
//

#ifndef PROJECT2_NUMBER_H
#define PROJECT2_NUMBER_H

#include "Defines.h"
#include <iostream>
#include <limits>

namespace void_star {
#define POSITIVE true
#define NEGATIVE false
    enum class Print_Tag {
        Scientific,
        Fractional,
        Decimal
    };


    class Number {
    public:
        static size_t precision;
        static Print_Tag tag;
        mutable bool sign{POSITIVE};
        std::vector<u8> nums;
        i64 power{0};

        Number(bool s, std::vector<u8> n, i64 p) : sign(s), nums(std::move(n)), power(p) {}

        Number();

        explicit Number(const std::string_view &);

        explicit Number(u64 num, bool s = POSITIVE, i64 power_ = 0);

        std::strong_ordering operator<=>(const Number &other) const;

        bool operator==(const Number &other) const;

        Number operator+(const Number &other) const;

        Number operator-(const Number &other) const;

        Number operator*(const Number &other) const;

        Number operator/(const Number &other) const;

        std::pair<Number, Number> operator%(const Number &other) const;

        void operator+=(const Number &other);

        void operator-=(const Number &other);

        void operator*=(const Number &other);

        void operator/=(const Number &other);

        void operator%=(const Number &other);

        Number &operator++();

        Number &operator--();

        friend std::ostream &operator<<(std::ostream &os, const Number &n);

        [[nodiscard]] bool isZero() const;

        void squeeze();

        void pad_zeros(i64 count);

        bool degenerate() const;

        i64 to_int() const;

        bool is_pow_10() const;

    };

    static const Number illegal = Number(1, POSITIVE, std::numeric_limits<i64>::max());

}
#endif //PROJECT2_NUMBER_H
