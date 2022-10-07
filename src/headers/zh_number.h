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
enum class print_tag {
    Scientific,
    Fractional,
    Decimal
};

class decimal {
public:
    mutable bool sign{POSITIVE};
    std::vector<uint8_t> nums;//[____1-9], reverse store, 12300 == {0, 0, 3, 2, 1}

    decimal() = default;

    explicit decimal(uint64_t num);

    decimal(bool s, std::vector<uint8_t> nm) : sign(s), nums(std::move(nm)) {}

    decimal operator+(const decimal &other) const;

    decimal operator-(const decimal &other) const;

    decimal operator*(const decimal &other);

    decimal operator/(const decimal &other);

    friend std::ostream &operator<<(std::ostream &os,const decimal&d);//just for test

    bool operator<(const decimal &other) const;//abs compare
};

class zh_number {
public:
    static size_t precision;
    static print_tag tag;
    decimal value;
    int64_t power{0};

    zh_number operator+(zh_number &other);

    zh_number operator-(zh_number &other);

    zh_number operator*(zh_number &other);

    zh_number operator/(zh_number &other);

    std::pair<zh_number, zh_number> operator%(zh_number &other);

    friend std::ostream &operator<<(std::ostream &os,zh_number& n);

    [[nodiscard]] bool isZero();

    void squeeze();

    void pad_zeros(int64_t count);
};


#endif //PROJECT2_ZH_NUMBER_H
