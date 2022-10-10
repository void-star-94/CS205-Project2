//
// Created by zhzhzh on 2022/10/4.
//
#include "zh_number.h"
#include <algorithm>
#include <stdexcept>
#include <complex>


const zh_number zero = {POSITIVE, Base::Decimal,
                        {}, 0};
const zh_number one = {POSITIVE, Base::Decimal,
                       {1}, 0};
const zh_number negative_one = {NEGATIVE, Base::Decimal,
                                {1}, 0};


//
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

void fft(std::vector<std::complex<double>> &x) {
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

zh_number mul_strassen(const zh_number &a, const zh_number &b) {
    // Default type is double. Use floats if they perform well enough.

    // building a complex signal with the information of both signals.
    std::vector<std::complex<double>> fftTable = create_fft_table(a.nums, b.nums);

    convolute_fft(fftTable);
    ifft(fftTable);

    const std::vector<std::complex<double>> &inverses = fftTable;

    zh_number ret;
    ret.nums.reserve(inverses.size());
    ret.base = a.base;
    ret.sign = !(a.sign xor b.sign);

    for (size_t i = 0, c = 0; i < inverses.size(); ++i) {
        // drop imaginary part of the number
        const double x = inverses[i].real();

        // round to an integer
        const auto ci = (uint64_t) ((double) c + std::floor(x + 0.5));

        ret.nums.push_back(ci % int(ret.base));

        // carry propagation
        c = (ci / int(ret.base));
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

std::ostream &operator<<(std::ostream &os, std::vector<uint8_t> &v) {
    for (auto it = v.rbegin(); it != v.rend(); ++it) {/*NOLINT*/
        os << int(*it);
    }
    return os;
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
    zh_number re;
    if (sign xor other.sign) {
        re.sign = (*this < other) ? other.sign : sign;
        re.nums = re.sign ? nums - other.nums : other.nums - nums;
    } else {
        re.sign = sign;
        re.nums = nums + other.nums;
    }
    re.power = power;
    re.squeeze();
    squeeze();
    other.squeeze();
    return re;
}

zh_number zh_number::operator-(zh_number &other) {
    other.squeeze();
    squeeze();
    other.sign = !other.sign;
    auto re = *this + other;
    other.sign = !other.sign;
    return re;
}

zh_number zh_number::operator*(zh_number &other) {
    squeeze();
    other.squeeze();
    return mul_strassen(*this, other);
}

zh_number zh_number::operator/(zh_number &other) {
    return one;
}

std::pair<zh_number, zh_number> zh_number::operator%(zh_number &other) {
    return {};
}

bool zh_number::isZero() {
    squeeze();
    return nums.empty();
}

void zh_number::squeeze() {
    if (base != Base::Decimal)return;
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
    int64_t dis = std::distance(nums.begin(), it);
    nums.erase(nums.begin(), it);
    power += dis;
}

void zh_number::pad_zeros(const int64_t count) {
    if (count == 0)return;
    squeeze();
    std::vector<uint8_t> tmp(count, 0);
    nums.insert(nums.begin(), tmp.begin(), tmp.end());
    power -= count;
}

std::ostream &operator<<(std::ostream &os, zh_number &n) {
    n.squeeze();
    if (!n.sign)os << '-';
    if (n.base != Base::Decimal) {
        os << prefix(n.base);
        os << n.nums;
        return os;
    }
    if (n.isZero()) {
        if (zh_number::tag == Print_Tag::Fractional)
            os << "0.0";
        else
            os << 0;
        return os;
    }
    auto &v = n.nums;
    switch (zh_number::tag) {
        case Print_Tag::Scientific:
            os << int(*v.rbegin()) << '.';
            for (auto it = v.rbegin() + 1; it != v.rend(); ++it) {
                os << int(*it);
            }
            os << 'e' << n.power + v.size() - 1;
            break;
        case Print_Tag::Fractional:
            if (n.power >= 0) {
                os << n.nums;
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
                    os << n.nums;
                }
            }
            break;
        case Print_Tag::Decimal:
            if (n.power >= 0) {
                os << n.nums;
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

std::strong_ordering zh_number::operator<=>(zh_number &other) {
    if (sign xor other.sign)
        return sign ? std::strong_ordering::greater : std::strong_ordering::less;
    squeeze();
    other.squeeze();
    uint64_t ll = nums.size() + power, rl = other.nums.size() + other.power;
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

bool zh_number::operator==(zh_number &other) {
    if (sign xor other.sign)return false;
    squeeze();
    other.squeeze();
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

void zh_number::operator+=(zh_number &other) {
    squeeze();
    other.squeeze();
    if (isZero()) {
        *this = other;
        return;
    }
    if (other.isZero())return;
    int64_t tmp = std::abs(power - other.power);
    if (power < other.power) {
        other.pad_zeros(tmp);
    } else if (power > other.power) {
        pad_zeros(tmp);
    }
    nums = nums + other.nums;
    squeeze();
    other.squeeze();
}

void zh_number::operator-=(zh_number &other) {
    squeeze();
    other.squeeze();
    other.sign = !other.sign;
    *this += other;
    other.sign = !other.sign;
}



