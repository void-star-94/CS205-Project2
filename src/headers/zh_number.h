//
// Created by zhzhzh on 2022/10/4.
//

#ifndef PROJECT2_ZH_NUMBER_H
#define PROJECT2_ZH_NUMBER_H

#include <utility>
#include <vector>
#include <cstdint>
#include <iostream>

#define POSITIVE true
#define NEGATIVE false
enum class Print_Tag {
    Scientific,
    Fractional,
    Decimal
};


class zh_number {
public:
    static size_t precision;
    static Print_Tag tag;
    mutable bool sign{POSITIVE};
    std::vector<uint8_t> nums;
    int64_t power{0};

    zh_number(bool s, std::vector<uint8_t> n, int64_t p) : sign(s), nums(std::move(n)), power(p) {}

    zh_number();

    explicit zh_number(uint64_t num, bool s = POSITIVE, int64_t power_ = 0);

    std::strong_ordering operator<=>(const zh_number &other) const;

    bool operator==(const zh_number &other) const;

    zh_number operator+(const zh_number &other) const;

    zh_number operator-(const zh_number &other) const;

    zh_number operator*(const zh_number &other) const;

    zh_number operator/(const zh_number &other) const;

    std::pair<zh_number, zh_number> operator%(const zh_number &other) const;

    void operator+=(const zh_number &other);

    void operator-=(const zh_number &other);

    void operator*=(const zh_number &other);

    void operator/=(const zh_number &other);

    void operator%=(const zh_number &other);

    zh_number &operator++();

    zh_number &operator--();

    friend std::ostream &operator<<(std::ostream &os, const zh_number &n);

    [[nodiscard]] bool isZero() const;

    void squeeze();

    void pad_zeros(int64_t count);

    bool degenerate() const;

    int64_t to_int() const;

    bool is_pow_10() const;

};


#endif //PROJECT2_ZH_NUMBER_H
