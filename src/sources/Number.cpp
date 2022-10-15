//
// Created by zhzhzh on 2022/10/4.
//
#include "Number.h"
#include <algorithm>
#include <stdexcept>
#include <complex>
#include "Functions.h"


Number zero = {POSITIVE,
               std::vector<u8>(), 0};
Number one = {POSITIVE,
              std::vector<u8>{1}, 0};
Number negative_one = {NEGATIVE,
                       std::vector<u8>{1}, 0};

inline bool abs_less(const Number &a, const Number &b) {
    u64 ll = a.nums.size() + a.power, rl = b.nums.size() + b.power;
    if (ll != rl)return ll <= rl;
    auto it_l = a.nums.rbegin(), it_r = b.nums.rbegin();
    for (; it_l != a.nums.rend() && it_r != b.nums.rend(); ++it_l, ++it_r) {
        if (*it_l != *it_r) {
            return *it_l < *it_r;
        }
    }
    return it_l == a.nums.rend();
}

Number::Number() {
    *this = zero;
}

Number::Number(u64 num, bool s, i64 power_) : sign(s), power(power_) {
    if (num == 0) {
        *this = zero;
    } else {
        nums.reserve(precision);
        while (num) {
            nums.push_back(num % 10);
            num /= 10;
        }
    }
    squeeze();
}

std::strong_ordering Number::operator<=>(const Number &other) const {
    if (sign xor other.sign)
        return sign ? std::strong_ordering::greater : std::strong_ordering::less;
    u64 ll = nums.size() + power, rl = other.nums.size() + other.power;
    if (ll != rl)return ll <=> rl;
    auto &v = other.nums;
    auto it_l = nums.rbegin(), it_r = v.rbegin();
    for (; it_l != nums.rend() && it_r != v.rend(); ++it_l, ++it_r) {
        if (*it_l != *it_r) {
            return sign ? *it_l <=> *it_r : *it_r <=> *it_l;
        }
    }
    if (it_l == nums.rend() && it_r == v.rend())
        return std::strong_ordering::equal;
    if (it_l == nums.rend())
        return sign ? std::strong_ordering::less : std::strong_ordering::greater;
    return sign ? std::strong_ordering::greater : std::strong_ordering::less;
}

bool Number::operator==(const Number &other) const {
    if (sign xor other.sign)return false;
    if ((power != other.power) || (nums.size() != other.nums.size()))return false;
    auto &v = other.nums;
    auto it_l = nums.begin(), it_r = v.begin();
    for (; it_l != nums.end(); ++it_l, ++it_r) {
        if (*it_l != *it_r) {
            return false;
        }
    }
    return true;
}

std::vector<uint8_t> operator+(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
    if (a.empty())return a;
    if (b.empty())return b;
    const std::vector<uint8_t> &a_ = a.size() > b.size() ? a : b;
    const std::vector<uint8_t> &b_ = a.size() > b.size() ? b : a;
    std::vector<uint8_t> re;
    re.reserve(Number::precision);
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
    re.reserve(Number::precision);
    for (size_t i = 0; i < l.size(); ++i) {
        borrow_ = borrow;
        uint8_t tmp = (i < s.size()) * s[i];
        borrow = (borrow + tmp > l[i]);
        re.push_back(borrow * 10 + l[i] - borrow_ - tmp);
    }
    if (*re.rbegin() == 0)re.erase(re.end() - 1);
    return re;
}

std::ostream &operator<<(std::ostream &os, const std::vector<uint8_t> &v) {
    for (auto it = v.rbegin(); it != v.rend(); ++it) {/*NOLINT*/
        os << int(*it);
    }
    return os;
}

Number Number::operator+(const Number &other_) const {
    if (isZero())return other_;
    if (other_.isZero())return *this;
    Number other = other_, tmp_ = *this;
    i64 tmp = std::abs(power - other_.power);
    if (power < other_.power) {
        other.pad_zeros(tmp);
    } else if (power > other_.power) {
        tmp_.pad_zeros(tmp);
    }
    Number re;
    if (tmp_.degenerate() && other.degenerate()) {
        i64 num1 = tmp_.to_int(), num2 = other.to_int();
        i64 num3 = num1 + num2;
        if (num3 == 0)return zero;
        re = num3 > 0 ? Number(num3, POSITIVE, other.power)
                      : Number(-num3, NEGATIVE, other.power);
        re.squeeze();
        return re;
    }

    bool b = abs_less(*this, other);
    re.power = tmp_.power;
    if (sign xor other.sign) {
        re.sign = b ? other.sign : sign;
        re.nums = b ? other.nums - tmp_.nums : tmp_.nums - other.nums;
        auto it = std::find_if(re.nums.rbegin(), re.nums.rend(), [](uint8_t i) { return i != 0; });
        i64 dis = std::distance(re.nums.rbegin(), it);
        re.nums.erase(re.nums.begin() + i64(re.nums.size() - dis), re.nums.end());
    } else {
        re.sign = sign;
        re.nums = tmp_.nums + other.nums;
    }
    re.squeeze();
    return re;
}

Number Number::operator-(const Number &other) const {
    other.sign = !other.sign;
    auto re = *this + other;
    other.sign = !other.sign;
    return re;
}

size_t next_pow2(size_t n) {
    if (n == 0) {
        return 0;
    }
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return ++n;
}

constexpr bool is_pow2(const unsigned n) {
    return n && !(n & (n - 1));
}

void fft(std::vector<std::complex<double>> &x) {/*NOLINT*/
    static constexpr double pi = 3.1415926535897932384626433832795;

    // in the event that an array was passed in with a non-power-of-two length.
    const size_t len = x.size();

    // base case
    if (len == 1) {
        x = std::vector<std::complex<double>>{x[0]};
        return;
    }

    // Partition the input list into evenly-index and oddly-indexed elements.
    const size_t halfLen = len / 2;

    std::vector<std::complex<double>> evens;
    evens.reserve(halfLen);
    std::vector<std::complex<double>> odds;
    odds.reserve(halfLen);

    for (size_t i = 0; i < halfLen; ++i) {
        // zero-pad both partitions if the input array was not at the
        // required radix=2 capacity
        if (i < x.size()) {
            evens.push_back(x[2 * i]);
            odds.push_back(x[2 * i + 1]);
        } else {
            evens[i] = odds[i] = std::complex<double>{0};
        }
    }

    // TODO: Fix the recursion!
    fft(evens);
    fft(odds);

    // combine the even and odd partitions
    const auto nf = (double) len;

    for (size_t k = 0; k < halfLen; ++k) {
        const auto kf = (double) k;
        const double w = -2.0 * kf * pi / nf;
        const std::complex<double> wk{std::cos(w), std::sin(w)};

        x[k] = evens[k] + (wk * odds[k]);
        x[k + halfLen] = evens[k] - (wk * odds[k]);
    }
}

void ifft(std::vector<std::complex<double>> &x) {
    for (std::complex<double> &e: x) {
        e = std::conj(e);
    }

    fft(x);

    const auto len = static_cast<double>(x.size());

    for (std::complex<double> &e: x) {
        e = std::conj(e) / len;
    }
}

std::vector<std::complex<double>> create_fft_table(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
    // Create a list of complex numbers with interleaved values from the two
    // input numbers. The output list must have a length that's a power of 2.
    size_t aLen = a.size();
    size_t bLen = b.size();
    // Ensure the Cooley-Tukey algorithm has its radix=2 requirement fulfilled.
    // Add a few digits to the end so the algorithm has room for overflow.
    size_t size = aLen + bLen;
    if (!is_pow2(size)) {
        size = next_pow2(size);
    }
    std::vector<std::complex<double>> ret;
    ret.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        // Add some zero-padding to the output list if necessary
        const uint8_t aVal = i < aLen ? a[i] : 0;
        const uint8_t bVal = i < bLen ? b[i] : 0;
        ret.emplace_back((double) aVal, (double) bVal);
    }
    return ret;
}

void convolute_fft(std::vector<std::complex<double>> &fftTable) {
    const size_t fftSize = fftTable.size();

    // transform.
    std::vector<std::complex<double>> transforms{fftTable};
    fft(transforms);

    // point-wise multiplication in frequency domain.
    for (size_t i = 0; i < fftSize; ++i) {
        // extract the individual transformed signals from the composed one.
        const std::complex<double> &ti = transforms[i];
        const std::complex<double> &&tc = std::conj(transforms[-i % fftSize]);

        // perform convolution
        const std::complex<double> x1 = ti + tc;
        const std::complex<double> x2 = ti - tc;
        const std::complex<double> x3 = x1 * x2;

        // avoid pedantic compilers
        constexpr auto rotation = double{0.25};

        fftTable[i] = std::complex<double>{x3.imag(), -x3.real()} * rotation;
    }
}

Number mul_strassen(const Number &a, const Number &b) {
    // Default type is double. Use floats if they perform well enough.

    // building a complex signal with the information of both signals.
    std::vector<std::complex<double>> fftTable = create_fft_table(a.nums, b.nums);

    convolute_fft(fftTable);
    ifft(fftTable);

    const std::vector<std::complex<double>> &inverses = fftTable;

    Number ret;
    ret.nums.reserve(inverses.size());
    ret.sign = !(a.sign xor b.sign);

    for (size_t i = 0, c = 0; i < inverses.size(); ++i) {
        // drop imaginary part of the number
        const double x = inverses[i].real();

        // round to an integer
        const auto ci = (u64) ((double) c + std::floor(x + 0.5));

        ret.nums.push_back(ci % 10);

        // carry propagation
        c = (ci / 10);
    }

    // trim trailing zeroes from the most-significant digits
    size_t numZeroes = 0;

    for (size_t i = ret.nums.size(); i-- > 0;) {
        if (ret.nums[i]) {
            break;
        }
        ++numZeroes;
    }

    ret.nums.resize(ret.nums.size() - numZeroes);
    ret.power = a.power + b.power;
    ret.squeeze();
    return ret;
}

Number Number::operator*(const Number &other) const {
    if (isZero() || other.isZero())return zero;
    Number re;
    if (*this == one || *this == negative_one) {
        re = other;
        re.sign = !(sign xor other.sign);
        return re;
    }
    if (other == one || other == negative_one) {
        re = *this;
        re.sign = !(sign xor other.sign);
        return re;
    }
    if (is_pow_10()) {
        re.sign = !(sign xor other.sign);
        re.nums = other.nums;
        re.power = power + other.power;
        return re;
    }
    if (other.is_pow_10()) {
        re.sign = !(sign xor other.sign);
        re.nums = nums;
        re.power = power + other.power;
        return re;
    }
    if (degenerate() && other.degenerate()) {
        i64 num1 = to_int(), num2 = other.to_int();
        i64 num3 = num1 * num2;
        re = num3 > 0 ? Number(num3, POSITIVE, power + other.power)
                      : Number(-num3, NEGATIVE, power + other.power);
        return re;
    }
    return mul_strassen(*this, other);
}

Number Number::operator/(const Number &other) const {//check before use
    if (isZero())return zero;
    Number re;
    if (other == one || other == negative_one) {
        re = *this;
        re.sign = !(sign xor other.sign);
        re.squeeze();
        return re;
    }
    if (other.is_pow_10()) {
        re.nums = nums;
        re.sign = !(sign xor other.sign);
        re.power = power - other.power;
        re.squeeze();
        return re;
    }
    i64 pre = i64(std::max({precision, nums.size() * 2, other.nums.size() * 2}));
    i64 pad = 0;
    if ((nums.size() < other.nums.size()) || (nums.size() - other.nums.size() < pre)) {
        pad = i64(pre - nums.size() + other.nums.size());
    }
    Number a, b;
    a.nums = nums;
    b.nums = other.nums;
    a.power = pad;
    re.sign = !(sign xor other.sign);
    re.nums.reserve(pre);
    i64 tmp_ = 0;
    for (i64 i = 0; i < pad; ++i, ++tmp_) {
        b.power = pad - tmp_;
        if (b == a) {
            re.nums.push_back(1);
            std::reverse(re.nums.begin(), re.nums.end());
            return re;
        }
        if (b > a) {
            if (i) {
                re.nums.push_back(0);
                continue;
            }
            while (b > a) {
                tmp_++;
                b.power = pad - tmp_;
            }
        }
        uint8_t tmp = 0;
        while (b < a) {
            a -= b;
            tmp++;
            a.squeeze();
        }
        re.nums.push_back(tmp);
        if (a.isZero()) {
            std::reverse(re.nums.begin(), re.nums.end());
            return re;
        }
    }
    std::reverse(re.nums.begin(), re.nums.end());
    re.power = power - other.power - tmp_ + 1;
    re.squeeze();
    return re;
}

std::pair<Number, Number> Number::operator%(const Number &other) const {//check before use
    if (*this < other)return {zero, *this};
    Number l = *this / other;
    l.squeeze();
    std::cout << l * other << "\n";
    Number r = (*this) - (l * other);
    std::cout << r << "\n";
    r.squeeze();
    return {l, r};
}

void Number::operator+=(const Number &other) {
    if (isZero()) {
        *this = other;
        return;
    }
    if (other.isZero())return;
    *this = *this + other;
    squeeze();
}

void Number::operator-=(const Number &other) {
    other.sign = !other.sign;
    *this += other;
    other.sign = !other.sign;
    squeeze();
}

void Number::operator*=(const Number &other) {
    if (isZero())return;
    if (other.isZero()) {
        *this = zero;
        return;
    }
    *this = *this * other;
    squeeze();
}

void Number::operator/=(const Number &other) {//check before use
    *this = *this / other;
    squeeze();
}

void Number::operator%=(const Number &other) {//check before use
    *this = (*this % other).second;
    squeeze();
}

Number &Number::operator++() {
    *this += one;
    squeeze();
    return *this;
}

Number &Number::operator--() {
    *this -= one;
    squeeze();
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Number &n) {
    if (!n.sign)os << '-';
    if (n.isZero()) {
        if (Number::tag == Print_Tag::Fractional)
            os << "0.0";
        else
            os << 0;
        return os;
    }
    auto &v = n.nums;
    switch (Number::tag) {
        case Print_Tag::Scientific:
            if (v.size() == 1) {
                os << int(v[0]);
                if (n.power != 0)
                    os << 'e' << n.power;
                return os;
            }
            os << int(*v.rbegin()) << '.';
            for (auto it = v.rbegin() + 1; it != v.rend(); ++it) {
                os << int(*it);
            }
            if (i64(n.power + v.size() - 1) != 0)
                os << 'e' << i64(n.power + v.size() - 1);
            break;
        case Print_Tag::Fractional:
            if (n.power >= 0) {
                os << n.nums;
                for (i64 i = 0; i < n.power; ++i) {
                    os << 0;
                }
                os << ".0";
            } else {
                if (v.size() > -n.power) {
                    u64 loc = v.size() + n.power, i = 0;
                    for (auto it = v.rbegin(); it != v.rend(); ++it, ++i) {
                        if (i == loc)os << '.';
                        os << int(*it);
                    }
                } else {
                    u64 l = -n.power - v.size();
                    os << "0.";
                    for (u64 i = 0; i < l; ++i) {
                        os << 0;
                    }
                    os << n.nums;
                }
            }
            break;
        case Print_Tag::Decimal:
            if (n.power >= 0) {
                os << n.nums;
                for (i64 i = 0; i < n.power; ++i) {
                    os << 0;
                }
            } else {
                if (v.size() > -n.power) {
                    u64 loc = v.size() + n.power, i = 0;
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

bool Number::isZero() const {
    return nums.empty();
}

void Number::squeeze() {
    if (nums.empty()) {
        power = 0;
        sign = POSITIVE;
        return;
    }
    if (nums[0] != 0)return;
    auto it = std::find_if(nums.begin(), nums.end(), [](uint8_t i) { return i != 0; });
    if (it == nums.end()) {
        *this = zero;
        return;
    }
    i64 dis = std::distance(nums.begin(), it);
    nums.erase(nums.begin(), it);
    power += dis;
}

void Number::pad_zeros(const i64 count) {
    if (count <= 0)return;
    squeeze();
    std::vector<uint8_t> tmp(count, 0);
    nums.insert(nums.begin(), tmp.begin(), tmp.end());
    power -= count;
}

bool Number::degenerate() const {
    u64 l = nums.size();
    if (l > 10)return false;
    if (l < 10)return true;
    return *nums.rbegin() < 2;
}

i64 Number::to_int() const {
    std::stringstream ss;
    if (!sign)ss << '-';
    for (auto it = nums.rbegin(); it != nums.rend(); ++it) {/*NOLINT*/
        ss << int(*it);
    }
    return std::stoll(ss.str());
}

bool Number::is_pow_10() const {
    if (nums.size() != 1)return false;
    return nums[0] == 1;
}

