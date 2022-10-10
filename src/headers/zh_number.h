//
// Created by zhzhzh on 2022/10/4.
//

#ifndef PROJECT2_ZH_NUMBER_H
#define PROJECT2_ZH_NUMBER_H

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

enum class Base {
    Binary = 2,
    Octal = 8,
    Decimal = 10,
    Hexadecimal = 16
};

const std::string bi = "0b";
const std::string oct = "0";
const std::string dec;
const std::string hex = "0x";

const std::string &prefix(Base b) {
    if (b == Base::Binary)return bi;
    if (b == Base::Octal)return oct;
    if (b == Base::Hexadecimal)return hex;
    return dec;
}

/*
class decimal {
public:
    mutable bool sign{POSITIVE};
    Base base = Base::Decimal;
    std::vector<uint8_t> nums;//[____1-9], reverse store, 12300 == {0, 0, 3, 2, 1}

    decimal() = default;

    explicit decimal(uint64_t num);

    decimal(bool s, std::vector<uint8_t> nm) : sign(s), nums(std::move(nm)) {}

    decimal operator+(const decimal &other) const;

    decimal operator-(const decimal &other) const;

    decimal operator*(const decimal &other) const;

    decimal operator/(const decimal &other);

    friend std::ostream &operator<<(std::ostream &os, const decimal &d);//just for test

    bool operator<(const decimal &other) const;//abs compare
};
*/
class zh_number {
public:
    static size_t precision;
    static Print_Tag tag;
    bool sign{POSITIVE};
    Base base = Base::Decimal;
    std::vector<uint8_t> nums;
    int64_t power{0};

    std::strong_ordering operator<=>(zh_number &other);

    bool operator==(zh_number &other);

    zh_number operator+(zh_number &other);

    zh_number operator-(zh_number &other);

    zh_number operator*(zh_number &other);

    zh_number operator/(zh_number &other);

    std::pair<zh_number, zh_number> operator%(zh_number &other);

    void operator+=(zh_number &other);

    void operator-=(zh_number &other);

    void operator*=(zh_number &other);

    void operator/=(zh_number &other);

    void operator%=(zh_number &other);

    friend std::ostream &operator<<(std::ostream &os, zh_number &n);

    [[nodiscard]] bool isZero();

    void squeeze();

    void pad_zeros(int64_t count);
};


#endif //PROJECT2_ZH_NUMBER_H
