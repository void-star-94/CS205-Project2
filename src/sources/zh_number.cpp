//
// Created by zhzhzh on 2022/10/4.
//
#include "zh_number.h"
#include <algorithm>
#include <stdexcept>


const zh_number zero = {{POSITIVE, {}},
                        0};
const zh_number one = {{POSITIVE, {1}},
                       0};
const zh_number negative_one = {{NEGATIVE, {1}}, 0};


std::vector<uint8_t> operator+(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
    if (a.empty())return a;
    if (b.empty())return b;
    const std::vector<uint8_t> &a_ = a.size() > b.size() ? a : b;
    const std::vector<uint8_t> &b_ = a.size() > b.size() ? b : a;
    std::vector<uint8_t> re;
    re.reserve(zh_number::precision);
    uint8_t carry = 0, sum;
    for (size_t i = 0; i < a_.size(); ++i) {
        sum = a_[i] + carry + (i < b_.size()) * b_[i];
        carry = sum > 10 ? 1 : 0;
        re.push_back(carry ? sum - 10 : sum);
    }
    if (carry)
        re.push_back(1);
    return re;
}

std::vector<uint8_t> operator-(const std::vector<uint8_t> &l, const std::vector<uint8_t> &s) {// l>s
    if (s.empty() || l.empty())return l;
    uint8_t borrow = 0, borrow_;
    std::vector<uint8_t> re;
    re.reserve(zh_number::precision);
    for (size_t i = 0; i < l.size(); ++i) {
        borrow_ = borrow;
        uint8_t tmp = (i < s.size()) * s[i];
        borrow = (borrow + tmp > l[i]);
        re.push_back(borrow * 10 + l[i] - borrow_ - tmp);
    }
    return re;
}

decimal decimal::operator+(const decimal &other) const {
    decimal re;
    if (sign xor other.sign) {
        re.sign = (*this < other) ? other.sign : sign;
        re.nums = re.sign ? nums - other.nums : other.nums - nums;
    } else {
        re.sign = sign;
        re.nums = nums + other.nums;
    }
    return re;
}

decimal decimal::operator-(const decimal &other) const {
    other.sign = !other.sign;
    decimal re = *this + other;
    other.sign = !other.sign;
    return re;
}

decimal decimal::operator*(const decimal &other) {
    return decimal();
}

decimal decimal::operator/(const decimal &other) {
    return decimal();
}

std::ostream &operator<<(std::ostream &os, const decimal &d) {
    auto &v = d.nums;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {/*NOLINT*/
        os << int(*it);
    }
    return os;
}

bool decimal::operator<(const decimal &other) const {
    if (nums.size() < other.nums.size())return true;
    if (nums.size() > other.nums.size())return false;
    uint64_t i = nums.size();
    for (; i > 0; --i) {
        if (nums[i] < other.nums[i])return true;
        if (nums[i] > other.nums[i])return true;
    }
    return nums[0] < other.nums[0];
}

decimal::decimal(uint64_t num) {
    sign = POSITIVE;
    nums.reserve(zh_number::precision);
    while (num) {
        nums.push_back(num % 10);
        num /= 10;
    }
}


zh_number zh_number::operator+(zh_number &other) {
    squeeze();
    other.squeeze();
    if (isZero())return other;
    if (other.isZero())return *this;
    int64_t tmp = std::abs(power - other.power);
    if (power < other.power) {
        other.pad_zeros(tmp);
    } else if (power > other.power) {
        pad_zeros(tmp);
    }
    zh_number re{value + other.value, power};
    re.squeeze();
    squeeze();
    other.squeeze();
    return re;
}

zh_number zh_number::operator-(zh_number &other) {
    other.value.sign = !other.value.sign;
    auto re = *this + other;
    other.value.sign = !other.value.sign;
    return re;
}

zh_number zh_number::operator*(zh_number &other) {
    return one;
}

zh_number zh_number::operator/(zh_number &other) {
    return one;
}

std::pair<zh_number, zh_number> zh_number::operator%(zh_number &other) {
    return {};
}

bool zh_number::isZero() {
    squeeze();
    return value.nums.empty();
}

void zh_number::squeeze() {
    if (value.nums[0] != 0)return;
    auto it = std::find_if(value.nums.begin(), value.nums.end(), [](uint8_t i) { return i != 0; });
    if (it == value.nums.end()) {
        *this = zero;
        return;
    }
    int64_t dis = std::distance(value.nums.begin(), it);
    value.nums.erase(value.nums.begin(), it);
    power += dis;
}

void zh_number::pad_zeros(const int64_t count) {
    if (count == 0)return;
    squeeze();
    std::vector<uint8_t> tmp(count, 0);
    value.nums.insert(value.nums.begin(), tmp.begin(), tmp.end());
    power -= count;
}

std::ostream &operator<<(std::ostream &os, zh_number &n) {
    n.squeeze();
    if (n.isZero()) {
        if (zh_number::tag == print_tag::Fractional)
            os << "0.0";
        else
            os << 0;
        return os;
    }
    auto &v = n.value.nums;
    if (!n.value.sign)os << '-';
    switch (zh_number::tag) {
        case print_tag::Scientific:
            os << int(*v.rbegin()) << '.';
            for (auto it = v.rbegin() + 1; it != v.rend(); ++it) {
                os << int(*it);
            }
            os << 'e' << n.power + v.size() - 1;
            break;
        case print_tag::Fractional:
            if (n.power >= 0) {
                os << n.value;
                for (int64_t i = 0; i < n.power; ++i) {
                    os << 0;
                }
                os << ".0";
            } else {
                if (v.size() > -n.power) {
                    uint64_t loc = v.size() + n.power, i = 0;
                    for (auto it = v.rbegin(); it != v.rend(); ++it, ++i) {
                        if (i == loc)os << '.';
                        os << int(*it);
                    }
                } else {
                    uint64_t l = -n.power - v.size();
                    os << "0.";
                    for (uint64_t i = 0; i < l; ++i) {
                        os << 0;
                    }
                    os << n.value;
                }
            }
            break;
        case print_tag::Decimal:
            if (n.power >= 0) {
                os << n.value;
                for (int64_t i = 0; i < n.power; ++i) {
                    os << 0;
                }
            } else {
                if (v.size() > -n.power) {
                    uint64_t loc = v.size() + n.power, i = 0;
                    for (auto it = v.rbegin(); it != v.rend(); ++it, ++i) {
                        if (i == loc)return os;
                        os << int(*it);
                    }
                } else {
                    os << 0;
                }
            }
            break;
    }
    return os;
}



